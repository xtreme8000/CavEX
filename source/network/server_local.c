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
#include <math.h>
#include <stdlib.h>

#include "../cglm/cglm.h"

#include "../item/inventory.h"
#include "client_interface.h"
#include "server_interface.h"
#include "server_local.h"

#define CHUNK_DIST2(x1, x2, z1, z2)                                            \
	(((x1) - (x2)) * ((x1) - (x2)) + ((z1) - (z2)) * ((z1) - (z2)))

static bool has_chunk(struct server_local* s, w_coord_t x, w_coord_t z) {
	assert(s);

	for(size_t k = 0; k < s->loaded_regions_length; k++) {
		bool chunk_exists;
		if(region_archive_contains(s->loaded_regions + k, x, z,
								   &chunk_exists)) {
			ilist_regions_unlink(s->loaded_regions + k);
			ilist_regions_push_back(s->loaded_regions_lru,
									s->loaded_regions + k);
			return chunk_exists;
		}
	}

	struct region_archive ra;
	if(!region_archive_create(&ra, "world", CHUNK_REGION_COORD(x),
							  CHUNK_REGION_COORD(z)))
		return false;

	struct region_archive* lru;
	if(ilist_regions_size(s->loaded_regions_lru) < MAX_REGIONS) {
		assert(s->loaded_regions_length < MAX_REGIONS);
		lru = s->loaded_regions + (s->loaded_regions_length++);
	} else {
		lru = ilist_regions_pop_front(s->loaded_regions_lru);
		region_archive_destroy(lru);
	}

	*lru = ra;
	ilist_regions_push_back(s->loaded_regions_lru, lru);

	return true;
}

static bool load_chunk(struct server_local* s, w_coord_t x, w_coord_t z,
					   uint8_t** ids, uint8_t** metadata, uint8_t** lighting) {
	assert(s && ids && metadata && lighting);

	ilist_regions_it_t it;
	ilist_regions_it(it, s->loaded_regions_lru);

	while(!ilist_regions_end_p(it)) {
		struct region_archive* ra = ilist_regions_ref(it);

		bool chunk_exists;
		if(region_archive_contains(ra, x, z, &chunk_exists))
			return chunk_exists ?
				region_archive_get_blocks(ra, x, z, ids, metadata, lighting) :
				false;

		ilist_regions_next(it);
	}

	return false;
}

static void server_local_process(struct server_rpc* call, void* user) {
	assert(call && user);

	struct server_local* s = user;

	switch(call->type) {
		case SRPC_PLAYER_POS:
			if(s->player.finished_loading) {
				s->player.x = call->payload.player_pos.x;
				s->player.y = call->payload.player_pos.y;
				s->player.z = call->payload.player_pos.z;
				s->player.has_pos = true;
			}
			break;
	}
}

