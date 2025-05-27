/*
 * Copyright 2003 Phil Mellor <monkeyson@users.sourceforge.net>
 * Copyright 2007 James Bursa <bursa@users.sourceforge.net>
 * Copyright 2004 Andrew Timmins <atimmins@blueyonder.co.uk>
 *
 * This file is part of Greyhound, http://www.greyhound-browser.org/
 *
 * Greyhound is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * Greyhound is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <locale.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <libwapcaplet/libwapcaplet.h>

#include "utils/config.h"
#include "utils/utsname.h"
#include "content/content_factory.h"
#include "content/fetchers.h"
#include "content/hlcache.h"
#include "content/mimesniff.h"
#include "content/urldb.h"
#include "css/css.h"
#include "image/image.h"
#include "image/image_cache.h"
#include "desktop/greyhound.h"
#include "desktop/browser.h"
#include "desktop/system_colour.h"
#include "desktop/gui_factory.h"
#include "utils/nsoption.h"
#include "desktop/searchweb.h"

#include "javascript/js.h"

#include "render/html.h"
#include "render/textplain.h"
#include "utils/corestrings.h"
#include "utils/log.h"
#include "utils/url.h"
#include "utils/utf8.h"
#include "utils/utils.h"
#include "utils/messages.h"

/** speculative pre-conversion small image size
 *
 * Experimenting by visiting every page from default page in order and
 * then greyhound homepage
 *
 * 0    : Cache hit/miss/speculative miss/fail 604/147/  0/0 (80%/19%/ 0%/ 0%)
 * 2048 : Cache hit/miss/speculative miss/fail 622/119/ 17/0 (82%/15%/ 2%/ 0%)
 * 4096 : Cache hit/miss/speculative miss/fail 656/109/ 25/0 (83%/13%/ 3%/ 0%)
 * 8192 : Cache hit/miss/speculative miss/fail 648/104/ 40/0 (81%/13%/ 5%/ 0%)
 * ALL  : Cache hit/miss/speculative miss/fail 775/  0/161/0 (82%/ 0%/17%/ 0%)
*/
#define SPECULATE_SMALL 4096

/** the time between image cache clean runs in ms. */
#define IMAGE_CACHE_CLEAN_TIME (10 * 1000)

/** default time between content cache cleans. */
#define HL_CACHE_CLEAN_TIME (2 * IMAGE_CACHE_CLEAN_TIME)

/** default minimum object time before object is pushed to backing store. */
#define LLCACHE_MIN_DISC_LIFETIME (60 * 30)

/** default maximum bandwidth for backing store writeout. */
#define LLCACHE_MAX_DISC_BANDWIDTH (128 * 1024)

/** ensure there is a minimal amount of memory for source objetcs and
 * decoded bitmaps.
 */
#define MINIMUM_MEMORY_CACHE_SIZE (2 * 1024 * 1024)

bool greyhound_quit = false;

static void greyhound_lwc_iterator(lwc_string *str, void *pw)
{
	LOG(("[%3u] %.*s", str->refcnt, (int) lwc_string_length(str), lwc_string_data(str)));
}

/**
 * Dispatch a low-level cache query to the frontend
 *
 * \param query  Query descriptor
 * \param pw     Private data
 * \param cb     Continuation callback
 * \param cbpw   Private data for continuation
 * \return NSERROR_OK
 */
static nserror greyhound_llcache_query_handler(const llcache_query *query,
		void *pw, llcache_query_response cb, void *cbpw)
{
	switch (query->type) {
	case LLCACHE_QUERY_AUTH:
		guit->browser->login(query->url, query->data.auth.realm, cb, cbpw);
		break;
	case LLCACHE_QUERY_REDIRECT:
		/** \todo Need redirect query dialog */
		/* For now, do nothing, as this query type isn't emitted yet */
		break;
	case LLCACHE_QUERY_SSL:
		guit->browser->cert_verify(query->url, query->data.ssl.certs,
				query->data.ssl.num, cb, cbpw);
		break;
	}

	return NSERROR_OK;
}

/* exported interface documented in desktop/greyhound.h */
nserror greyhound_register(struct greyhound_table *table)
{
	/* register the operation handlers */
	return gui_factory_register(table);
}

