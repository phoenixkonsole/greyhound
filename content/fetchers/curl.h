/*
 * Copyright 2007 Daniel Silverstone <dsilvers@digital-scurf.org>
 *
 * This file is part of Greyhound.
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
 * Fetching of data from a URL (Registration).
 */

#ifndef GREYHOUND_CONTENT_FETCHERS_FETCH_CURL_H
#define GREYHOUND_CONTENT_FETCHERS_FETCH_CURL_H

#include <curl/curl.h>

void fetch_curl_register(void);

/** Global cURL multi handle. */
extern CURLM *fetch_curl_multi;

#endif
