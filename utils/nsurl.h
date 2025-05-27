/*
 * Copyright 2011 Michael Drake <tlsa@greyhound-browser.org>
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
 * Greyhound URL handling (interface).
 */

#ifndef _GREYHOUND_UTILS_NSURL_H_
#define _GREYHOUND_UTILS_NSURL_H_

#include <libwapcaplet/libwapcaplet.h>
#include "utils/errors.h"


/** Greyhound URL object */
typedef struct nsurl nsurl;


typedef enum nsurl_component {
	NSURL_SCHEME		= (1 << 0),
	NSURL_USERNAME		= (1 << 1),
	NSURL_PASSWORD		= (1 << 2),
	NSURL_CREDENTIALS	= NSURL_USERNAME | NSURL_PASSWORD,
	NSURL_HOST		= (1 << 3),
	NSURL_PORT		= (1 << 4),
	NSURL_AUTHORITY		= NSURL_CREDENTIALS | NSURL_HOST | NSURL_PORT,
	NSURL_PATH		= (1 << 5),
	NSURL_QUERY		= (1 << 6),
	NSURL_COMPLETE		= NSURL_SCHEME | NSURL_AUTHORITY |
				  NSURL_PATH | NSURL_QUERY,
	NSURL_FRAGMENT		= (1 << 7),
	NSURL_WITH_FRAGMENT	= NSURL_COMPLETE | NSURL_FRAGMENT
} nsurl_component;


/**
 * Create a Greyhound URL object from a URL string
 *
 * \param url_s	  String to create Greyhound URL from
 * \param url	  Returns a Greyhound URL
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, nothing will be returned in url.
 *
 * It is up to the client to call nsurl_destroy when they are finished with
 * the created object.
 */
nserror nsurl_create(const char * const url_s, nsurl **url);


/**
 * Increment the reference count to a Greyhound URL object
 *
 * \param url	  Greyhound URL to create another reference to
 * \return The Greyhound URL pointer to use as the copy
 *
 * Use this when copying a Greyhound URL into a persistent data structure.
 */
nsurl *nsurl_ref(nsurl *url);


/**
 * Drop a reference to a Greyhound URL object
 *
 * \param url	  Greyhound URL to drop reference to
 *
 * When the reference count reaches zero then the Greyhound URL will be destroyed
 */
void nsurl_unref(nsurl *url);


/**
 * Compare two URLs
 *
 * \param url1	  First Greyhound URL
 * \param url2	  Second Greyhound URL
 * \param parts	  The URL components to be compared
 * \param match	  Returns true if url1 and url2 matched, else false
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, match will be false.
 */
bool nsurl_compare(const nsurl *url1, const nsurl *url2, nsurl_component parts);


/**
 * Get URL (section) as a string, from a Greyhound URL object
 *
 * \param url	  Greyhound URL
 * \param parts	  The required URL components.
 * \param url_s	  Returns a url string
 * \param url_l	  Returns length of url_s
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, nothing will be returned in url_s or url_l.
 *
 * The string returned in url_s is owned by the client and it is up to them
 * to free it.  It includes a trailing '\0'.
 *
 * The length returned in url_l excludes the trailing '\0'.
 *
 * That the required URL components be consecutive is not enforced, however,
 * non-consecutive URL components generally make no sense.  The exception
 * is removal of credentials from a URL, such as for display in browser
 * window URL bar.  'NSURL_COMPLETE &~ NSURL_PASSWORD' would remove the
 * password from a complete URL.
 */
nserror nsurl_get(const nsurl *url, nsurl_component parts,
		char **url_s, size_t *url_l);


/**
 * Get part of a URL as a lwc_string, from a Greyhound URL object
 *
 * \param url	  Greyhound URL object
 * \param part	  The URL component required
 * \return the required component as an lwc_string, or NULL
 *
 * The caller owns the returned lwc_string and should call lwc_string_unref
 * when they are done with it.
 *
 * The valid values for the part parameter are:
 *    NSURL_SCHEME
 *    NSURL_USERNAME
 *    NSURL_PASSWORD
 *    NSURL_HOST
 *    NSURL_PORT
 *    NSURL_PATH
 *    NSURL_QUERY
 *    NSURL_FRAGMENT
 */
lwc_string *nsurl_get_component(const nsurl *url, nsurl_component part);


