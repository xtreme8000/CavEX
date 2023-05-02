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

#ifndef BLOCKS_DATA_H
#define BLOCKS_DATA_H

#include <stdint.h>

enum side {
	SIDE_TOP = 0,
	SIDE_BOTTOM = 1,
	SIDE_LEFT = 2,
	SIDE_RIGHT = 3,
	SIDE_FRONT = 4,
	SIDE_BACK = 5,
	SIDE_MAX
};

enum block_material {
	MATERIAL_WOOD,
	MATERIAL_STONE,
	MATERIAL_WOOL,
	MATERIAL_ORGANIC,
	MATERIAL_SAND,
	MATERIAL_GLASS,
};

enum block_type {
	BLOCK_AIR = 0,
	BLOCK_STONE = 1,
	BLOCK_GRASS = 2,
	BLOCK_DIRT = 3,
	BLOCK_COBBLESTONE = 4,
	BLOCK_WATER_FLOW = 8,
	BLOCK_WATER_STILL = 9,
	BLOCK_LAVA_FLOW = 10,
	BLOCK_LAVA_STILL = 11,
	BLOCK_SAND = 12,
	BLOCK_GRAVEL = 13,
	BLOCK_LOG = 17,
	BLOCK_LEAVES = 18,
	BLOCK_TALL_GRASS = 31,
	BLOCK_DOUBLE_SLAB = 43,
	BLOCK_MOSSY_COBBLE = 48,
	BLOCK_OBSIDIAN = 49,
	BLOCK_SNOW = 78,
	BLOCK_ICE = 79,
	BLOCK_CACTUS = 81,
	BLOCK_PORTAL = 90,
};

typedef int32_t w_coord_t;

struct block_data {
	uint8_t type;
	uint8_t metadata : 4;
	uint8_t sky_light : 4;
	uint8_t torch_light : 4;
};

struct block_info {
	struct block_data* block;
	struct block_data* neighbours;
	w_coord_t x, y, z;
};

#endif
