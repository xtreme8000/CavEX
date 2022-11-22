#ifndef SERVER_LOCAL
#define SERVER_LOCAL

#include <stdbool.h>
#include <stddef.h>

#include "../world.h"
#include "server_interface.h"

#define MAX_VIEW_DISTANCE 5 // in chunks
#define MAX_CHUNKS ((MAX_VIEW_DISTANCE * 2 + 2) * (MAX_VIEW_DISTANCE * 2 + 2))

struct server_local {
	struct {
		double x, y, z;
	} player;
	struct loaded_chunk {
		w_coord_t x, z; // not!!! stored in multiples of CHUNK_SIZE
		bool modified;
	} loaded_chunks[MAX_CHUNKS];
	size_t loaded_chunks_length;
	struct server_interface iface;
};

void server_local_create(struct server_local* s);
struct server_interface* server_local_interface(struct server_local* s);
void server_local_update(struct server_local* s, float dt);

#endif
