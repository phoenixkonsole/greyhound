/*
 * Copyright 2009 Vincent Sanders <vince@simtec.co.uk>
 * Copyright 2010 Michael Drake <tlsa@greyhound-browser.org>
 *
 * This file is part of libnsfb, http://www.greyhound-browser.org/
 * Licenced under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 */

#include <stdbool.h>
#include <endian.h>
#include <stdlib.h>

#include "libnsfb.h"
#include "libnsfb_plot.h"
#include "libnsfb_plot_util.h"

#include "nsfb.h"
#include "plot.h"


#define UNUSED __attribute__((unused)) 

static inline uint32_t *get_xy_loc(nsfb_t *nsfb, int x, int y)
{
        return (void *)(nsfb->ptr + (y * nsfb->linelen) + (x << 2));
}

#if __BYTE_ORDER == __BIG_ENDIAN
static inline nsfb_colour_t pixel_to_colour(UNUSED nsfb_t *nsfb, uint32_t pixel)
{
	/* TODO: FIX */
        return (pixel >> 8) & ~0xFF000000U;
}

/* convert a colour value to a 32bpp pixel value ready for screen output */
static inline uint32_t colour_to_pixel(UNUSED nsfb_t *nsfb, nsfb_colour_t c)
{
	/* TODO: FIX */
        return (c << 8);
}
#else /* __BYTE_ORDER == __BIG_ENDIAN */
static inline nsfb_colour_t pixel_to_colour(UNUSED nsfb_t *nsfb, uint32_t pixel)
{
        return pixel | 0xFF000000U;
}

/* convert a colour value to a 32bpp pixel value ready for screen output */
static inline uint32_t colour_to_pixel(UNUSED nsfb_t *nsfb, nsfb_colour_t c)
{
        return c;
}
#endif

#define PLOT_TYPE uint32_t
#define PLOT_LINELEN(ll) ((ll) >> 2)

#include "32bpp-common.c"

const nsfb_plotter_fns_t _nsfb_32bpp_xbgr8888_plotters = {
        .line = line,
        .fill = fill,
        .point = point,
        .bitmap = bitmap,
        .bitmap_tiles = bitmap_tiles,
        .glyph8 = glyph8,
        .glyph1 = glyph1,
        .readrect = readrect,
};

/*
 * Local Variables:
 * c-basic-offset:8
 * End:
 */
