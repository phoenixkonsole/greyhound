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
 * Thumbail handling (interface).
 */

#ifndef _GREYHOUND_DESKTOP_THUMBNAIL_H_
#define _GREYHOUND_DESKTOP_THUMBNAIL_H_

#include <stdbool.h>
#include "utils/nsurl.h"
#include "utils/types.h"

struct hlcache_handle;
struct bitmap;


/**
 * Redraw a content for thumbnailing
 *
 * Calls the redraw function for the content, 
 *
 * \param  content  The content to redraw for thumbnail
 * \param  width    The thumbnail width
 * \param  height   The thumbnail height
 * \param  ctx      current redraw context
 * \return true if successful, false otherwise
 *
 * The thumbnail is guaranteed to be filled to its width/height extents, so
 * there is no need to render a solid background first.
 *
 * Units for width and height are pixels.
 */
bool thumbnail_redraw(struct hlcache_handle *content,
		int width, int height, const struct redraw_context *ctx);


/* In platform specific thumbnail.c. */
bool thumbnail_create(struct hlcache_handle *content, struct bitmap *bitmap,
		nsurl *url);

#endif
