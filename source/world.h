#ifndef WORLD_H
#define WORLD_H

#include <m-lib/m-dict.h>
#include <stddef.h>
#include <stdint.h>

#include "cglm/cglm.h"

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
	struct block_data* neighbours;
	w_coord_t x, y, z;
};

#include "block/aabb.h"
#include "chunk.h"
#include "game/camera.h"
#include "util.h"

DICT_DEF2(dict_chunks, uint64_t, M_BASIC_OPLIST, struct chunk*, M_PTR_OPLIST)

struct world {
	dict_chunks_t chunks;
	struct chunk* world_chunk_cache;
	ilist_chunks_t render;
	ilist_chunks2_t gpu_busy_chunks;
	ptime_t anim_timer;
};

void world_create(struct world* w);
void world_destroy(struct world* w);
void world_load_chunk(struct world* w, struct chunk* c);
void world_unload_chunk(struct world* w, struct chunk* c);
void world_build_chunks(struct world* w, size_t tokens);
void world_render_completed(struct world* w, bool new_render);
struct chunk* world_find_chunk_neighbour(struct world* w, struct chunk* c,
										 enum side s);
struct chunk* world_find_chunk(struct world* w, w_coord_t x, w_coord_t y,
							   w_coord_t z);
struct block_data world_get_block(struct world* w, w_coord_t x, w_coord_t y,
								  w_coord_t z);
void world_set_block(struct world* w, w_coord_t x, w_coord_t y, w_coord_t z,
					 struct block_data blk);
void world_preload(struct world* w,
				   void (*progress)(struct world* w, float percent));
bool world_block_intersection(struct world* w, struct ray* r, w_coord_t x,
							  w_coord_t y, w_coord_t z, enum side* s);
void world_pre_render(struct world* w, struct camera* c, mat4 view);
size_t world_render(struct world* w, struct camera* c, bool pass);

#endif
