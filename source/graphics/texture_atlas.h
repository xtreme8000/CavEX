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

#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include <stdint.h>

#include <m-lib/m-array.h>

enum tex_atlas_entry {
	TEXAT_STONE,
	TEXAT_DIRT,
	TEXAT_PLANKS,
	TEXAT_SLAB_STONE_SIDE,
	TEXAT_SLAB_STONE_TOP,
	TEXAT_BRICKS,
	TEXAT_TNT_SIDE,
	TEXAT_TNT_TOP,
	TEXAT_TNT_BOTTOM,
	TEXAT_COBWEB,
	TEXAT_ROSE,
	TEXAT_DANDELION,
	TEXAT_SAPLING_OAK,
	TEXAT_COBBLESTONE,
	TEXAT_BEDROCK,
	TEXAT_SAND,
	TEXAT_GRAVEL,
	TEXAT_LOG_OAK_SIDE,
	TEXAT_LOG_OAK_TOP,
	TEXAT_CAST_BLOCK_IRON,
	TEXAT_CAST_BLOCK_GOLD,
	TEXAT_CAST_BLOCK_DIAMOND,
	TEXAT_CHEST_TOP,
	TEXAT_CHEST_SIDE,
	TEXAT_CHEST_FRONT_SINGLE,
	TEXAT_MUSHROOM_RED,
	TEXAT_MUSHROOM_BROWN,
	TEXAT_ORE_GOLD,
	TEXAT_ORE_IRON,
	TEXAT_ORE_COAL,
	TEXAT_BOOKSHELF,
	TEXAT_COBBLESTONE_MOSSY,
	TEXAT_OBSIDIAN,
	TEXAT_CHEST_FRONT_1,
	TEXAT_CHEST_FRONT_2,
	TEXAT_WORKBENCH_TOP,
	TEXAT_FURNACE_FRONT,
	TEXAT_FURNACE_SIDE,
	TEXAT_DISPENSER_FRONT,
	TEXAT_SPONGE,
	TEXAT_GLASS,
	TEXAT_ORE_DIAMOND,
	TEXAT_ORE_REDSTONE,
	TEXAT_LEAVES_BIRCH,
	TEXAT_DEADBUSH,
	TEXAT_CHEST_BACK_1,
	TEXAT_CHEST_BACK_2,
	TEXAT_WORKBENCH_SIDE_1,
	TEXAT_WORKBENCH_SIDE_2,
	TEXAT_FURNACE_FRONT_LIT,
	TEXAT_FURNACE_TOP,
	TEXAT_SAPLING_SPRUCE,
	TEXAT_SPAWNER,
	TEXAT_SNOW,
	TEXAT_GRASS_SIDE_SNOW,
	TEXAT_CACTUS_TOP,
	TEXAT_CACTUS_SIDE,
	TEXAT_CACTUS_BOTTOM,
	TEXAT_CLAY,
	TEXAT_REED,
	TEXAT_JUKBEBOX_SIDE,
	TEXAT_JUKBEBOX_TOP,
	TEXAT_SAPLING_BIRCH,
	TEXAT_TORCH,
	TEXAT_DOOR_WOOD_TOP,
	TEXAT_DOOR_IRON_TOP,
	TEXAT_LADDER,
	TEXAT_TRAPDOOR,
	TEXAT_FARMLAND_WET,
	TEXAT_FARMLAND_DRY,
	TEXAT_CROPS_0,
	TEXAT_CROPS_1,
	TEXAT_CROPS_2,
	TEXAT_CROPS_3,
	TEXAT_CROPS_4,
	TEXAT_CROPS_5,
	TEXAT_CROPS_6,
	TEXAT_CROPS_7,
	TEXAT_DOOR_WOOD_BOTTOM,
	TEXAT_DOOR_IRON_BOTTOM,
	TEXAT_REDSTONE_TORCH_LIT,
	TEXAT_PUMPKIN_TOP,
	TEXAT_NETHERRACK,
	TEXAT_SOULSAND,
	TEXAT_GLOWSTONE,
	TEXAT_RAIL_CURVED,
	TEXAT_REDSTONE_TORCH,
	TEXAT_LOG_SPRUCE_SIDE,
	TEXAT_LOG_BIRCH_SIDE,
	TEXAT_PUMPKIN_SIDE,
	TEXAT_PUMPKIN_FRONT,
	TEXAT_PUMPKIN_FRONT_LIT,
	TEXAT_CAKE_TOP,
	TEXAT_CAKE_SIDE,
	TEXAT_CAKE_SIDE_CUT,
	TEXAT_CAKE_BOTTOM,
	TEXAT_RAIL,
	TEXAT_LEAVES_SPRUCE,
	TEXAT_BED_TOP_1,
	TEXAT_BED_TOP_2,
	TEXAT_CAST_BLOCK_LAPIS,
	TEXAT_BED_BACK,
	TEXAT_BED_SIDE_1,
	TEXAT_BED_SIDE_2,
	TEXAT_BED_FRONT,
	TEXAT_ORE_LAPIS,
	TEXAT_RAIL_POWERED_OFF,
	TEXAT_SANDSTONE_TOP,
	TEXAT_RAIL_POWERED_ON,
	TEXAT_SANDSTONE_SIDE,
	TEXAT_RAIL_DETECTOR,
	TEXAT_SANDSTONE_BOTTOM,
	TEXAT_WOOL_0,
	TEXAT_WOOL_1,
	TEXAT_WOOL_2,
	TEXAT_WOOL_3,
	TEXAT_WOOL_4,
	TEXAT_WOOL_5,
	TEXAT_WOOL_6,
	TEXAT_WOOL_7,
	TEXAT_WOOL_8,
	TEXAT_WOOL_9,
	TEXAT_WOOL_10,
	TEXAT_WOOL_11,
	TEXAT_WOOL_12,
	TEXAT_WOOL_13,
	TEXAT_WOOL_14,
	TEXAT_WOOL_15,
	TEXAT_GRASS_TOP,
	TEXAT_GRASS_SIDE,
	TEXAT_TALLGRASS,
	TEXAT_LEAVES_OAK,
	TEXAT_FERN,

