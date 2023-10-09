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

#include <assert.h>
#include <m-lib/m-string.h>
#include <stdlib.h>

#include "../config.h"
#include "../game/game_state.h"
#include "../graphics/gui_util.h"
#include "../lodepng/lodepng.h"
#include "texture.h"

struct tex_gfx texture_fog;
struct tex_gfx texture_terrain;
struct tex_gfx texture_items;
struct tex_gfx texture_font;
struct tex_gfx texture_anim;
struct tex_gfx texture_gui_inventory;
struct tex_gfx texture_gui2;
struct tex_gfx texture_controls;
struct tex_gfx texture_pointer;
struct tex_gfx texture_clouds;
struct tex_gfx texture_sun;
struct tex_gfx texture_moon;

struct tex_gfx texture_mob_char;

struct tex_gfx texture_armor_chain1;
struct tex_gfx texture_armor_chain2;
struct tex_gfx texture_armor_cloth1;
struct tex_gfx texture_armor_cloth2;
struct tex_gfx texture_armor_gold1;
struct tex_gfx texture_armor_gold2;
struct tex_gfx texture_armor_iron1;
struct tex_gfx texture_armor_iron2;
struct tex_gfx texture_armor_diamond1;
struct tex_gfx texture_armor_diamond2;

#define distance_2d(x1, y1, x2, y2)                                            \
	(((x1) - (x2)) * ((x1) - (x2)) + ((y1) - (y2)) * ((y1) - (y2)))

static void gen_texture_fog(uint8_t* img, size_t size) {
	for(size_t y = 0; y < size; y++) {
		for(size_t x = 0; x < size; x++) {
			float d = (sqrt(distance_2d(size / 2.0F, size / 2.0F, x + 0.5F,
										y + 0.5F))
					   - (size / 2.0F - 9.0F))
				/ 8.0F;

			uint8_t* pixel = img + (x + y * size) * 4;
			pixel[0] = pixel[1] = pixel[2]
				= roundf(glm_clamp(d * 255.0F, 0.0F, 255.0F));
			pixel[3] = 255;
		}
	}
}

void tex_init() {
	tex_init_pre();

	size_t w, h;
	void* output = tex_atlas_block("terrain.png", &w, &h);
	if(output)
		tex_gfx_load(&texture_terrain, output, w, h, TEX_FMT_RGBA16, false);

	tex_gfx_load_file(&texture_font, "default.png", TEX_FMT_I8, false);
	gutil_reset_font(&texture_font);

	tex_gfx_load_file(&texture_anim, "anim.png", TEX_FMT_RGBA32, false);
	tex_gfx_load_file(&texture_gui_inventory, "gui/inventory.png",
					  TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_gui2, "gui_2.png", TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_items, "items.png", TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_controls, "controls.png", TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_pointer, "pointer.png", TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_clouds, "environment/clouds.png", TEX_FMT_IA4,
					  false);
	tex_gfx_load_file(&texture_sun, "terrain/sun.png", TEX_FMT_RGB16, false);
	tex_gfx_load_file(&texture_moon, "terrain/moon.png", TEX_FMT_RGB16, false);

	tex_gfx_load_file(&texture_armor_chain1, "armor/chain_1.png",
					  TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_armor_chain2, "armor/chain_2.png",
					  TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_armor_cloth1, "armor/cloth_1.png",
					  TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_armor_cloth2, "armor/cloth_2.png",
					  TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_armor_gold1, "armor/gold_1.png", TEX_FMT_RGBA16,
					  false);
	tex_gfx_load_file(&texture_armor_gold2, "armor/gold_2.png", TEX_FMT_RGBA16,
					  false);
	tex_gfx_load_file(&texture_armor_iron1, "armor/iron_1.png", TEX_FMT_RGBA16,
					  false);
	tex_gfx_load_file(&texture_armor_iron2, "armor/iron_2.png", TEX_FMT_RGBA16,
					  false);
	tex_gfx_load_file(&texture_armor_diamond1, "armor/diamond_1.png",
					  TEX_FMT_RGBA16, false);
	tex_gfx_load_file(&texture_armor_diamond2, "armor/diamond_2.png",
					  TEX_FMT_RGBA16, false);

	tex_gfx_load_file(&texture_mob_char, "mob/char.png", TEX_FMT_RGBA16, false);

	size_t fog_size = 128;
	uint8_t* fog = malloc(fog_size * fog_size * 4);
	gen_texture_fog(fog, fog_size);
	tex_gfx_load(&texture_fog, fog, fog_size, fog_size, TEX_FMT_I8, true);
}

uint8_t* tex_read(const char* filename, size_t* width, size_t* height) {
	assert(filename && width && height);

	string_t tmp;
	string_init_printf(
		tmp, "%s/%s",
		config_read_string(&gstate.config_user, "paths.texturepack", "assets"),
		filename);

	uint8_t* img;
	unsigned w, h;
	if(lodepng_decode32_file(&img, &w, &h, string_get_cstr(tmp))) {
		string_clear(tmp);
		return NULL;
	}

	string_clear(tmp);

	*width = w;
	*height = h;

	return img;
}

void tex_gfx_load_file(struct tex_gfx* tex, const char* filename,
					   enum tex_format type, bool linear) {
	assert(filename);

	size_t width, height;
	void* img = tex_read(filename, &width, &height);

	if(!img)
		return;

	tex_gfx_load(tex, img, width, height, type, linear);
}

#ifdef PLATFORM_WII
#include "wii/texture.c"
#endif

#ifdef PLATFORM_PC
#include "pc/texture.c"
#endif
