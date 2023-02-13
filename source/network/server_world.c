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

#include "../lighting.h"
#include "../util.h"
#include "server_world.h"

#define CHUNK_DIST2(x1, x2, z1, z2)                                            \
	(((x1) - (x2)) * ((x1) - (x2)) + ((z1) - (z2)) * ((z1) - (z2)))

#define S_CHUNK_IDX(x, y, z)                                                   \
	((y) + (W2C_COORD(z) + W2C_COORD(x) * CHUNK_SIZE) * WORLD_HEIGHT)

void server_world_chunk_destroy(struct server_chunk* sc) {
	assert(sc);

	free(sc->ids);
	free(sc->metadata);
	free(sc->lighting_sky);
	free(sc->lighting_torch);
	free(sc->heightmap);
}

void server_world_create(struct server_world* w, string_t level_name,
						 enum world_dim dimension) {
	assert(w && dimension >= -1 && dimension <= 0);

	dict_server_chunks_init(w->chunks);
	ilist_regions_init(w->loaded_regions_lru);
	string_init_set(w->level_name, level_name);
	w->dimension = dimension;
	w->loaded_regions_length = 0;
}

void server_world_destroy(struct server_world* w) {
	assert(w);

	dict_server_chunks_it_t it;
	dict_server_chunks_it(it, w->chunks);

	while(!dict_server_chunks_end_p(it)) {
		struct server_chunk* sc = &dict_server_chunks_ref(it)->value;
		int64_t id = dict_server_chunks_ref(it)->key;
		server_world_save_chunk_obj(w, false, S_CHUNK_X(id), S_CHUNK_Z(id), sc);
		server_world_chunk_destroy(sc);

		dict_server_chunks_next(it);
	}

	dict_server_chunks_clear(w->chunks);
	string_clear(w->level_name);
}

static bool server_chunk_get_block(void* user, c_coord_t x, w_coord_t y,
								   c_coord_t z, struct block_data* blk) {
	assert(user && blk);
	struct server_chunk* sc = user;

	if(y < 0 || y >= WORLD_HEIGHT)
		return false;

	size_t idx = S_CHUNK_IDX(x, y, z);

	*blk = (struct block_data) {
		.type = sc->ids[idx],
		.metadata = nibble_read(sc->metadata, idx),
		.sky_light = nibble_read(sc->lighting_sky, idx),
		.torch_light = nibble_read(sc->lighting_torch, idx),
	};

	return true;
}

static bool server_world_light_get_block(void* user, w_coord_t x, w_coord_t y,
										 w_coord_t z, struct block_data* blk,
										 uint8_t* height) {
	assert(user);
	struct server_world* w = user;

	if(y < 0 || y >= WORLD_HEIGHT)
		return false;

	struct server_chunk* sc = dict_server_chunks_get(
		w->chunks, S_CHUNK_ID(WCOORD_CHUNK_OFFSET(x), WCOORD_CHUNK_OFFSET(z)));

	if(!sc)
		return false;

	if(blk)
		server_chunk_get_block(sc, W2C_COORD(x), y, W2C_COORD(z), blk);

	if(height)
		*height = sc->heightmap[W2C_COORD(x) + W2C_COORD(z) * CHUNK_SIZE];

	return true;
}

static void server_world_light_set_light(void* user, w_coord_t x, w_coord_t y,
										 w_coord_t z, uint8_t light) {
	assert(user);
	struct server_world* w = user;
	struct server_chunk* sc = dict_server_chunks_get(
		w->chunks, S_CHUNK_ID(WCOORD_CHUNK_OFFSET(x), WCOORD_CHUNK_OFFSET(z)));
	assert(sc);

	size_t idx = S_CHUNK_IDX(x, y, z);
	nibble_write(sc->lighting_sky, idx, light & 0xF);
	nibble_write(sc->lighting_torch, idx, light >> 4);
	sc->modified = true;
}

bool server_world_set_block(struct server_world* w, w_coord_t x, w_coord_t y,
							w_coord_t z, struct block_data blk) {
	assert(w && y >= 0 && y < WORLD_HEIGHT);

	struct server_chunk* sc = dict_server_chunks_get(
		w->chunks, S_CHUNK_ID(WCOORD_CHUNK_OFFSET(x), WCOORD_CHUNK_OFFSET(z)));

	if(sc) {
		size_t idx = S_CHUNK_IDX(x, y, z);
		sc->modified = true;
		sc->ids[idx] = blk.type;
		nibble_write(sc->metadata, idx, blk.metadata);

		if(w->dimension != WORLD_DIM_NETHER)
			lighting_heightmap_update(sc->heightmap, W2C_COORD(x), y,
									  W2C_COORD(z), blk.type,
									  server_chunk_get_block, sc);

		lighting_update_at_block(
			(struct world_modification_entry) {
				.x = x,
				.y = y,
				.z = z,
				.blk = blk,
			},
			w->dimension == WORLD_DIM_NETHER, server_world_light_get_block,
			server_world_light_set_light, w);
	}

	return sc;
}

