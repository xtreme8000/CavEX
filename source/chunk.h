#ifndef CHUNK_H
#define CHUNK_H

#include <gccore.h>
#include <m-lib/m-i-list.h>
#include <stdbool.h>
#include <stdint.h>

#include "block/blocks.h"
#include "displaylist.h"
#include "intrusive_list.h"
#include "world.h"

#define CHUNK_SIZE_BITS 0xF
#define CHUNK_SIZE 16

#define W2C_COORD(x) ((x)&CHUNK_SIZE_BITS)

typedef uint32_t c_coord_t;

struct chunk {
	Mtx model_view;
	w_coord_t x, y, z;
	uint8_t* blocks;
	bool has_faces;
	struct displaylist mesh[13];
	bool has_displist[13];
	bool rebuild_displist;
	struct world* world;
	uint8_t reachable[6];
	struct chunk_step {
		bool visited;
		enum side from;
		uint8_t used_exit_sides;
		int steps;
	} tmp_data;
	ILIST_INTERFACE(ilist_chunks, struct chunk);
};

void chunk_init(struct chunk* c, struct world* world, w_coord_t x, w_coord_t y,
				w_coord_t z);
void chunk_destroy(struct chunk* c);
struct block_data chunk_get_block(struct chunk* c, c_coord_t x, c_coord_t y,
								  c_coord_t z);
void chunk_set_block(struct chunk* c, c_coord_t x, c_coord_t y, c_coord_t z,
					 struct block_data blk);
void chunk_check_built(struct chunk* c);
void chunk_render(struct chunk* c, bool pass, float x, float y, float z);
void chunk_pre_render(struct chunk* c, Mtx view);
void chunk_test(struct chunk* c);

#endif
