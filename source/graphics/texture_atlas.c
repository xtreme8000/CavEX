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

#include <stddef.h>

#include "../platform/texture.h"
#include "texture_atlas.h"

static uint8_t global_atlas[TEXAT_MAX];

static int clamp_n(int x, int n) {
	if(x < 0)
		return 0;
	if(x > n - 1)
		return n - 1;
	return x;
}

void tex_atlas_reg(dict_atlas_src_t atlas, enum tex_atlas_entry name, uint8_t x,
				   uint8_t y) {
	dict_atlas_src_push_back(atlas,
							 (struct texture_entry) {
								 .name = name,
								 .x = x,
								 .y = y,
								 .bg.enable = false,
								 .colorize.enable = false,
							 });
}

void tex_atlas_reg_col(dict_atlas_src_t atlas, enum tex_atlas_entry name,
					   uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b) {
	dict_atlas_src_push_back(atlas,
							 (struct texture_entry) {
								 .name = name,
								 .x = x,
								 .y = y,
								 .bg.enable = false,
								 .colorize.enable = true,
								 .colorize.r = r,
								 .colorize.g = g,
								 .colorize.b = b,
							 });
}

void tex_atlas_reg_grass(dict_atlas_src_t atlas, enum tex_atlas_entry name,
						 uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b,
						 uint8_t bg_x, uint8_t bg_y) {
	dict_atlas_src_push_back(atlas,
							 (struct texture_entry) {
								 .name = name,
								 .x = x,
								 .y = y,
								 .bg.enable = true,
								 .bg.x = bg_x,
								 .bg.y = bg_y,
								 .colorize.enable = true,
								 .colorize.r = r,
								 .colorize.g = g,
								 .colorize.b = b,
							 });
}

void* tex_atlas_compute(dict_atlas_src_t atlas, uint8_t* atlas_dst,
						uint8_t* image, size_t width, size_t height) {
	assert(image && width >= 256 && width == height);

	int tile_size = width / 16;
	int border_scale = width / 256;

	uint8_t* output = malloc(width * height * 4);

	if(!output)
		return NULL;

	memset(output, 255, width * height * 4);

	dict_atlas_src_it_t it;
	dict_atlas_src_it(it, atlas);

	int current = 0;

	while(!dict_atlas_src_end_p(it)) {
		struct texture_entry* e = dict_atlas_src_ref(it);

		size_t current_x = (current % 14) * (tile_size + 2 * border_scale)
			+ 3 * border_scale;
		size_t current_y = (current / 14) * (tile_size + 2 * border_scale)
			+ 3 * border_scale;

		for(int y = -border_scale; y < tile_size + border_scale; y++) {
			for(int x = -border_scale; x < tile_size + border_scale; x++) {
				uint8_t* src_col = image
					+ ((clamp_n(x, tile_size) + e->x * tile_size)
					   + (clamp_n(y, tile_size) + e->y * tile_size) * width)
						* 4;
				uint8_t* dst_col
					= output + ((current_x + x) + (current_y + y) * width) * 4;

				if(e->colorize.enable) {
					dst_col[0] = src_col[0] * e->colorize.r / 255;
					dst_col[1] = src_col[1] * e->colorize.g / 255;
					dst_col[2] = src_col[2] * e->colorize.b / 255;
					dst_col[3] = src_col[3];
				} else {
					for(size_t k = 0; k < 4; k++)
						dst_col[k] = src_col[k];
				}

				if(e->bg.enable && dst_col[3] < 128) {
					uint8_t* src_col2 = image
						+ ((clamp_n(x, tile_size) + e->bg.x * tile_size)
						   + (clamp_n(y, tile_size) + e->bg.y * tile_size)
							   * width)
							* 4;
					for(size_t k = 0; k < 4; k++)
						dst_col[k] = src_col2[k];
				}
			}
		}

		atlas_dst[e->name] = TEXTURE_INDEX(current % 14, current / 14);

		current++;
		dict_atlas_src_next(it);
	}

	return output;
}

uint8_t tex_atlas_lookup(enum tex_atlas_entry name) {
	return global_atlas[name];
}

