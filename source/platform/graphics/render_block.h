#ifndef RENDER_BLOCK_H
#define RENDER_BLOCK_H

#include <gccore.h>
#include <stdbool.h>
#include <stddef.h>

#include "../../cglm/cglm.h"

#include "../../block/blocks.h"
#include "../../world.h"
#include "displaylist.h"

void render_block_selection(mat4 view_matrix, struct block_info* this);

size_t render_block_pressure_plate(struct displaylist* d,
								   struct block_info* this, enum side side,
								   struct block_info* it, uint8_t* vertex_light,
								   bool count_only);

size_t render_block_stairs(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only);

size_t render_block_stairs_always(struct displaylist* d,
								  struct block_info* this, enum side side,
								  struct block_info* it, uint8_t* vertex_light,
								  bool count_only);

size_t render_block_layer(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_cross(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_fluid(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_full(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

size_t render_block_slab(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

size_t render_block_bed(struct displaylist* d, struct block_info* this,
						enum side side, struct block_info* it,
						uint8_t* vertex_light, bool count_only);

size_t render_block_fire(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

size_t render_block_farmland(struct displaylist* d, struct block_info* this,
							 enum side side, struct block_info* it,
							 uint8_t* vertex_light, bool count_only);

size_t render_block_crops(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_torch(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_cactus(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only);

size_t render_block_portal(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only);

size_t render_block_rail(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

size_t render_block_ladder(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only);

size_t render_block_cake(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

#endif
