#ifndef WORLD_H
#define WORLD_H

#include <stddef.h>
#include <stdint.h>

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
	struct world* world;
	w_coord_t x, y, z;
};

struct world {
	int test;
};

void world_init(struct world* w, size_t chunks);
void world_destroy(struct world* w);
struct chunk* world_find_chunk(struct world* w, w_coord_t x, w_coord_t y,
							   w_coord_t z);
struct block_data world_get_block(struct world* w, w_coord_t x, w_coord_t y,
								  w_coord_t z);
void world_set_block(struct world* w, w_coord_t x, w_coord_t y, w_coord_t z,
					 struct block_data blk);

#endif