void* tex_atlas_block(const char* filename, size_t* width, size_t* height) {
	dict_atlas_src_t atlas;
	dict_atlas_src_init(atlas);

	tex_atlas_reg(atlas, TEXAT_STONE, 1, 0);
	tex_atlas_reg(atlas, TEXAT_DIRT, 2, 0);
	tex_atlas_reg(atlas, TEXAT_PLANKS, 4, 0);
	tex_atlas_reg(atlas, TEXAT_SLAB_STONE_SIDE, 5, 0);
	tex_atlas_reg(atlas, TEXAT_SLAB_STONE_TOP, 6, 0);
	tex_atlas_reg(atlas, TEXAT_BRICKS, 7, 0);
	tex_atlas_reg(atlas, TEXAT_TNT_SIDE, 8, 0);
	tex_atlas_reg(atlas, TEXAT_TNT_TOP, 9, 0);
	tex_atlas_reg(atlas, TEXAT_TNT_BOTTOM, 10, 0);
	tex_atlas_reg(atlas, TEXAT_COBWEB, 11, 0);
	tex_atlas_reg(atlas, TEXAT_ROSE, 12, 0);
	tex_atlas_reg(atlas, TEXAT_DANDELION, 13, 0);
	tex_atlas_reg(atlas, TEXAT_SAPLING_OAK, 15, 0);

	tex_atlas_reg(atlas, TEXAT_COBBLESTONE, 0, 1);
	tex_atlas_reg(atlas, TEXAT_BEDROCK, 1, 1);
	tex_atlas_reg(atlas, TEXAT_SAND, 2, 1);
	tex_atlas_reg(atlas, TEXAT_GRAVEL, 3, 1);
	tex_atlas_reg(atlas, TEXAT_LOG_OAK_SIDE, 4, 1);
	tex_atlas_reg(atlas, TEXAT_LOG_OAK_TOP, 5, 1);
	tex_atlas_reg(atlas, TEXAT_CAST_BLOCK_IRON, 6, 1);
	tex_atlas_reg(atlas, TEXAT_CAST_BLOCK_GOLD, 7, 1);
	tex_atlas_reg(atlas, TEXAT_CAST_BLOCK_DIAMOND, 8, 1);
	tex_atlas_reg(atlas, TEXAT_CHEST_TOP, 9, 1);
	tex_atlas_reg(atlas, TEXAT_CHEST_SIDE, 10, 1);
	tex_atlas_reg(atlas, TEXAT_CHEST_FRONT_SINGLE, 11, 1);
	tex_atlas_reg(atlas, TEXAT_MUSHROOM_RED, 12, 1);
	tex_atlas_reg(atlas, TEXAT_MUSHROOM_BROWN, 13, 1);

	tex_atlas_reg(atlas, TEXAT_ORE_GOLD, 0, 2);
	tex_atlas_reg(atlas, TEXAT_ORE_IRON, 1, 2);
	tex_atlas_reg(atlas, TEXAT_ORE_COAL, 2, 2);
	tex_atlas_reg(atlas, TEXAT_BOOKSHELF, 3, 2);
	tex_atlas_reg(atlas, TEXAT_COBBLESTONE_MOSSY, 4, 2);
	tex_atlas_reg(atlas, TEXAT_OBSIDIAN, 5, 2);
	tex_atlas_reg(atlas, TEXAT_CHEST_FRONT_1, 9, 2);
	tex_atlas_reg(atlas, TEXAT_CHEST_FRONT_2, 10, 2);
	tex_atlas_reg(atlas, TEXAT_WORKBENCH_TOP, 11, 2);
	tex_atlas_reg(atlas, TEXAT_FURNACE_FRONT, 12, 2);
	tex_atlas_reg(atlas, TEXAT_FURNACE_SIDE, 13, 2);
	tex_atlas_reg(atlas, TEXAT_DISPENSER_FRONT, 14, 2);

	tex_atlas_reg(atlas, TEXAT_SPONGE, 0, 3);
	tex_atlas_reg(atlas, TEXAT_GLASS, 1, 3);
	tex_atlas_reg(atlas, TEXAT_ORE_DIAMOND, 2, 3);
	tex_atlas_reg(atlas, TEXAT_ORE_REDSTONE, 3, 3);
	tex_atlas_reg_col(atlas, TEXAT_LEAVES_BIRCH, 4, 3, 128, 167, 85);
	tex_atlas_reg(atlas, TEXAT_DEADBUSH, 7, 3);
	tex_atlas_reg(atlas, TEXAT_CHEST_BACK_1, 9, 3);
	tex_atlas_reg(atlas, TEXAT_CHEST_BACK_2, 10, 3);
	tex_atlas_reg(atlas, TEXAT_WORKBENCH_SIDE_1, 11, 3);
	tex_atlas_reg(atlas, TEXAT_WORKBENCH_SIDE_2, 12, 3);
	tex_atlas_reg(atlas, TEXAT_FURNACE_FRONT_LIT, 13, 3);
	tex_atlas_reg(atlas, TEXAT_FURNACE_TOP, 14, 3);
	tex_atlas_reg(atlas, TEXAT_SAPLING_SPRUCE, 15, 3);

	tex_atlas_reg(atlas, TEXAT_SPAWNER, 1, 4);
	tex_atlas_reg(atlas, TEXAT_SNOW, 2, 4);
	tex_atlas_reg(atlas, TEXAT_GRASS_SIDE_SNOW, 4, 4);
	tex_atlas_reg(atlas, TEXAT_CACTUS_TOP, 5, 4);
	tex_atlas_reg(atlas, TEXAT_CACTUS_SIDE, 6, 4);
	tex_atlas_reg(atlas, TEXAT_CACTUS_BOTTOM, 7, 4);
	tex_atlas_reg(atlas, TEXAT_CLAY, 8, 4);
	tex_atlas_reg(atlas, TEXAT_REED, 9, 4);
	tex_atlas_reg(atlas, TEXAT_JUKBEBOX_SIDE, 10, 4);
	tex_atlas_reg(atlas, TEXAT_JUKBEBOX_TOP, 11, 4);
	tex_atlas_reg(atlas, TEXAT_SAPLING_BIRCH, 15, 4);

	tex_atlas_reg(atlas, TEXAT_TORCH, 0, 5);
	tex_atlas_reg(atlas, TEXAT_DOOR_WOOD_TOP, 1, 5);
	tex_atlas_reg(atlas, TEXAT_DOOR_IRON_TOP, 2, 5);
	tex_atlas_reg(atlas, TEXAT_LADDER, 3, 5);
	tex_atlas_reg(atlas, TEXAT_TRAPDOOR, 4, 5);
	tex_atlas_reg(atlas, TEXAT_FARMLAND_WET, 6, 5);
	tex_atlas_reg(atlas, TEXAT_FARMLAND_DRY, 7, 5);
	tex_atlas_reg(atlas, TEXAT_CROPS_0, 8, 5);
	tex_atlas_reg(atlas, TEXAT_CROPS_1, 9, 5);
	tex_atlas_reg(atlas, TEXAT_CROPS_2, 10, 5);
	tex_atlas_reg(atlas, TEXAT_CROPS_3, 11, 5);
	tex_atlas_reg(atlas, TEXAT_CROPS_4, 12, 5);
	tex_atlas_reg(atlas, TEXAT_CROPS_5, 13, 5);
	tex_atlas_reg(atlas, TEXAT_CROPS_6, 14, 5);
	tex_atlas_reg(atlas, TEXAT_CROPS_7, 15, 5);

	// tex_atlas_reg(atlas, "lever", 0, 6);
	tex_atlas_reg(atlas, TEXAT_DOOR_WOOD_BOTTOM, 1, 6);
	tex_atlas_reg(atlas, TEXAT_DOOR_IRON_BOTTOM, 2, 6);
	tex_atlas_reg(atlas, TEXAT_REDSTONE_TORCH_LIT, 3, 6);
	tex_atlas_reg(atlas, TEXAT_PUMPKIN_TOP, 6, 6);
	tex_atlas_reg(atlas, TEXAT_NETHERRACK, 7, 6);
	tex_atlas_reg(atlas, TEXAT_SOULSAND, 8, 6);
	tex_atlas_reg(atlas, TEXAT_GLOWSTONE, 9, 6);
	/*tex_atlas_reg(atlas, "piston_front_sticky", 10, 6);
	tex_atlas_reg(atlas, "piston_front", 11, 6);
	tex_atlas_reg(atlas, "piston_side", 12, 6);
	tex_atlas_reg(atlas, "piston_back", 13, 6);
	tex_atlas_reg(atlas, "piston_inner", 14, 6);*/

	tex_atlas_reg(atlas, TEXAT_RAIL_CURVED, 0, 7);
	tex_atlas_reg(atlas, TEXAT_REDSTONE_TORCH, 3, 7);
	tex_atlas_reg(atlas, TEXAT_LOG_SPRUCE_SIDE, 4, 7);
	tex_atlas_reg(atlas, TEXAT_LOG_BIRCH_SIDE, 5, 7);
	tex_atlas_reg(atlas, TEXAT_PUMPKIN_SIDE, 6, 7);
	tex_atlas_reg(atlas, TEXAT_PUMPKIN_FRONT, 7, 7);
	tex_atlas_reg(atlas, TEXAT_PUMPKIN_FRONT_LIT, 8, 7);
	tex_atlas_reg(atlas, TEXAT_CAKE_TOP, 9, 7);
	tex_atlas_reg(atlas, TEXAT_CAKE_SIDE, 10, 7);
	tex_atlas_reg(atlas, TEXAT_CAKE_SIDE_CUT, 11, 7);
	tex_atlas_reg(atlas, TEXAT_CAKE_BOTTOM, 12, 7);

	tex_atlas_reg(atlas, TEXAT_RAIL, 0, 8);
	// tex_atlas_reg(atlas, "repeater_off", 3, 8);
	tex_atlas_reg_col(atlas, TEXAT_LEAVES_SPRUCE, 4, 8, 97, 153, 97);
	tex_atlas_reg(atlas, TEXAT_BED_TOP_1, 6, 8);
	tex_atlas_reg(atlas, TEXAT_BED_TOP_2, 7, 8);

	tex_atlas_reg(atlas, TEXAT_CAST_BLOCK_LAPIS, 0, 9);
	// tex_atlas_reg(atlas, "repeater_on", 3, 9);
	tex_atlas_reg(atlas, TEXAT_BED_BACK, 5, 9);
	tex_atlas_reg(atlas, TEXAT_BED_SIDE_1, 6, 9);
	tex_atlas_reg(atlas, TEXAT_BED_SIDE_2, 7, 9);
	tex_atlas_reg(atlas, TEXAT_BED_FRONT, 8, 9);

	tex_atlas_reg(atlas, TEXAT_ORE_LAPIS, 0, 10);
	tex_atlas_reg(atlas, TEXAT_RAIL_POWERED_OFF, 3, 10);
	/*tex_atlas_reg_col(atlas, "redstone_intersect", 4, 10, 252, 49, 0);
	tex_atlas_reg_col(atlas, "redstone_wire", 5, 10, 252, 49, 0);*/

	tex_atlas_reg(atlas, TEXAT_SANDSTONE_TOP, 0, 11);
	tex_atlas_reg(atlas, TEXAT_RAIL_POWERED_ON, 3, 11);

	tex_atlas_reg(atlas, TEXAT_SANDSTONE_SIDE, 0, 12);
	tex_atlas_reg(atlas, TEXAT_RAIL_DETECTOR, 3, 12);

	tex_atlas_reg(atlas, TEXAT_SANDSTONE_BOTTOM, 0, 13);

	tex_atlas_reg(atlas, TEXAT_WOOL_0, 0, 4);
	tex_atlas_reg(atlas, TEXAT_WOOL_1, 2, 13);
	tex_atlas_reg(atlas, TEXAT_WOOL_2, 2, 12);
	tex_atlas_reg(atlas, TEXAT_WOOL_3, 2, 11);
	tex_atlas_reg(atlas, TEXAT_WOOL_4, 2, 10);
	tex_atlas_reg(atlas, TEXAT_WOOL_5, 2, 9);
	tex_atlas_reg(atlas, TEXAT_WOOL_6, 2, 8);
	tex_atlas_reg(atlas, TEXAT_WOOL_7, 2, 7);
	tex_atlas_reg(atlas, TEXAT_WOOL_8, 1, 14);
	tex_atlas_reg(atlas, TEXAT_WOOL_9, 1, 13);
	tex_atlas_reg(atlas, TEXAT_WOOL_10, 1, 12);
	tex_atlas_reg(atlas, TEXAT_WOOL_11, 1, 11);
	tex_atlas_reg(atlas, TEXAT_WOOL_12, 1, 10);
	tex_atlas_reg(atlas, TEXAT_WOOL_13, 1, 9);
	tex_atlas_reg(atlas, TEXAT_WOOL_14, 1, 8);
	tex_atlas_reg(atlas, TEXAT_WOOL_15, 1, 7);

	tex_atlas_reg_col(atlas, TEXAT_GRASS_TOP, 0, 0, 110, 198, 63);
	tex_atlas_reg_grass(atlas, TEXAT_GRASS_SIDE, 6, 2, 110, 198, 63, 3, 0);
	tex_atlas_reg_col(atlas, TEXAT_TALLGRASS, 7, 2, 110, 198, 63);
	tex_atlas_reg_col(atlas, TEXAT_LEAVES_OAK, 4, 3, 75, 182, 15);
	tex_atlas_reg_col(atlas, TEXAT_FERN, 8, 3, 110, 198, 63);

	for(int k = 0; k < 10; k++)
		tex_atlas_reg(atlas, TEXAT_BREAK_0 + k, k, 15);

	memset(global_atlas, 0, sizeof(global_atlas));

	uint8_t* image = tex_read(filename, width, height);
	void* output
		= tex_atlas_compute(atlas, global_atlas, image, *width, *height);
	dict_atlas_src_clear(atlas);
	free(image);

	return output;
}
