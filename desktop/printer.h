/*
 * Copyright 2008 Adam Blokus <adamblokus@gmail.com>
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
 * 	Printer interface - contains plotter to use, functions for
 * initialization, handling pages and cleaning up.
*/

#ifndef GREYHOUND_DESKTOP_PRINTER_H
#define GREYHOUND_DESKTOP_PRINTER_H

#include "desktop/plotters.h"
#include "desktop/print.h"

/** Printer interface */
struct printer{
	const struct plotter_table *plotter;

	bool (*print_begin) (struct print_settings*);

	bool (*print_next_page)(void);

	void (*print_end)(void);
};

#endif
