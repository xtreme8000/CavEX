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

#ifndef WORLD_H
#define WORLD_H

#include <m-lib/m-dict.h>
#include <stddef.h>
#include <stdint.h>

#include "cglm/cglm.h"

#include "stack.h"

#define WORLD_HEIGHT 128

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

#include "block/aabb.h"
#include "chunk.h"
#include "game/camera.h"
#include "util.h"

#define COLUMN_HEIGHT ((WORLD_HEIGHT + CHUNK_SIZE - 1) / CHUNK_SIZE)

struct world_section {
	int8_t heightmap[CHUNK_SIZE * CHUNK_SIZE];
	struct chunk* column[COLUMN_HEIGHT];
};

struct world_modification_entry {
	w_coord_t x, y, z;
	struct block_data blk;
};

#define SECTION_TO_ID(x, z) (((int64_t)(z) << 32) | (((int64_t)(x)&0xFFFFFFFF)))

DICT_DEF2(dict_wsection, int64_t, M_BASIC_OPLIST, struct world_section,
		  M_POD_OPLIST)

struct world {
	dict_wsection_t sections;
	struct chunk* world_chunk_cache;
	ilist_chunks_t render;
	ilist_chunks2_t gpu_busy_chunks;
	ptime_t anim_timer;
	struct stack lighting_updates;
};

void world_create(struct world* w);
void world_destroy(struct world* w);
void world_unload_section(struct world* w, w_coord_t x, w_coord_t z);
w_coord_t world_get_height(struct world* w, w_coord_t x, w_coord_t z);
void world_copy_heightmap(struct world* w, struct chunk* c, int8_t* heightmap);
size_t world_build_chunks(struct world* w, size_t tokens);
void world_render_completed(struct world* w, bool new_render);
struct chunk* world_find_chunk_neighbour(struct world* w, struct chunk* c,
										 enum side s);
struct chunk* world_chunk_from_section(struct world* w, struct world_section* s,
									   w_coord_t y);
struct chunk* world_find_chunk(struct world* w, w_coord_t x, w_coord_t y,
							   w_coord_t z);
struct block_data world_get_block(struct world* w, w_coord_t x, w_coord_t y,
								  w_coord_t z);
void world_set_block(struct world* w, w_coord_t x, w_coord_t y, w_coord_t z,
					 struct block_data blk, bool light_update);
void world_update_lighting(struct world* w);
void world_preload(struct world* w,
				   void (*progress)(struct world* w, float percent));
bool world_block_intersection(struct world* w, struct ray* r, w_coord_t x,
							  w_coord_t y, w_coord_t z, enum side* s);
void world_pre_render(struct world* w, struct camera* c, mat4 view);
size_t world_render(struct world* w, struct camera* c, bool pass);
bool world_aabb_intersection(struct world* w, struct AABB* a);

#endif
