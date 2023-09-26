/*
	Copyright (c) 2023 ByteBit/xtreme8000

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

#include <string.h>

#include "../platform/gfx.h"
#include "gui_util.h"
#include "render_block.h"
#include "texture_atlas.h"

int gutil_control_icon(int x, enum input_button b, char* str) {
	int symbol, symbol_help;
	enum input_category category;

	if(!input_symbol(b, &symbol, &symbol_help, &category))
		return 0;

	gfx_bind_texture(&texture_controls);
	int scale = 32;
	int text_scale = 10;

	gutil_texquad(x, gfx_height() - scale * 8 / 5, (symbol_help % 8) * 32,
				  (symbol_help / 8) * 32 * 2, 32, 32 * 2, scale, scale);
	gutil_text(x + scale + text_scale / 2,
			   gfx_height() - scale * 8 / 5 + (scale - text_scale) / 2, str,
			   text_scale, true);
	return scale + text_scale + gutil_font_width(str, text_scale);
}

void gutil_texquad_col(int x, int y, int tx, int ty, int sx, int sy, int width,
					   int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
	gfx_draw_quads(
		4,
		(int16_t[]) {x, y, -2, x + width, y, -2, x + width, y + height, -2, x,
					 y + height, -2},
		(uint8_t[]) {r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a},
		(uint16_t[]) {tx, ty, tx + sx, ty, tx + sx, ty + sy, tx, ty + sy});
}

void gutil_texquad(int x, int y, int tx, int ty, int sx, int sy, int width,
				   int height) {
	gutil_texquad_col(x, y, tx, ty, sx, sy, width, height, 0xFF, 0xFF, 0xFF,
					  0xFF);
}

void gutil_texquad_rt(int x, int y, int tx, int ty, int sx, int sy, int width,
					  int height) {
	gfx_draw_quads(
		4,
		(int16_t[]) {x, y, -2, x + width, y, -2, x + width, y + height, -2, x,
					 y + height, -2},
		(uint8_t[]) {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
					 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		(uint16_t[]) {tx, ty + sy, tx, ty, tx + sx, ty, tx + sx, ty + sy});
}

void gutil_texquad_rt_any(int x, int y, float angle, int tx, int ty, int sx,
						  int sy, float width, float height) {
	width *= 0.707107F; // 1 / sqrt(2)
	height *= 0.707107F;
	angle -= glm_rad(45.0F);

	gfx_draw_quads(
		4,
		(int16_t[]) {x + sinf(angle) * width, y - cosf(angle) * height, -2,
					 x + cosf(angle) * width, y + sinf(angle) * height, -2,
					 x - sinf(angle) * width, y + cosf(angle) * height, -2,
					 x - cosf(angle) * width, y - sinf(angle) * height, -2},
		(uint8_t[]) {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
					 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		(uint16_t[]) {tx, ty, tx + sx, ty, tx + sx, ty + sy, tx, ty + sy});
}

void gutil_bg() {
	gfx_bind_texture(&texture_terrain);

	int scale = 16 * 4;
	int cx = (gfx_width() + scale - 1) / scale;
	int cy = (gfx_height() + scale - 1) / scale;

	uint8_t tex = tex_atlas_lookup(TEXAT_DIRT);

	uint8_t s = TEX_OFFSET(TEXTURE_X(tex));
	uint8_t t = TEX_OFFSET(TEXTURE_Y(tex));

	for(int y = 0; y < cy; y++) {
		for(int x = 0; x < cx; x++) {
			gutil_texquad_col(x * scale, y * scale, s, t, 16, 16, scale, scale,
							  0x40, 0x40, 0x40, 0xFF);
		}
	}
}

static const uint8_t font_char_width[256] = {
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 1, 4, 5, 5, 5, 5, 2, 4, 4, 4, 5,
	1, 5, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 4, 5, 4, 5, 6, 5,
	5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 3, 5, 3, 5, 5, 2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 5, 4, 2, 5,
	5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 4, 1, 4, 6, 5,
};

int gutil_font_width(char* str, int scale) {
	int x = 0;

	int skip = 0;

	while(*str) {
		if(*str == '\247')
			skip = 2;

		if(skip > 0) {
			skip--;
		} else {
			x += (font_char_width[(int)*str] + 1) * scale / 8;
		}

		str++;
	}

	return x;
}

static const uint8_t chat_colors[16][3] = {
	{0x00, 0x00, 0x00}, {0x00, 0x00, 0xAA}, {0x00, 0xAA, 0x00},
	{0x00, 0xAA, 0xAA}, {0xAA, 0x00, 0x00}, {0xAA, 0x00, 0xAA},
	{0xFF, 0xAA, 0x00}, {0xAA, 0xAA, 0xAA}, {0x55, 0x55, 0x55},
	{0x55, 0x55, 0xFF}, {0x55, 0xFF, 0x55}, {0x55, 0xFF, 0xFF},
	{0xFF, 0x55, 0x55}, {0xFF, 0x55, 0xFF}, {0xFF, 0xFF, 0x55},
	{0xFF, 0xFF, 0xFF},
};

void gutil_text(int x, int y, char* str, int scale, bool shadow) {
	gfx_bind_texture(&texture_font);

	int skip = 0;
	int col = 15;

	while(*str) {
		if(*str == '\247')
			skip = 2;

		if(skip > 0) {
			skip--;

			if(*str >= '0' && *str <= '9')
				col = *str - '0';

			if(*str >= 'a' && *str <= 'f')
				col = *str - 'a' + 10;
		} else {
			uint8_t tex_x = *str % 16 * 16;
			uint8_t tex_y = *str / 16 * 16;
			uint8_t width = (font_char_width[(int)*str] + 1) * scale / 8;

			if(shadow) {
				gfx_draw_quads(
					4,
					(int16_t[]) {x + scale / 8, y + scale / 8, -2,
								 x + scale + scale / 8, y + scale / 8, -2,
								 x + scale + scale / 8, y + scale + scale / 8,
								 -2, x + scale / 8, y + scale + scale / 8, -2},
					(uint8_t[]) {
						chat_colors[col][0] / 4, chat_colors[col][1] / 4,
						chat_colors[col][2] / 4, 0xFF, chat_colors[col][0] / 4,
						chat_colors[col][1] / 4, chat_colors[col][2] / 4, 0xFF,
						chat_colors[col][0] / 4, chat_colors[col][1] / 4,
						chat_colors[col][2] / 4, 0xFF, chat_colors[col][0] / 4,
						chat_colors[col][1] / 4, chat_colors[col][2] / 4, 0xFF},
					(uint16_t[]) {tex_x, tex_y, tex_x + 16, tex_y, tex_x + 16,
								  tex_y + 16, tex_x, tex_y + 16});
			}

			gfx_draw_quads(
				4,
				(int16_t[]) {x, y, -1, x + scale, y, -1, x + scale, y + scale,
							 -1, x, y + scale, -1},
				(uint8_t[]) {chat_colors[col][0], chat_colors[col][1],
							 chat_colors[col][2], 0xFF, chat_colors[col][0],
							 chat_colors[col][1], chat_colors[col][2], 0xFF,
							 chat_colors[col][0], chat_colors[col][1],
							 chat_colors[col][2], 0xFF, chat_colors[col][0],
							 chat_colors[col][1], chat_colors[col][2], 0xFF},
				(uint16_t[]) {tex_x, tex_y, tex_x + 16, tex_y, tex_x + 16,
							  tex_y + 16, tex_x, tex_y + 16});

			x += width;
		}

		str++;
	}
}

void gutil_draw_item(struct item_data* item, int x, int y, int layer) {
	assert(item);

	struct item* it = item_get(item);

	if(it) {
		mat4 model;
		glm_translate_make(model, (vec3) {x, y, 0});

		gfx_depth_range(0.1F * layer, 0.1F * (layer + 1));
		gfx_write_buffers(true, true, true);
		it->renderItem(it, item, model, true, R_ITEM_ENV_INVENTORY);
		gfx_write_buffers(true, false, false);
		gfx_depth_range(0.0F, 1.0F);

		if(it->has_damage && item->durability > 0) {
			gfx_texture(false);
			gutil_texquad_col(x + 4, y + 26, 0, 0, 0, 0, 26, 4, 0, 0, 0, 255);
			gutil_texquad_col(
				x + 4, y + 26, 0, 0, 0, 0,
				26 * (1.0F - (float)item->durability / (float)it->max_damage),
				2, 4, 251, 0, 255);
			gfx_texture(true);
		}

		if(item->count > 1) {
			char count[4];
			snprintf(count, sizeof(count), "%u", item->count);
			gutil_text(17 * 2 - gutil_font_width(count, 16) + x, y + 18, count,
					   16, true);
		}
	} else {
		char tmp[16];
		snprintf(tmp, sizeof(tmp), "%u", item->id);
		gutil_text(17 * 2 - gutil_font_width(tmp, 16) + x, y + 2, tmp, 16,
				   true);

		snprintf(tmp, sizeof(tmp), "%u", item->count);
		gutil_text(17 * 2 - gutil_font_width(tmp, 16) + x, y + 18, tmp, 16,
				   true);
	}
}