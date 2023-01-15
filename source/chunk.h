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

#ifndef CHUNK_H
#define CHUNK_H

#include <m-lib/m-i-list.h>
#include <stdbool.h>
#include <stdint.h>

#include "cglm/cglm.h"
#include <m-lib/m-i-list.h>

#include "block/blocks.h"
#include "chunk_mesher.h"
#include "platform/graphics/displaylist.h"
#include "world.h"

#define CHUNK_SIZE_BITS 0xF
#define CHUNK_SIZE 16

#define WCOORD_CHUNK_OFFSET(x)                                                 \
	((x) < 0 ? (((x) + 1) / CHUNK_SIZE - 1) : (x) / CHUNK_SIZE)
#define CHUNK_TO_ID(x, y, z)                                                   \
	((((int64_t)(z)&0x3FFFFFFF) << 34) | (((int64_t)(x)&0x3FFFFFFF) << 4)      \
	 | ((int64_t)(y)&0xF))
#define W2C_COORD(x) ((x)&CHUNK_SIZE_BITS)

typedef uint32_t c_coord_t;

struct chunk {
	mat4 model_view;
	w_coord_t x, y, z;
	uint8_t* blocks;
	struct displaylist mesh[13];
	bool has_displist[13];
	bool rebuild_displist;
	struct world* world;
	uint8_t reachable[6];
	size_t reference_count;
	bool has_fog;
	struct chunk_step {
		bool visited;
		enum side from;
		uint8_t used_exit_sides;
		int steps;
	} tmp_data;
	ILIST_INTERFACE(ilist_chunks, struct chunk);
	ILIST_INTERFACE(ilist_chunks2, struct chunk);
};

ILIST_DEF(ilist_chunks, struct chunk, M_POD_OPLIST)
ILIST_DEF(ilist_chunks2, struct chunk, M_POD_OPLIST)

void chunk_init(struct chunk* c, struct world* world, w_coord_t x, w_coord_t y,
				w_coord_t z);
void chunk_ref(struct chunk* c);
void chunk_unref(struct chunk* c);
struct block_data chunk_get_block(struct chunk* c, c_coord_t x, c_coord_t y,
								  c_coord_t z);
struct block_data chunk_lookup_block(struct chunk* c, w_coord_t x, w_coord_t y,
									 w_coord_t z);
void chunk_set_block(struct chunk* c, c_coord_t x, c_coord_t y, c_coord_t z,
					 struct block_data blk);
bool chunk_check_built(struct chunk* c);
void chunk_set_light(struct chunk* c, c_coord_t x, c_coord_t y, c_coord_t z,
					 uint8_t light);
void chunk_render(struct chunk* c, bool pass, float x, float y, float z);
void chunk_pre_render(struct chunk* c, mat4 view, bool has_fog);

#endif
