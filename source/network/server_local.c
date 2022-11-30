#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "client_interface.h"
#include "server_interface.h"
#include "server_local.h"

#define CHUNK_DIST2(x1, x2, z1, z2)                                            \
	(((x1) - (x2)) * ((x1) - (x2)) + ((z1) - (z2)) * ((z1) - (z2)))

static const char* chunk_files[] = {
	"x0 z-10.vc",	"x-1 z-2.vc",  "x-3 z-5.vc",  "x-5 z-8.vc",	 "x-8 z-11.vc",
	"x0 z-11.vc",	"x-1 z-3.vc",  "x-3 z-6.vc",  "x-5 z-9.vc",	 "x-8 z-1.vc",
	"x0 z-1.vc",	"x-1 z-4.vc",  "x-3 z-7.vc",  "x-6 z-10.vc", "x-8 z-2.vc",
	"x0 z-2.vc",	"x-1 z-5.vc",  "x-3 z-8.vc",  "x-6 z-11.vc", "x-8 z-3.vc",
	"x0 z-3.vc",	"x-1 z-6.vc",  "x-3 z-9.vc",  "x-6 z-1.vc",	 "x-8 z-4.vc",
	"x0 z-4.vc",	"x-1 z-7.vc",  "x-4 z-10.vc", "x-6 z-2.vc",	 "x-8 z-5.vc",
	"x0 z-5.vc",	"x-1 z-8.vc",  "x-4 z-11.vc", "x-6 z-3.vc",	 "x-8 z-6.vc",
	"x0 z-6.vc",	"x-1 z-9.vc",  "x-4 z-1.vc",  "x-6 z-4.vc",	 "x-8 z-7.vc",
	"x0 z-7.vc",	"x-2 z-10.vc", "x-4 z-2.vc",  "x-6 z-5.vc",	 "x-8 z-8.vc",
	"x0 z-8.vc",	"x-2 z-11.vc", "x-4 z-3.vc",  "x-6 z-6.vc",	 "x-8 z-9.vc",
	"x0 z-9.vc",	"x-2 z-1.vc",  "x-4 z-4.vc",  "x-6 z-7.vc",	 "x-9 z-10.vc",
	"x-10 z-10.vc", "x-2 z-2.vc",  "x-4 z-5.vc",  "x-6 z-8.vc",	 "x-9 z-11.vc",
	"x-10 z-11.vc", "x-2 z-3.vc",  "x-4 z-6.vc",  "x-6 z-9.vc",	 "x-9 z-1.vc",
	"x-10 z-1.vc",	"x-2 z-4.vc",  "x-4 z-7.vc",  "x-7 z-10.vc", "x-9 z-2.vc",
	"x-10 z-2.vc",	"x-2 z-5.vc",  "x-4 z-8.vc",  "x-7 z-11.vc", "x-9 z-3.vc",
	"x-10 z-3.vc",	"x-2 z-6.vc",  "x-4 z-9.vc",  "x-7 z-1.vc",	 "x-9 z-4.vc",
	"x-10 z-4.vc",	"x-2 z-7.vc",  "x-5 z-10.vc", "x-7 z-2.vc",	 "x-9 z-5.vc",
	"x-10 z-5.vc",	"x-2 z-8.vc",  "x-5 z-11.vc", "x-7 z-3.vc",	 "x-9 z-6.vc",
	"x-10 z-6.vc",	"x-2 z-9.vc",  "x-5 z-1.vc",  "x-7 z-4.vc",	 "x-9 z-7.vc",
	"x-10 z-7.vc",	"x-3 z-10.vc", "x-5 z-2.vc",  "x-7 z-5.vc",	 "x-9 z-8.vc",
	"x-10 z-8.vc",	"x-3 z-11.vc", "x-5 z-3.vc",  "x-7 z-6.vc",	 "x-9 z-9.vc",
	"x-10 z-9.vc",	"x-3 z-1.vc",  "x-5 z-4.vc",  "x-7 z-7.vc",	 "x-1 z-10.vc",
	"x-3 z-2.vc",	"x-5 z-5.vc",  "x-7 z-8.vc",  "x-1 z-11.vc", "x-3 z-3.vc",
	"x-5 z-6.vc",	"x-7 z-9.vc",  "x-1 z-1.vc",  "x-3 z-4.vc",	 "x-5 z-7.vc",
	"x-8 z-10.vc",
};

static w_coord_t chunk_files_lookup[sizeof(chunk_files) / sizeof(*chunk_files)]
								   [2];

static const char* find_chunk(int x, int z) {
	for(size_t k = 0; k < sizeof(chunk_files) / sizeof(*chunk_files); k++) {
		if(chunk_files_lookup[k][0] == x && chunk_files_lookup[k][1] == z)
			return chunk_files[k];
	}

	return NULL;
}

