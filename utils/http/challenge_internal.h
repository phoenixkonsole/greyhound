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

#ifndef GREYHOUND_UTILS_HTTP_CHALLENGE_INTERNAL_H_
#define GREYHOUND_UTILS_HTTP_CHALLENGE_INTERNAL_H_

#include "utils/errors.h"
#include "utils/http/challenge.h"

nserror http__parse_challenge(const char **input, http_challenge **parameter);

#endif