/**
 * Enquire about the existence of componenets in a given URL
 *
 * \param url	  Greyhound URL object
 * \param part	  The URL components confirm existence of
 * \return true iff the component in question exists in url
 *
 * The valid values for the part parameter are:
 *    NSURL_SCHEME
 *    NSURL_USERNAME
 *    NSURL_PASSWORD
 *    NSURL_CREDENTIALS
 *    NSURL_HOST
 *    NSURL_PORT
 *    NSURL_PATH
 *    NSURL_QUERY
 *    NSURL_FRAGMENT
 */
bool nsurl_has_component(const nsurl *url, nsurl_component part);


/**
 * Access a Greyhound URL object as a string
 *
 * \param url	  Greyhound URL to retrieve a string pointer for.
 * \return the required string
 *
 * The returned string is owned by the Greyhound URL object.  It will die
 * with the Greyhound URL object.  Keep a reference to the URL if you need it.
 *
 * The returned string has a trailing '\0'.
 */
const char *nsurl_access(const nsurl *url);


/**
 * Access a URL's path leaf as a string
 *
 * \param url	  Greyhound URL to retrieve a string pointer for.
 * \return the required string
 *
 * The returned string is owned by the Greyhound URL object.  It will die
 * with the Greyhound URL object.  Keep a reference to the URL if you need it.
 *
 * The returned string has a trailing '\0'.
 */
const char *nsurl_access_leaf(const nsurl *url);


/**
 * Find the length of a Greyhound URL object's URL, as returned by nsurl_access
 *
 * \param url	  Greyhound URL to find length of.
 * \return the required string
 *
 * The returned length excludes the trailing '\0'.
 */
size_t nsurl_length(const nsurl *url);


/**
 * Get a URL's hash value
 *
 * \param url	  Greyhound URL get hash value for.
 * \return the hash value
 */
uint32_t nsurl_hash(const nsurl *url);


/**
 * Join a base url to a relative link part, creating a new Greyhound URL object
 *
 * \param base	  Greyhound URL containing the base to join rel to
 * \param rel	  String containing the relative link part
 * \param joined  Returns joined Greyhound URL
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, nothing will be returned in join.
 *
 * It is up to the client to call nsurl_destroy when they are finished with
 * the created object.
 */
nserror nsurl_join(const nsurl *base, const char *rel, nsurl **joined);


/**
 * Create a Greyhound URL object without a fragment from a Greyhound URL
 *
 * \param url	  Greyhound URL to create new Greyhound URL from
 * \param no_frag Returns new Greyhound URL without fragment
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, nothing will be returned in no_frag.
 *
 * It is up to the client to call nsurl_destroy when they are finished with
 * the created object.
 */
nserror nsurl_defragment(const nsurl *url, nsurl **no_frag);


/**
 * Create a Greyhound URL object, adding a fragment to an existing URL object
 *
 * \param url	  Greyhound URL to create new Greyhound URL from
 * \param frag	  Fragment to add
 * \param new_url Returns new Greyhound URL without fragment
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, nothing will be returned in new_url.
 *
 * It is up to the client to call nsurl_destroy when they are finished with
 * the created object.
 *
 * Any fragment in url is replaced with frag in new_url.
 */
nserror nsurl_refragment(const nsurl *url, lwc_string *frag, nsurl **new_url);


/**
 * Create a Greyhound URL object, with query string replaced
 *
 * \param url	  Greyhound URL to create new Greyhound URL from
 * \param query	  Query string to use
 * \param new_url Returns new Greyhound URL with query string provided
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, nothing will be returned in new_url.
 *
 * It is up to the client to call nsurl_destroy when they are finished with
 * the created object.
 *
 * Any query component in url is replaced with query in new_url.
 */
nserror nsurl_replace_query(const nsurl *url, const char *query,
		nsurl **new_url);


/**
 * Create a Greyhound URL object for URL with parent location of an existing URL.
 *
 * \param url	  Greyhound URL to create new Greyhound URL from
 * \param new_url Returns new Greyhound URL with parent URL path
 * \return NSERROR_OK on success, appropriate error otherwise
 *
 * If return value != NSERROR_OK, nothing will be returned in new_url.
 *
 * It is up to the client to call nsurl_destroy when they are finished with
 * the created object.
 *
 * As well as stripping top most path segment, query and fragments are stripped.
 */
nserror nsurl_parent(const nsurl *url, nsurl **new_url);

#endif
