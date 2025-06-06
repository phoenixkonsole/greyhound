/*
 * Copyright 2011 Chris Young <chris@unsatisfactorysoftware.co.uk>
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

#include "utils/testament.h"

/* NB: AmigaOS revision numbers start at 1 (not 0) and are monotonically
 * incremental (v1.20 is higher than v1.3 and not the same as v1.2).
 * Consequently, this version pair may not match the user-facing one in
 * desktop/version.c.
 */
#define GREYHOUND_VERSION_MAJOR "3"
#if defined(CI_BUILD)
#define GREYHOUND_VERSION_MINOR CI_BUILD
#else
#define GREYHOUND_VERSION_MINOR "2"
#endif

#include "utils/testament.h"

static const char * const greyhound_version = "3.2";
static const int greyhound_version_major = 3;
static const int greyhound_version_minor = 2;

static const char version[] = "\0$VER: Greyhound 68k" GREYHOUND_VERSION_MAJOR "." GREYHOUND_VERSION_MINOR " (" __DATE__ ") NetSurf fork by Pascal Papara 2025 \0";

