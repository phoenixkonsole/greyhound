/*
 * Copyright 2008 John-Mark Bell <jmb@greyhound-browser.org>
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
 * Locale-specific variants of various routines (Amiga-compatible implementation)
 */

#include <ctype.h>
#include "utils/locale.h"

/* Direct wrappers around <ctype.h> functions, no locale handling */
#define MAKELSCTYPE(x) \
int ls_##x(int c) { \
	return x(c); \
}

MAKELSCTYPE(isalpha)
MAKELSCTYPE(isalnum)
MAKELSCTYPE(iscntrl)
MAKELSCTYPE(isdigit)
MAKELSCTYPE(isgraph)
MAKELSCTYPE(islower)
MAKELSCTYPE(isprint)
MAKELSCTYPE(ispunct)
MAKELSCTYPE(isspace)
MAKELSCTYPE(isupper)
MAKELSCTYPE(isxdigit)
MAKELSCTYPE(tolower)
MAKELSCTYPE(toupper)

#undef MAKELSCTYPE
