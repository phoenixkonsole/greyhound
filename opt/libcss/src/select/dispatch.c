/*
 * This file is part of LibCSS
 * Licensed under the MIT License,
 *		  http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 John-Mark Bell <jmb@greyhound-browser.org>
 */

#include "select/dispatch.h"
#include "select/properties/properties.h"

/**
 * Dispatch table for properties, indexed by opcode
 */
#define PROPERTY_FUNCS(pname)				\
	css__cascade_##pname,				\
	css__set_##pname##_from_hint,			\
	css__initial_##pname,				\
	css__compose_##pname

struct prop_table prop_dispatch[CSS_N_PROPERTIES] = {
	{
		PROPERTY_FUNCS(azimuth),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(background_attachment),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(background_color),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(background_image),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(background_position),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(background_repeat),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_collapse),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_spacing),
		1,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(border_top_color),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_right_color),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_bottom_color),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_left_color),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_top_style),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_right_style),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_bottom_style),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_left_style),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_top_width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_right_width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_bottom_width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(border_left_width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(bottom),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(caption_side),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(clear),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(clip),
		0,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(color),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(content),
		0,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(counter_increment),
		0,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(counter_reset),
		0,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(cue_after),
		0,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(cue_before),
		0,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(cursor),
		1,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(direction),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(display),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(elevation),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(empty_cells),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(float),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(font_family),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(font_size),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(font_style),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(font_variant),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(font_weight),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(height),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(left),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(letter_spacing),
		1,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(line_height),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(list_style_image),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(list_style_position),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(list_style_type),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(margin_top),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(margin_right),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(margin_bottom),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(margin_left),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(max_height),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(max_width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(min_height),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(min_width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(orphans),
		1,
		GROUP_PAGE
	},
	{
		PROPERTY_FUNCS(outline_color),
		0,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(outline_style),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(outline_width),
		0,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(overflow_x),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(padding_top),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(padding_right),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(padding_bottom),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(padding_left),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(page_break_after),
		0,
		GROUP_PAGE
	},
	{
		PROPERTY_FUNCS(page_break_before),
		0,
		GROUP_PAGE
	},
	{
		PROPERTY_FUNCS(page_break_inside),
		1,
		GROUP_PAGE
	},
	{
		PROPERTY_FUNCS(pause_after),
		0,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(pause_before),
		0,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(pitch_range),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(pitch),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(play_during),
		0,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(position),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(quotes),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(richness),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(right),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(speak_header),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(speak_numeral),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(speak_punctuation),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(speak),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(speech_rate),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(stress),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(table_layout),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(text_align),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(text_decoration),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(text_indent),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(text_transform),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(top),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(unicode_bidi),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(vertical_align),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(visibility),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(voice_family),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(volume),
		1,
		GROUP_AURAL
	},
	{
		PROPERTY_FUNCS(white_space),
		1,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(widows),
		1,
		GROUP_PAGE
	},
	{
		PROPERTY_FUNCS(width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(word_spacing),
		1,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(z_index),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(opacity),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(break_after),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(break_before),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(break_inside),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(column_count),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(column_fill),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(column_gap),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(column_rule_color),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(column_rule_style),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(column_rule_width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(column_span),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(column_width),
		0,
		GROUP_NORMAL
	},
	{
		PROPERTY_FUNCS(writing_mode),
		0,
		GROUP_UNCOMMON
	},
	{
		PROPERTY_FUNCS(overflow_y),
		0,
		GROUP_NORMAL
	}
};