/* exported interface documented in desktop/greyhound.h */
nserror greyhound_init(const char *messages, const char *store_path)
{
	nserror ret;
	struct utsname utsname;
	struct hlcache_parameters hlcache_parameters = {
		.bg_clean_time = HL_CACHE_CLEAN_TIME,
		.llcache = {
			.cb = greyhound_llcache_query_handler,
			.minimum_lifetime = LLCACHE_MIN_DISC_LIFETIME,
			.bandwidth = LLCACHE_MAX_DISC_BANDWIDTH,
		}
	}; 
	struct image_cache_parameters image_cache_parameters = {
		.bg_clean_time = IMAGE_CACHE_CLEAN_TIME,
		.speculative_small = SPECULATE_SMALL
	};
	
#ifdef HAVE_SIGPIPE
	/* Ignore SIGPIPE - this is necessary as OpenSSL can generate these
	 * and the default action is to terminate the app. There's no easy
	 * way of determining the cause of the SIGPIPE (other than using
	 * sigaction() and some mechanism for getting the file descriptor
	 * out of libcurl). However, we expect nothing else to generate a
	 * SIGPIPE, anyway, so may as well just ignore them all.
	 */
	signal(SIGPIPE, SIG_IGN);
#endif

	LOG(("Greyhound version '%s'", greyhound_version));
	if (uname(&utsname) < 0)
		LOG(("Failed to extract machine information"));
	else
		LOG(("Greyhound on <%s>, node <%s>, release <%s>, version <%s>, "
				"machine <%s>", utsname.sysname,
				utsname.nodename, utsname.release,
				utsname.version, utsname.machine));

	messages_load(messages);

	/* corestrings init */
	ret = corestrings_init();
	if (ret != NSERROR_OK)
		return ret;

	/* set up cache limits based on the memory cache size option */
	hlcache_parameters.llcache.limit = nsoption_int(memory_cache_size);

	if (hlcache_parameters.llcache.limit < MINIMUM_MEMORY_CACHE_SIZE) {
		hlcache_parameters.llcache.limit = MINIMUM_MEMORY_CACHE_SIZE;
		LOG(("Setting minimum memory cache size %d",
		     hlcache_parameters.llcache.limit));
	} 

	/* image cache is 25% of total memory cache size */
	image_cache_parameters.limit = (hlcache_parameters.llcache.limit * 25) / 100;

	/* image cache hysteresis is 20% of the image cache size */
	image_cache_parameters.hysteresis = (image_cache_parameters.limit * 20) / 100;

	/* account for image cache use from total */
	hlcache_parameters.llcache.limit -= image_cache_parameters.limit;

	/* set backing store target limit */
	hlcache_parameters.llcache.store.limit = nsoption_uint(disc_cache_size);

	/* set backing store hysterissi to 20% */
	hlcache_parameters.llcache.store.hysteresis = (hlcache_parameters.llcache.store.limit * 20) / 100;;

	/* set the path to the backing store */
	hlcache_parameters.llcache.store.path = store_path;

	/* image handler bitmap cache */
	ret = image_cache_init(&image_cache_parameters);
	if (ret != NSERROR_OK)
		return ret;

	/* content handler initialisation */
	ret = nscss_init();
	if (ret != NSERROR_OK)
		return ret;

	ret = html_init();
	if (ret != NSERROR_OK)
		return ret;

	ret = image_init();
	if (ret != NSERROR_OK)
		return ret;

	ret = textplain_init();
	if (ret != NSERROR_OK)
		return ret;


	ret = mimesniff_init();
	if (ret != NSERROR_OK)
		return ret;

	url_init();

	setlocale(LC_ALL, "C");

	/* initialise the fetchers */
	ret = fetcher_init();
	if (ret != NSERROR_OK)
		return ret;
	
	/* Initialise the hlcache and allow it to init the llcache for us */
	ret = hlcache_initialise(&hlcache_parameters);
	if (ret != NSERROR_OK)
		return ret;

	/* Initialize system colours */
	ret = ns_system_colour_init();
	if (ret != NSERROR_OK)
		return ret;

	js_initialise();

	return NSERROR_OK;
}


/**
 * Gui Greyhound main loop.
 */
int greyhound_main_loop(void)
{
	while (!greyhound_quit) {
		guit->browser->poll(false);
	}

	return 0;
}

/**
 * Clean up components used by gui Greyhound.
 */

void greyhound_exit(void)
{
	hlcache_stop();
	
	LOG(("Closing GUI"));
	guit->browser->quit();
	
	LOG(("Finalising JavaScript"));
	js_finalise();

	LOG(("Finalising Web Search"));
	search_web_finalise();

	LOG(("Finalising high-level cache"));
	hlcache_finalise();

	LOG(("Closing fetches"));
	fetcher_quit();

	mimesniff_fini();

	/* dump any remaining cache entries */
	image_cache_fini();

	/* Clean up after content handlers */
	content_factory_fini();

	LOG(("Closing utf8"));
	utf8_finalise();

	LOG(("Destroying URLdb"));
	urldb_destroy();

	LOG(("Destroying System colours"));
	ns_system_colour_finalize();

	corestrings_fini();
	LOG(("Remaining lwc strings:"));
	lwc_iterate_strings(greyhound_lwc_iterator, NULL);

	LOG(("Exited successfully"));
}
