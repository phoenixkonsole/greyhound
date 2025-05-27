/*
 * Copyright 2014 vincent Sanders <vince@greyhound-browser.org>
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
 * Interface to system colour values.
 */

#ifndef _GREYHOUND_DESKTOP_SYSTEM_COLOUR_H_
#define _GREYHOUND_DESKTOP_SYSTEM_COLOUR_H_

#include <libcss/libcss.h>

#include "utils/errors.h"
#include "desktop/plot_style.h"

/** css callback to obtain named system colours. */
css_error ns_system_colour(void *pw, lwc_string *name, css_color *color);

/** Obtain a named system colour from a frontend. */
colour ns_system_colour_char(const char *name);

nserror ns_system_colour_init(void);
void ns_system_colour_finalize(void);

#endif
