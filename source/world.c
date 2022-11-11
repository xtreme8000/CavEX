#include <assert.h>

#include "chunk.h"
#include "world.h"

struct block_data world_get_block(struct world* w, w_coord_t x, w_coord_t y,
								  w_coord_t z) {
	assert(w);
	struct chunk* c = world_find_chunk(w, x, y, z);

	return c ? chunk_get_block(c, x & CHUNK_SIZE_BITS, y & CHUNK_SIZE_BITS,
							   z & CHUNK_SIZE_BITS) :
			   (struct block_data) {.type = (y < WORLD_HEIGHT) ? 1 : 0};
}

void world_set_block(struct world* w, w_coord_t x, w_coord_t y, w_coord_t z,
					 struct block_data blk) {
	assert(w);
	struct chunk* c = world_find_chunk(w, x, y, z);

	if(c)
		chunk_set_block(c, x & CHUNK_SIZE_BITS, y & CHUNK_SIZE_BITS,
						z & CHUNK_SIZE_BITS, blk);
}
