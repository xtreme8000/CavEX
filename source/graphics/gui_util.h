/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	CavEX is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with CavEX.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <stdbool.h>
#include <stdint.h>

#include "../item/items.h"
#include "../platform/input.h"
#include "../platform/texture.h"

int gutil_control_icon(int x, enum input_button b, char* str);
void gutil_texquad_col(int x, int y, int tx, int ty, int sx, int sy, int width,
					   int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void gutil_texquad(int x, int y, int tx, int ty, int sx, int sy, int width,
				   int height);
void gutil_texquad_rt(int x, int y, int tx, int ty, int sx, int sy, int width,
					  int height);
void gutil_texquad_rt_any(int x, int y, float angle, int tx, int ty, int sx,
						  int sy, float width, float height);
void gutil_bg(void);
void gutil_reset_font(struct tex_gfx* tex);
int gutil_font_width(char* str, int scale);
void gutil_text(int x, int y, char* str, int scale, bool shadow);
void gutil_draw_item(struct item_data* item, int x, int y, int layer);

#endif