	TEXAT_BREAK_0,
	TEXAT_BREAK_1,
	TEXAT_BREAK_2,
	TEXAT_BREAK_3,
	TEXAT_BREAK_4,
	TEXAT_BREAK_5,
	TEXAT_BREAK_6,
	TEXAT_BREAK_7,
	TEXAT_BREAK_8,
	TEXAT_BREAK_9,

	TEXAT_MAX,
};

struct texture_entry {
	enum tex_atlas_entry name;
	uint8_t x : 4, y : 4;
	struct {
		bool enable;
		uint8_t r, g, b;
	} colorize;
	struct {
		bool enable;
		uint8_t x : 4, y : 4;
	} bg;
};

ARRAY_DEF(dict_atlas_src, struct texture_entry, M_POD_OPLIST)

#define TEXTURE_INDEX(x, y) (((y)*14) + (x))
#define TEXTURE_X(idx) ((idx) % 14)
#define TEXTURE_Y(idx) ((idx) / 14)

void tex_atlas_reg(dict_atlas_src_t atlas, enum tex_atlas_entry name, uint8_t x,
				   uint8_t y);
void tex_atlas_reg_col(dict_atlas_src_t atlas, enum tex_atlas_entry name,
					   uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b);
void tex_atlas_reg_grass(dict_atlas_src_t atlas, enum tex_atlas_entry name,
						 uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b,
						 uint8_t bg_x, uint8_t bg_y);
void* tex_atlas_compute(dict_atlas_src_t atlas, uint8_t* atlas_dst,
						uint8_t* image, size_t width, size_t height);

uint8_t tex_atlas_lookup(enum tex_atlas_entry name);
void* tex_atlas_block(const char* filename, size_t* width, size_t* height);

#endif
