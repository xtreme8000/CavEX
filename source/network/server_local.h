#ifndef SERVER_LOCAL
#define SERVER_LOCAL

#include <stdbool.h>
#include <stddef.h>

#include "../world.h"
#include "region_archive.h"

#define MAX_REGIONS 4
#define MAX_VIEW_DISTANCE 5 // in chunks
#define MAX_CHUNKS ((MAX_VIEW_DISTANCE * 2 + 2) * (MAX_VIEW_DISTANCE * 2 + 2))

struct server_local {
	struct {
		double x, y, z;
		bool has_pos;
	} player;
	struct loaded_chunk {
		w_coord_t x, z; // not!!! stored in multiples of CHUNK_SIZE
		bool modified;
	} loaded_chunks[MAX_CHUNKS];
	size_t loaded_chunks_length;
	struct region_archive loaded_regions[MAX_REGIONS];
	ilist_regions_t loaded_regions_lru;
	size_t loaded_regions_length;
	ptime_t last_chunk_load;
};

void server_local_create(struct server_local* s);

#endif