static void server_local_update(struct server_local* s) {
	assert(s);

	svin_process_messages(server_local_process, s, false);

	if(!s->player.has_pos)
		return;

	s->last_chunk_load = time_get();

	w_coord_t px = WCOORD_CHUNK_OFFSET(floor(s->player.x));
	w_coord_t pz = WCOORD_CHUNK_OFFSET(floor(s->player.z));

	struct loaded_chunk* c_furthest = NULL;
	w_coord_t c_furthest_dist2 = 0;
	for(size_t k = 0; k < s->loaded_chunks_length; k++) {
		// can unload chunk?
		w_coord_t d
			= CHUNK_DIST2(px, s->loaded_chunks[k].x, pz, s->loaded_chunks[k].z);
		if((abs(px - s->loaded_chunks[k].x) > MAX_VIEW_DISTANCE
			|| abs(pz - s->loaded_chunks[k].z) > MAX_VIEW_DISTANCE)
		   && d > c_furthest_dist2) {
			c_furthest_dist2 = d;
			c_furthest = s->loaded_chunks + k;
		}
	}

	if(c_furthest) {
		// unload just one chunk
		clin_rpc_send(&(struct client_rpc) {
			.type = CRPC_UNLOAD_CHUNK,
			.payload.unload_chunk.x = c_furthest->x,
			.payload.unload_chunk.z = c_furthest->z,
		});
		*c_furthest = s->loaded_chunks[--s->loaded_chunks_length];
	}

	// iterate over all chunks that should be loaded
	bool c_nearest = false;
	w_coord_t c_nearest_x, c_nearest_z;
	w_coord_t c_nearest_dist2;
	if(s->loaded_chunks_length < MAX_CHUNKS) {
		for(w_coord_t z = pz - MAX_VIEW_DISTANCE; z <= pz + MAX_VIEW_DISTANCE;
			z++) {
			for(w_coord_t x = px - MAX_VIEW_DISTANCE;
				x <= px + MAX_VIEW_DISTANCE; x++) {
				bool loaded = false;
				for(size_t k = 0; k < s->loaded_chunks_length; k++) {
					if(x == s->loaded_chunks[k].x
					   && z == s->loaded_chunks[k].z) {
						loaded = true;
						break;
					}
				}

				w_coord_t d = CHUNK_DIST2(px, x, pz, z);
				if(!loaded && (d < c_nearest_dist2 || !c_nearest)
				   && has_chunk(s, x, z)) {
					c_nearest_dist2 = d;
					c_nearest_x = x;
					c_nearest_z = z;
					c_nearest = true;
				}
			}
		}
	}

	// load just one chunk
	uint8_t *ids, *metadata, *lighting;
	if(c_nearest && has_chunk(s, c_nearest_x, c_nearest_z)
	   && load_chunk(s, c_nearest_x, c_nearest_z, &ids, &metadata, &lighting)) {
		s->loaded_chunks[s->loaded_chunks_length++]
			= (struct loaded_chunk) {.x = c_nearest_x, .z = c_nearest_z};

		clin_rpc_send(&(struct client_rpc) {
			.type = CRPC_CHUNK,
			.payload.chunk.x = c_nearest_x * CHUNK_SIZE,
			.payload.chunk.y = 0,
			.payload.chunk.z = c_nearest_z * CHUNK_SIZE,
			.payload.chunk.sx = CHUNK_SIZE,
			.payload.chunk.sy = WORLD_HEIGHT,
			.payload.chunk.sz = CHUNK_SIZE,
			.payload.chunk.ids = ids,
			.payload.chunk.metadata = metadata,
			.payload.chunk.lighting = lighting,
		});
	} else if(!s->player.finished_loading) {
		struct client_rpc pos;
		pos.type = CRPC_PLAYER_POS;
		if(level_archive_read_player(&s->level, pos.payload.player_pos.position,
									 pos.payload.player_pos.rotation, NULL))
			clin_rpc_send(&pos);

		struct item_data inventory[INVENTORY_SIZE];
		if(level_archive_read_inventory(&s->level, inventory, INVENTORY_SIZE)) {
			for(size_t k = 0; k < INVENTORY_SIZE; k++) {
				if(inventory[k].id > 0) {
					clin_rpc_send(&(struct client_rpc) {
						.type = CRPC_INVENTORY_SLOT,
						.payload.inventory_slot.slot = k,
						.payload.inventory_slot.item = inventory[k],
					});
				}
			}
		}

		s->player.finished_loading = true;
	}
}

static void* server_local_thread(void* user) {
	while(1) {
		server_local_update(user);
		usleep(50 * 1000);
	}

	return NULL;
}

void server_local_create(struct server_local* s) {
	assert(s);
	s->loaded_chunks_length = 0;
	s->loaded_regions_length = 0;
	s->last_chunk_load = time_get();
	s->player.has_pos = false;
	s->player.finished_loading = false;

	level_archive_create(&s->level, "world/level.dat");

	vec3 pos;
	if(level_archive_read_player(&s->level, pos, NULL, NULL)) {
		s->player.x = pos[0];
		s->player.y = pos[1];
		s->player.z = pos[2];
		s->player.has_pos = true;
	}

	ilist_regions_init(s->loaded_regions_lru);

	lwp_t thread;
	LWP_CreateThread(&thread, server_local_thread, s, NULL, 0, 8);
}
