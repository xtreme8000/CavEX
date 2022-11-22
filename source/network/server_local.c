#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "client_interface.h"
#include "server_local.h"

static void server_local_player_pos(void* user, double x, double y, double z) {
	assert(user);
	struct server_local* local = user;
	local->player.x = x;
	local->player.y = y;
	local->player.z = z;
}

void server_local_create(struct server_local* s) {
	assert(s);
	s->loaded_chunks_length = 0;
	s->iface.user = s;
	s->iface.player_pos = server_local_player_pos;
}

struct server_interface* server_local_interface(struct server_local* s) {
	assert(s);
	return &s->iface;
}

void server_local_update(struct server_local* s, float dt) {
	assert(s && dt > 0);

	w_coord_t px = WCOORD_CHUNK_OFFSET(floor(s->player.x));
	w_coord_t pz = WCOORD_CHUNK_OFFSET(floor(s->player.z));

	for(size_t k = 0; k < s->loaded_chunks_length; k++) {
		// can unload chunk?
		if(abs(px - s->loaded_chunks[k].x) > MAX_VIEW_DISTANCE + 1
		   || abs(pz - s->loaded_chunks[k].z) > MAX_VIEW_DISTANCE + 1) {
			// unload just one chunk
			clin_unload_chunk(s->loaded_chunks[k].x, s->loaded_chunks[k].z);
			s->loaded_chunks[k]
				= s->loaded_chunks[(s->loaded_chunks_length--) - 1];
			break;
		}
	}

	// iterate over all chunks that should be loaded
	for(w_coord_t z = pz - MAX_VIEW_DISTANCE; z < pz + MAX_VIEW_DISTANCE; z++) {
		for(w_coord_t x = px - MAX_VIEW_DISTANCE; x < px + MAX_VIEW_DISTANCE;
			x++) {
			bool loaded = false;
			for(size_t k = 0; k < s->loaded_chunks_length; k++) {
				if(x == s->loaded_chunks[k].x && z == s->loaded_chunks[k].z) {
					loaded = true;
					break;
				}
			}

			if(!loaded && s->loaded_chunks_length < MAX_CHUNKS) {
				// load just one chunk
				s->loaded_chunks[s->loaded_chunks_length++]
					= (struct loaded_chunk) {.x = x, .z = z};
				clin_chunk(x, 0, z, CHUNK_SIZE, WORLD_HEIGHT, CHUNK_SIZE, NULL);
				break;
			}
		}
	}
}
