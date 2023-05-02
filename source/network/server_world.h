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

#ifndef SERVER_WORLD_H
#define SERVER_WORLD_H

#include <m-lib/m-dict.h>
#include <stdbool.h>
#include <stdint.h>

#include "region_archive.h"

struct server_chunk {
	uint8_t* ids;
	uint8_t* metadata;
	uint8_t* lighting_sky;
	uint8_t* lighting_torch;
	uint8_t* heightmap;
	bool modified;
};

#define MAX_REGIONS 4
#define S_CHUNK_ID(x, z) (((int64_t)(z) << 32) | (((int64_t)(x)&0xFFFFFFFF)))
#define S_CHUNK_X(id) ((int32_t)((id)&0xFFFFFFFF))
#define S_CHUNK_Z(id) ((int32_t)((id) >> 32))

// key not!!! stored in multiples of CHUNK_SIZE
DICT_DEF2(dict_server_chunks, int64_t, M_BASIC_OPLIST, struct server_chunk,
		  M_POD_OPLIST)

struct server_world {
	dict_server_chunks_t chunks;
	enum world_dim dimension;
	string_t level_name;
	struct region_archive loaded_regions[MAX_REGIONS];
	ilist_regions_t loaded_regions_lru;
	size_t loaded_regions_length;
};

void server_world_create(struct server_world* w, string_t level_name,
						 enum world_dim dimension);
void server_world_destroy(struct server_world* w);

bool server_world_get_block(struct server_world* w, w_coord_t x, w_coord_t y,
							w_coord_t z, struct block_data* blk);
bool server_world_set_block(struct server_world* w, w_coord_t x, w_coord_t y,
							w_coord_t z, struct block_data blk);

bool server_world_furthest_chunk(struct server_world* w, w_coord_t dist,
								 w_coord_t px, w_coord_t pz, w_coord_t* x,
								 w_coord_t* z);

bool server_world_is_chunk_loaded(struct server_world* w, w_coord_t x,
								  w_coord_t z);
bool server_world_load_chunk(struct server_world* w, w_coord_t x, w_coord_t z,
							 struct server_chunk** sc);
void server_world_save_chunk(struct server_world* w, bool erase, w_coord_t x,
							 w_coord_t z);
void server_world_save_chunk_obj(struct server_world* w, bool erase,
								 w_coord_t x, w_coord_t z,
								 struct server_chunk* c);
struct region_archive* server_world_chunk_region(struct server_world* w,
												 w_coord_t x, w_coord_t z);
bool server_world_disk_has_chunk(struct server_world* w, w_coord_t x,
								 w_coord_t z);

#endif
