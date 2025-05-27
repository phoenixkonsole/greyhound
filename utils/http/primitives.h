/*
 * Copyright 2010 John-Mark Bell <jmb@greyhound-browser.org>
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

#ifndef GREYHOUND_UTILS_HTTP_PRIMITIVES_H_
#define GREYHOUND_UTILS_HTTP_PRIMITIVES_H_

#include <libwapcaplet/libwapcaplet.h>

#include "utils/errors.h"

void http__skip_LWS(const char **input);

nserror http__parse_token(const char **input, lwc_string **value);

nserror http__parse_quoted_string(const char **input, lwc_string **value);

#endif
