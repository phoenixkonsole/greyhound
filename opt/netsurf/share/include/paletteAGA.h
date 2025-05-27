/*
 * Copyright 2012 Michael Drake <tlsa@greyhound-browser.org>
 *
 * This file is part of libnsfb, http://www.greyhound-browser.org/
 * Licenced under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 *
 * This is the *internal* interface for the cursor. 
 */

#ifndef PALETTE_H
#define PALETTE_H 1

#include <stdint.h>
#include <limits.h>

#include "libnsfb.h"
#include "libnsfb_plot.h"

 
enum nsfb_palette_type_e {
	NSFB_PALETTE_EMPTY,     /**< empty palette object */
	NSFB_PALETTE_NSFB_8BPP, /**< libnsfb's own 8bpp palette */
	NSFB_PALETTE_CUBE_676,   /**< palette with 6x7x6 + 3 colors */
	NSFB_PALETTE_OTHER      /**< any other palette  */
};

struct nsfb_palette_s {
	enum nsfb_palette_type_e type; /**< Palette type */
	uint8_t last; /**< Last used palette index */
	nsfb_colour_t data[256]; /**< Palette for index modes */

	bool dither; /**< Whether to use error diffusion */
	struct {
		int width; /**< Length of error value buffer ring*/
		int current; /**< Current pos in ring buffer*/
		int *data; /**< Ring buffer error values */
		int data_len; /**< Max size of ring */
	} dither_ctx;
};
	

/** Create an empty palette object. */
bool nsfb_palette_new(struct nsfb_palette_s **palette, int width);

/** Free a palette object. */
void nsfb_palette_free(struct nsfb_palette_s *palette);

/** Init error diffusion for a plot. */
void nsfb_palette_dither_init(struct nsfb_palette_s *palette, int width);

/** Finalise error diffusion after a plot. */
void nsfb_palette_dither_fini(struct nsfb_palette_s *palette);

/** Generate cube 676 palette. */
void nsfb_palette_generate_cube_676(struct nsfb_palette_s *palette);

/** Find best palette match for given colour. */
uint8_t nsfb_palette_best_match_dither(struct nsfb_palette_s *palette, nsfb_colour_t c);



#endif /* PALETTE_H */
