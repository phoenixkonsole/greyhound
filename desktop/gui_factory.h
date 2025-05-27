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
 * Interface to gui interface factory
 */

#ifndef _NETSURF_DESKTOP_GUI_FACTORY_H_
#define _NETSURF_DESKTOP_GUI_FACTORY_H_

#include "desktop/gui.h"

/** The global operation table */
extern struct greyhound_table *guit;

/** register and verify global operation table
 *
 * @param gt The global table to register
 * @return NSERROR_OK on success or error code on faliure. On faliure
 * global table will not be initialised
 */
nserror gui_factory_register(struct greyhound_table *gt);

#endif
