/*
 * Copyright 2006 Richard Wilson <info@tinct.net>
 * Copyright 2008 Sean Fox <dyntryx@gmail.com>
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
 * Content for image/bmp (interface).
 */

#ifndef _GREYHOUND_IMAGE_BMP_H_
#define _GREYHOUND_IMAGE_BMP_H_

#include <libnsbmp.h>

#include "image/bitmap.h"

extern bmp_bitmap_callback_vt bmp_bitmap_callbacks; /** Only to be used by ICO code.  */

nserror nsbmp_init(void);

#endif