bool server_world_furthest_chunk(struct server_world* w, w_coord_t dist,
								 w_coord_t px, w_coord_t pz, w_coord_t* x,
								 w_coord_t* z) {
	assert(w && x && z);

	dict_server_chunks_it_t it;
	dict_server_chunks_it(it, w->chunks);

	w_coord_t furthest_dist2 = -1;
	bool found = false;
	while(!dict_server_chunks_end_p(it)) {
		int64_t id = dict_server_chunks_ref(it)->key;
		w_coord_t d = CHUNK_DIST2(px, S_CHUNK_X(id), pz, S_CHUNK_Z(id));
		dict_server_chunks_next(it);

		if((abs(px - S_CHUNK_X(id)) > dist || abs(pz - S_CHUNK_Z(id)) > dist)
		   && d > furthest_dist2) {
			*x = S_CHUNK_X(id);
			*z = S_CHUNK_Z(id);
			furthest_dist2 = d;
			found = true;
		}
	}

	return found;
}

bool server_world_is_chunk_loaded(struct server_world* w, w_coord_t x,
								  w_coord_t z) {
	assert(w);
	return dict_server_chunks_get(w->chunks, S_CHUNK_ID(x, z)) != NULL;
}

bool server_world_load_chunk(struct server_world* w, w_coord_t x, w_coord_t z,
							 struct server_chunk** sc) {
	assert(w && sc);

	if(server_world_is_chunk_loaded(w, x, z))
		return false;

	ilist_regions_it_t it;
	ilist_regions_it(it, w->loaded_regions_lru);

	while(!ilist_regions_end_p(it)) {
		struct region_archive* ra = ilist_regions_ref(it);

		struct server_chunk tmp = (struct server_chunk) {.modified = false};

		bool chunk_exists;
		if(region_archive_contains(ra, x, z, &chunk_exists)) {
			if(chunk_exists && region_archive_get_blocks(ra, x, z, &tmp)) {
				dict_server_chunks_set_at(w->chunks, S_CHUNK_ID(x, z), tmp);
				*sc = dict_server_chunks_get(w->chunks, S_CHUNK_ID(x, z));
				return true;
			} else {
				return false;
			}
		}

		ilist_regions_next(it);
	}

	return false;
}

void server_world_save_chunk(struct server_world* w, bool erase, w_coord_t x,
							 w_coord_t z) {
	assert(w);
	struct server_chunk* c
		= dict_server_chunks_get(w->chunks, S_CHUNK_ID(x, z));
	if(c)
		server_world_save_chunk_obj(w, erase, x, z, c);
}

void server_world_save_chunk_obj(struct server_world* w, bool erase,
								 w_coord_t x, w_coord_t z,
								 struct server_chunk* c) {
	assert(w && c);

	if(c->modified) {
		//  load region archive into cache
		struct region_archive tmp;
		struct region_archive* ra = server_world_chunk_region(w, x, z);

		if(!ra) {
			if(!region_archive_create_new(&tmp, w->level_name,
										  CHUNK_REGION_COORD(x),
										  CHUNK_REGION_COORD(z), w->dimension))
				return;
			ra = &tmp;
		}

		region_archive_set_blocks(ra, x, z, c);
		c->modified = false;
	}

	if(erase) {
		server_world_chunk_destroy(c);
		dict_server_chunks_erase(w->chunks, S_CHUNK_ID(x, z));
	}
}

bool server_world_disk_has_chunk(struct server_world* w, w_coord_t x,
								 w_coord_t z) {
	struct region_archive* ra = server_world_chunk_region(w, x, z);
	bool chunk_exists;
	return ra ?
		(region_archive_contains(ra, x, z, &chunk_exists) && chunk_exists) :
		false;
}

struct region_archive* server_world_chunk_region(struct server_world* w,
												 w_coord_t x, w_coord_t z) {
	assert(w);

	for(size_t k = 0; k < w->loaded_regions_length; k++) {
		bool chunk_exists;
		if(region_archive_contains(w->loaded_regions + k, x, z,
								   &chunk_exists)) {
			ilist_regions_unlink(w->loaded_regions + k);
			ilist_regions_push_back(w->loaded_regions_lru,
									w->loaded_regions + k);
			return w->loaded_regions + k;
		}
	}

	struct region_archive ra;
	if(!region_archive_create(&ra, w->level_name, CHUNK_REGION_COORD(x),
							  CHUNK_REGION_COORD(z), w->dimension))
		return NULL;

	struct region_archive* lru;
	if(ilist_regions_size(w->loaded_regions_lru) < MAX_REGIONS) {
		assert(w->loaded_regions_length < MAX_REGIONS);
		lru = w->loaded_regions + (w->loaded_regions_length++);
	} else {
		lru = ilist_regions_pop_front(w->loaded_regions_lru);
		region_archive_destroy(lru);
	}

	*lru = ra;
	ilist_regions_push_back(w->loaded_regions_lru, lru);

	return lru;
}
