/*
 * Copyright 2003 Phil Mellor <monkeyson@users.sourceforge.net>
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

#ifndef _NETSURF_DESKTOP_NETSURF_H_
#define _NETSURF_DESKTOP_NETSURF_H_

#include <stdbool.h>
#include "utils/errors.h"

extern bool greyhound_quit;
extern const char * const greyhound_version;
extern const int greyhound_version_major;
extern const int greyhound_version_minor;

struct greyhound_table;

/**
 * Register operation table.
 *
 * @param table Greyhound operations table.
 * @return NSERROR_OK on success or error code on faliure.
 */
nserror greyhound_register(struct greyhound_table *table);

/**
 * Initialise greyhound core.
 *
 * @param messages path to translation mesage file.
 * @return NSERROR_OK on success or error code on faliure.
 */
nserror greyhound_init(const char *messages, const char *store_path);

/**
 * Run event loop.
 */
extern int greyhound_main_loop(void);

/**
 * Finalise Greyhound core
 */
extern void greyhound_exit(void);


#endif