static void find_chunk_cache() {
	for(size_t k = 0; k < sizeof(chunk_files) / sizeof(*chunk_files); k++) {
		int chunk_x, chunk_z;
		FILE* f = fopen(chunk_files[k], "rb");
		assert(f);
		fread((uint8_t*)&chunk_x + 3, sizeof(uint8_t), 1, f);
		fread((uint8_t*)&chunk_x + 2, sizeof(uint8_t), 1, f);
		fread((uint8_t*)&chunk_x + 1, sizeof(uint8_t), 1, f);
		fread((uint8_t*)&chunk_x + 0, sizeof(uint8_t), 1, f);
		fread((uint8_t*)&chunk_z + 3, sizeof(uint8_t), 1, f);
		fread((uint8_t*)&chunk_z + 2, sizeof(uint8_t), 1, f);
		fread((uint8_t*)&chunk_z + 1, sizeof(uint8_t), 1, f);
		fread((uint8_t*)&chunk_z + 0, sizeof(uint8_t), 1, f);
		fclose(f);

		chunk_files_lookup[k][0] = chunk_x;
		chunk_files_lookup[k][1] = chunk_z;
	}
}

static void load_chunk(const char* file, int* chunk_x, int* chunk_z,
					   uint8_t** ids, uint8_t** metadata, uint8_t** lighting) {
	uint8_t* chunk_data = malloc(16 * 16 * 128 * 3);

	FILE* f = fopen(file, "rb");
	assert(f);
	fread((uint8_t*)chunk_x + 3, sizeof(uint8_t), 1, f);
	fread((uint8_t*)chunk_x + 2, sizeof(uint8_t), 1, f);
	fread((uint8_t*)chunk_x + 1, sizeof(uint8_t), 1, f);
	fread((uint8_t*)chunk_x + 0, sizeof(uint8_t), 1, f);
	fread((uint8_t*)chunk_z + 3, sizeof(uint8_t), 1, f);
	fread((uint8_t*)chunk_z + 2, sizeof(uint8_t), 1, f);
	fread((uint8_t*)chunk_z + 1, sizeof(uint8_t), 1, f);
	fread((uint8_t*)chunk_z + 0, sizeof(uint8_t), 1, f);
	fread(chunk_data, 1, 16 * 16 * 128 * 3, f);
	fclose(f);

	*ids = malloc(16 * 16 * 128);
	*metadata = malloc(16 * 16 * 128);
	*lighting = malloc(16 * 16 * 128);

#define CHUNK_INDEX(x, y, z) ((x) + ((z) + (y)*16) * 16)

	for(int y = 0; y < 128; y++) {
		for(int z = 0; z < 16; z++) {
			for(int x = 0; x < 16; x++) {
				(*ids)[CHUNK_INDEX(x, y, z)]
					= chunk_data[CHUNK_INDEX(x, y, z) * 3 + 0];
				(*metadata)[CHUNK_INDEX(x, y, z)]
					= chunk_data[CHUNK_INDEX(x, y, z) * 3 + 1];
				(*lighting)[CHUNK_INDEX(x, y, z)]
					= chunk_data[CHUNK_INDEX(x, y, z) * 3 + 2];
			}
		}
	}

	free(chunk_data);
}

static void server_local_process(struct server_rpc* call, void* user) {
	assert(call && user);

	struct server_local* s = user;

	switch(call->type) {
		case SRPC_PLAYER_POS:
			s->player.x = call->payload.player_pos.x;
			s->player.y = call->payload.player_pos.y;
			s->player.z = call->payload.player_pos.z;
			break;
	}
}

static void server_local_update(struct server_local* s) {
	assert(s);

	svin_process_messages(server_local_process, s, false);

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
				   && find_chunk(x, z)) {
					c_nearest_dist2 = d;
					c_nearest_x = x;
					c_nearest_z = z;
					c_nearest = true;
				}
			}
		}
	}

	// load just one chunk
	if(c_nearest) {
		const char* str = find_chunk(c_nearest_x, c_nearest_z);
		if(str) {
			s->loaded_chunks[s->loaded_chunks_length++]
				= (struct loaded_chunk) {.x = c_nearest_x, .z = c_nearest_z};

			int x2, z2;
			uint8_t *ids, *metadata, *lighting;
			load_chunk(str, &x2, &z2, &ids, &metadata, &lighting);
			assert(x2 == c_nearest_x && z2 == c_nearest_z);

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
		}
	}
}

static void* server_local_thread(void* user) {
	find_chunk_cache();

	while(1) {
		server_local_update(user);
		usleep(100 * 1000);
	}

	return NULL;
}

void server_local_create(struct server_local* s) {
	assert(s);
	s->loaded_chunks_length = 0;
	s->last_chunk_load = time_get();

	lwp_t thread;
	LWP_CreateThread(&thread, server_local_thread, s, NULL, 0, 8);
}
