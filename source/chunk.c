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

#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#include "block/blocks.h"
#include "chunk.h"
#include "game/game_state.h"
#include "platform/gfx.h"
#include "stack.h"

#define CHUNK_INDEX(x, y, z) ((x) + ((z) + (y)*CHUNK_SIZE) * CHUNK_SIZE)
#define CHUNK_LIGHT_INDEX(x, y, z)                                             \
	((x) + ((z) + (y) * (CHUNK_SIZE + 2)) * (CHUNK_SIZE + 2))

void chunk_init(struct chunk* c, struct world* world, w_coord_t x, w_coord_t y,
				w_coord_t z) {
	assert(c && world);

	c->blocks = malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 5 / 2);
	assert(c->blocks);

	memset(c->blocks, BLOCK_AIR, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 5 / 2);

	c->x = x;
	c->y = y;
	c->z = z;

	for(int k = 0; k < 13; k++)
		c->has_displist[k] = false;
	c->rebuild_displist = false;
	c->world = world;
	c->reference_count = 0;

	ilist_chunks_init_field(c);
	ilist_chunks2_init_field(c);
}

static void chunk_destroy(struct chunk* c) {
	assert(c);

	free(c->blocks);

	for(int k = 0; k < 13; k++) {
		if(c->has_displist[k])
			displaylist_destroy(c->mesh + k);
	}

	free(c);
}

void chunk_ref(struct chunk* c) {
	assert(c);
	c->reference_count++;
}

void chunk_unref(struct chunk* c) {
	assert(c);
	c->reference_count--;

	if(!c->reference_count)
		chunk_destroy(c);
}

struct block_data chunk_get_block(struct chunk* c, c_coord_t x, c_coord_t y,
								  c_coord_t z) {
	assert(c && x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE);

	size_t idx = CHUNK_INDEX(x, y, z) / 2 * 5;
	size_t off = CHUNK_INDEX(x, y, z) % 2;

	/* storage layout:
		type 0
		type 1
		light 0
		light 1
		meta 1/0
	*/

	return (struct block_data) {
		.type = c->blocks[idx + off + 0],
		.metadata = (c->blocks[idx + 4] >> (off * 4)) & 0xF,
		.sky_light = c->blocks[idx + off + 2] & 0xF,
		.torch_light = c->blocks[idx + off + 2] >> 4,
	};
}

// for global world lookup
struct block_data chunk_lookup_block(struct chunk* c, w_coord_t x, w_coord_t y,
									 w_coord_t z) {
	assert(c);
	struct chunk* other = c;

	if(x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE
	   || z >= CHUNK_SIZE)
		other = world_find_chunk(c->world, c->x + x, c->y + y, c->z + z);

	return other ?
		chunk_get_block(other, W2C_COORD(x), W2C_COORD(y), W2C_COORD(z)) :
		(struct block_data) {
			.type = (y < WORLD_HEIGHT) ? 1 : 0,
			.metadata = 0,
			.sky_light = (y < WORLD_HEIGHT) ? 0 : 15,
			.torch_light = 0,
		};
}

static void chunk_trigger_neighbour_update(struct chunk* c, c_coord_t x,
										   c_coord_t y, c_coord_t z) {
	// TODO: diagonal chunks, just sharing edge or single point

	bool cond[6] = {
		x == 0, x == CHUNK_SIZE - 1, y == 0, y == CHUNK_SIZE - 1,
		z == 0, z == CHUNK_SIZE - 1,
	};

	int offset[6][3] = {
		{-1, 0, 0},			{CHUNK_SIZE, 0, 0}, {0, -1, 0},
		{0, CHUNK_SIZE, 0}, {0, 0, -1},			{0, 0, CHUNK_SIZE},
	};

	for(int k = 0; k < 6; k++) {
		if(cond[k]) {
			struct chunk* other
				= world_find_chunk(c->world, c->x + offset[k][0],
								   c->y + offset[k][1], c->z + offset[k][2]);
			if(other)
				other->rebuild_displist = true;
		}
	}
}

void chunk_set_light(struct chunk* c, c_coord_t x, c_coord_t y, c_coord_t z,
					 uint8_t light) {
	assert(c && x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE);

	size_t idx = CHUNK_INDEX(x, y, z) / 2 * 5;
	size_t off = CHUNK_INDEX(x, y, z) % 2;

	c->blocks[idx + off + 2] = light;
	c->rebuild_displist = true;

	chunk_trigger_neighbour_update(c, x, y, z);
}

void chunk_set_block(struct chunk* c, c_coord_t x, c_coord_t y, c_coord_t z,
					 struct block_data blk) {
	assert(c && x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE);

	size_t idx = CHUNK_INDEX(x, y, z) / 2 * 5;
	size_t off = CHUNK_INDEX(x, y, z) % 2;

	c->blocks[idx + off + 0] = blk.type;
	c->blocks[idx + off + 2] = (blk.torch_light << 4) | blk.sky_light;
	c->blocks[idx + 4] = (c->blocks[idx + 4] & ~(0x0F << (off * 4)))
		| (blk.metadata << (off * 4));
	c->rebuild_displist = true;

	chunk_trigger_neighbour_update(c, x, y, z);
}

bool chunk_check_built(struct chunk* c) {
	assert(c);

	if(c->rebuild_displist && chunk_mesher_send(c)) {
		c->rebuild_displist = false;
		return true;
	}

	return false;
}

void chunk_pre_render(struct chunk* c, mat4 view, bool has_fog) {
	assert(c && view);

	glm_translate_to(view, (vec3) {c->x, c->y, c->z}, c->model_view);
	c->has_fog = has_fog;
}

static void check_matrix_set(struct chunk* c, bool* needs_matrix) {
	assert(c && needs_matrix);

	if(*needs_matrix) {
		gfx_matrix_modelview(c->model_view);
		gfx_fog(c->has_fog);
		gfx_fog_pos(c->x - gstate.camera.x, c->z - gstate.camera.z,
					gstate.config.fog_distance);
		*needs_matrix = false;
	}
}

void chunk_render(struct chunk* c, bool pass, float x, float y, float z) {
	assert(c);

	bool needs_matrix = true;
	int offset = pass ? 6 : 0;

	if(y < c->y + CHUNK_SIZE && c->has_displist[SIDE_BOTTOM + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_BOTTOM + offset);
	}

	if(y > c->y && c->has_displist[SIDE_TOP + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_TOP + offset);
	}

	if(x < c->x + CHUNK_SIZE && c->has_displist[SIDE_LEFT + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_LEFT + offset);
	}

	if(x > c->x && c->has_displist[SIDE_RIGHT + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_RIGHT + offset);
	}

	if(z < c->z + CHUNK_SIZE && c->has_displist[SIDE_FRONT + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_FRONT + offset);
	}

	if(z > c->z && c->has_displist[SIDE_BACK + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_BACK + offset);
	}

	if(!pass && c->has_displist[12]) {
		check_matrix_set(c, &needs_matrix);
		gfx_culling(false);
		displaylist_render(c->mesh + 12);
		gfx_culling(true);
	}
}
