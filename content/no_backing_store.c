/*
 * Copyright 2014 Vincent Sanders <vince@greyhound-browser.org>
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

/** \file
 * Low-level resource cache null persistant storage implementation.
 */

#include "utils/nsurl.h"

#include "content/backing_store.h"


/* default to disabled backing store */
static nserror initialise(const struct llcache_store_parameters *parameters)
{
	return NSERROR_OK;
}

static nserror finalise(void)
{
	return NSERROR_OK;
}

static nserror store(nsurl *url,
		     enum backing_store_flags flags,
		     const uint8_t *data,
		     const size_t datalen)
{
	return NSERROR_SAVE_FAILED;
}

static nserror fetch(nsurl *url,
		     enum backing_store_flags *flags,
		     uint8_t **data_out,
		     size_t *datalen_out)
{
	return NSERROR_NOT_FOUND;
}

static nserror invalidate(nsurl *url)
{
	return NSERROR_NOT_FOUND;
}

static struct gui_llcache_table llcache_table = {
	.initialise = initialise,
	.finalise = finalise,
	.store = store,
	.fetch = fetch,
	.invalidate = invalidate,
};

struct gui_llcache_table *null_llcache_table = &llcache_table;
