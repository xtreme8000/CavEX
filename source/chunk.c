#include <assert.h>
#include <gccore.h>
#include <malloc.h>
#include <math.h>
#include <stddef.h>
#include <string.h>

#include "block/blocks.h"
#include "chunk.h"
#include "stack.h"

#define CHUNK_INDEX(x, y, z) ((x) + ((z) + (y)*CHUNK_SIZE) * CHUNK_SIZE)
#define CHUNK_LIGHT_INDEX(x, y, z)                                             \
	((x) + ((z) + (y) * (CHUNK_SIZE + 2)) * (CHUNK_SIZE + 2))

void chunk_init(struct chunk* c, struct world* world, w_coord_t x, w_coord_t y,
				w_coord_t z) {
	assert(c && world);

	c->blocks = malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 3);
	assert(c->blocks);

	memset(c->blocks, BLOCK_AIR, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 3);

	c->x = x;
	c->y = y;
	c->z = z;

	for(int k = 0; k < 13; k++)
		c->has_displist[k] = false;
	c->rebuild_displist = false;
	c->world = world;
}

void chunk_destroy(struct chunk* c) {
	assert(c);

	free(c->blocks);

	for(int k = 0; k < 13; k++) {
		if(c->has_displist[k])
			displaylist_destroy(c->mesh + k);
	}
}

struct block_data chunk_get_block(struct chunk* c, c_coord_t x, c_coord_t y,
								  c_coord_t z) {
	assert(c && x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE);

	return (struct block_data) {
		.type = c->blocks[CHUNK_INDEX(x, y, z) * 3 + 0],
		.metadata = c->blocks[CHUNK_INDEX(x, y, z) * 3 + 1],
		.sky_light = c->blocks[CHUNK_INDEX(x, y, z) * 3 + 2] & 0xF,
		.torch_light = c->blocks[CHUNK_INDEX(x, y, z) * 3 + 2] >> 4,
	};
}

static struct block_data chunk_lookup_block(struct chunk* c, w_coord_t x,
											w_coord_t y, w_coord_t z) {
	assert(c);
	struct chunk* other = c;

	if(x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE
	   || z >= CHUNK_SIZE)
		other = world_find_chunk(c->world, c->x + x, c->y + y, c->z + z);

	return other ? chunk_get_block(other, x & CHUNK_SIZE_BITS,
								   y & CHUNK_SIZE_BITS, z & CHUNK_SIZE_BITS) :
				   (struct block_data) {
					   .type = (y < WORLD_HEIGHT) ? 1 : 0,
					   .metadata = 0,
					   .sky_light = 0,
					   .torch_light = 0,
				   };
}

void chunk_set_block(struct chunk* c, c_coord_t x, c_coord_t y, c_coord_t z,
					 struct block_data blk) {
	assert(c && x < CHUNK_SIZE && y < CHUNK_SIZE && z < CHUNK_SIZE);

	c->blocks[CHUNK_INDEX(x, y, z) * 3 + 0] = blk.type;
	c->blocks[CHUNK_INDEX(x, y, z) * 3 + 1] = blk.metadata;
	c->blocks[CHUNK_INDEX(x, y, z) * 3 + 2]
		= (blk.torch_light << 4) | blk.sky_light;
	c->rebuild_displist = true;

	// TODO: diagonal chunks, just sharing edge or single point

	if(x == 0) {
		struct chunk* other = world_find_chunk(c->world, c->x - 1, c->y, c->z);
		if(other)
			other->rebuild_displist = true;
	}

	if(x == CHUNK_SIZE - 1) {
		struct chunk* other
			= world_find_chunk(c->world, c->x + CHUNK_SIZE, c->y, c->z);
		if(other)
			other->rebuild_displist = true;
	}

	if(y == 0) {
		struct chunk* other = world_find_chunk(c->world, c->x, c->y - 1, c->z);
		if(other)
			other->rebuild_displist = true;
	}

	if(y == CHUNK_SIZE - 1) {
		struct chunk* other
			= world_find_chunk(c->world, c->x, c->y + CHUNK_SIZE, c->z);
		if(other)
			other->rebuild_displist = true;
	}

	if(z == 0) {
		struct chunk* other = world_find_chunk(c->world, c->x, c->y, c->z - 1);
		if(other)
			other->rebuild_displist = true;
	}

	if(z == CHUNK_SIZE - 1) {
		struct chunk* other
			= world_find_chunk(c->world, c->x, c->y, c->z + CHUNK_SIZE);
		if(other)
			other->rebuild_displist = true;
	}
}

static int chunk_test_side(enum side* on_sides, c_coord_t x, c_coord_t y,
						   c_coord_t z) {
	assert(on_sides);

	int count = 0;

	if(x == 0)
		on_sides[count++] = SIDE_LEFT;

	if(x == CHUNK_SIZE - 1)
		on_sides[count++] = SIDE_RIGHT;

	if(y == 0)
		on_sides[count++] = SIDE_BOTTOM;

	if(y == CHUNK_SIZE - 1)
		on_sides[count++] = SIDE_TOP;

	if(z == 0)
		on_sides[count++] = SIDE_FRONT;

	if(z == CHUNK_SIZE - 1)
		on_sides[count++] = SIDE_BACK;

	return count;
}

static void chunk_test2(struct chunk* c, struct stack* queue, bool* visited,
						uint8_t* reachable, c_coord_t x, c_coord_t y,
						c_coord_t z) {
	assert(queue && visited && reachable);

	if(visited[CHUNK_INDEX(x, y, z)]
	   || (blocks[c->blocks[CHUNK_INDEX(x, y, z) * 3 + 0]]
		   && !blocks[c->blocks[CHUNK_INDEX(x, y, z) * 3 + 0]]
				   ->can_see_through))
		return;

	stack_clear(queue);
	stack_push(queue, (uint8_t[]) {x, y, z});
	visited[CHUNK_INDEX(x, y, z)] = true;

	uint8_t reached_sides = 0;

	while(!stack_empty(queue)) {
		uint8_t block[3];
		stack_pop(queue, block);

		enum side on_sides[3];
		size_t on_sides_len
			= chunk_test_side(on_sides, block[0], block[1], block[2]);
		assert(on_sides_len <= 3);

		for(size_t k = 0; k < on_sides_len; k++)
			reached_sides |= (1 << on_sides[k]);

		for(int s = 0; s < 6; s++) {
			int nx, ny, nz;
			blocks_side_offset(s, &nx, &ny, &nz);
			nx += block[0];
			ny += block[1];
			nz += block[2];

			if(nx >= 0 && ny >= 0 && nz >= 0 && nx < CHUNK_SIZE
			   && ny < CHUNK_SIZE && nz < CHUNK_SIZE
			   && !visited[CHUNK_INDEX(nx, ny, nz)]
			   && (!blocks[c->blocks[CHUNK_INDEX(nx, ny, nz) * 3 + 0]]
				   || blocks[c->blocks[CHUNK_INDEX(nx, ny, nz) * 3 + 0]]
						  ->can_see_through)) {
				stack_push(queue, (uint8_t[]) {nx, ny, nz});
				visited[CHUNK_INDEX(nx, ny, nz)] = true;
			}
		}
	}

	for(int s = 0; s < 6; s++) {
		if(reached_sides & (1 << s))
			reachable[s] |= reached_sides;
	}
}

void chunk_test(struct chunk* c) {
	assert(c);

	memset(c->reachable, 0, sizeof(c->reachable));

	bool* visited = malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	assert(visited);

	memset(visited, false, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

	struct stack queue;
	stack_create(&queue, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE / 4,
				 sizeof(uint8_t[3]));

	for(int y = 0; y < CHUNK_SIZE; y++) {
		for(int x = 0; x < CHUNK_SIZE; x++) {
			chunk_test2(c, &queue, visited, c->reachable, x, y, 0);
			chunk_test2(c, &queue, visited, c->reachable, x, 0, y);
			chunk_test2(c, &queue, visited, c->reachable, 0, x, y);
			chunk_test2(c, &queue, visited, c->reachable, x, y, CHUNK_SIZE - 1);
			chunk_test2(c, &queue, visited, c->reachable, x, CHUNK_SIZE - 1, y);
			chunk_test2(c, &queue, visited, c->reachable, CHUNK_SIZE - 1, x, y);
		}
	}

	stack_destroy(&queue);
	free(visited);
}

static void chunk_vertex_light(struct chunk* c, uint8_t* light_data) {
	assert(c && light_data);

	for(c_coord_t y = 0; y < CHUNK_SIZE + 2; y++) {
		for(c_coord_t z = 0; z < CHUNK_SIZE + 2; z++) {
			for(c_coord_t x = 0; x < CHUNK_SIZE + 2; x++) {
				struct block_data b1[4] = {
					chunk_lookup_block(c, x + 0, y - 1, z + 0),
					chunk_lookup_block(c, x - 1, y - 1, z + 0),
					chunk_lookup_block(c, x + 0, y - 1, z - 1),
					chunk_lookup_block(c, x - 1, y - 1, z - 1),
				};

				struct block_data b2[4] = {
					chunk_lookup_block(c, x - 1, y + 0, z + 0),
					b1[1],
					chunk_lookup_block(c, x - 1, y + 0, z - 1),
					b1[3],
				};

				struct block_data b3[4] = {
					chunk_lookup_block(c, x + 0, y + 0, z - 1),
					b2[2],
					b1[2],
					b1[3],
				};

				// TODO: clean up horrible code

				int shade_table[5] = {0, 1, 3, 5, 0};

				int sum_sky, sum_torch, count;

				sum_sky = sum_torch = count = 0;
				for(int k = 0; k < 4; k++) {
					if(!blocks[b1[k].type]
					   || (blocks[b1[k].type]->can_see_through
						   && !blocks[b1[k].type]->ignore_lighting)) {
						sum_sky += b1[k].sky_light;
						sum_torch += b1[k].torch_light;
						count++;
					}
				}

				sum_torch = count > 0 ? (sum_torch + count - 1) / count : 0;
				sum_sky = count > 0 ? (sum_sky + count - 1) / count : 0;
				sum_torch = MAX(sum_torch - shade_table[4 - count], 0);
				sum_sky = MAX(sum_sky - shade_table[4 - count], 0);

				light_data[CHUNK_LIGHT_INDEX(x, y, z) * 3 + 0]
					= (sum_torch << 4) | sum_sky;

				sum_sky = sum_torch = count = 0;
				for(int k = 0; k < 4; k++) {
					if(!blocks[b2[k].type]
					   || (blocks[b2[k].type]->can_see_through
						   && !blocks[b2[k].type]->ignore_lighting)) {
						sum_sky += b2[k].sky_light;
						sum_torch += b2[k].torch_light;
						count++;
					}
				}

				sum_torch = count > 0 ? (sum_torch + count - 1) / count : 0;
				sum_sky = count > 0 ? (sum_sky + count - 1) / count : 0;
				sum_torch = MAX(sum_torch - shade_table[4 - count], 0);
				sum_sky = MAX(sum_sky - shade_table[4 - count], 0);

				light_data[CHUNK_LIGHT_INDEX(x, y, z) * 3 + 1]
					= (sum_torch << 4) | sum_sky;

				sum_sky = sum_torch = count = 0;
				for(int k = 0; k < 4; k++) {
					if(!blocks[b3[k].type]
					   || (blocks[b3[k].type]->can_see_through
						   && !blocks[b3[k].type]->ignore_lighting)) {
						sum_sky += b3[k].sky_light;
						sum_torch += b3[k].torch_light;
						count++;
					}
				}

				sum_torch = count > 0 ? (sum_torch + count - 1) / count : 0;
				sum_sky = count > 0 ? (sum_sky + count - 1) / count : 0;
				sum_torch = MAX(sum_torch - shade_table[4 - count], 0);
				sum_sky = MAX(sum_sky - shade_table[4 - count], 0);

				light_data[CHUNK_LIGHT_INDEX(x, y, z) * 3 + 2]
					= (sum_torch << 4) | sum_sky;
			}
		}
	}
}

static void chunk_rebuild(struct chunk* c, struct displaylist* d,
						  uint8_t* light_data, bool count_only,
						  size_t* vertices) {
	assert(c && d && light_data);

	for(int k = 0; k < 13; k++)
		vertices[k] = 0;

	for(c_coord_t y = 0; y < CHUNK_SIZE; y++) {
		for(c_coord_t z = 0; z < CHUNK_SIZE; z++) {
			for(c_coord_t x = 0; x < CHUNK_SIZE; x++) {
				struct block_data local = chunk_get_block(c, x, y, z);

				if(blocks[local.type]) {
					struct block_info local_info = (struct block_info) {
						.block = &local,
						.world = c->world,
						.x = c->x + x,
						.y = c->y + y,
						.z = c->z + z,
					};

					uint8_t vertex_light[24];
					bool light_loaded = count_only;

					for(int k = 0; k < SIDE_MAX; k++) {
						enum side s = (enum side)k;

						int ox, oy, oz;
						blocks_side_offset(s, &ox, &oy, &oz);

						struct block_data neighbours
							= chunk_lookup_block(c, x + ox, y + oy, z + oz);

						struct block_info neighbours_info
							= (struct block_info) {
								.block = &neighbours,
								.world = c->world,
								.x = c->x + x + ox,
								.y = c->y + y + oy,
								.z = c->z + z + oz,
							};

						bool face_visible = true;

						if(blocks[neighbours.type]
						   && ((!blocks[local.type]->transparent
								&& !blocks[neighbours.type]->transparent)
							   || blocks[local.type]->transparent)) {
							struct face_occlusion* a
								= blocks[local.type]->getSideMask(
									&local_info, s, &neighbours_info);
							struct face_occlusion* b
								= blocks[neighbours.type]->getSideMask(
									&neighbours_info, blocks_side_opposite(s),
									&local_info);

							face_visible = face_occlusion_test(a, b);
						}

						int dp_index = k;

						if(blocks[local.type]->transparent)
							dp_index += 6;

						if(blocks[local.type]->double_sided)
							dp_index = 12;

						if(face_visible) {
							if(!light_loaded) {
								light_loaded = true;
								vertex_light[0] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 0, z + 0) * 3
									 + 0];
								vertex_light[1] = light_data
									[CHUNK_LIGHT_INDEX(x + 1, y + 0, z + 0) * 3
									 + 0];
								vertex_light[2] = light_data
									[CHUNK_LIGHT_INDEX(x + 1, y + 0, z + 1) * 3
									 + 0];
								vertex_light[3] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 0, z + 1) * 3
									 + 0];
								vertex_light[4] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 2, z + 0) * 3
									 + 0];
								vertex_light[5] = light_data
									[CHUNK_LIGHT_INDEX(x + 1, y + 2, z + 0) * 3
									 + 0];
								vertex_light[6] = light_data
									[CHUNK_LIGHT_INDEX(x + 1, y + 2, z + 1) * 3
									 + 0];
								vertex_light[7] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 2, z + 1) * 3
									 + 0];

								vertex_light[8] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 0, z + 0) * 3
									 + 1];
								vertex_light[9] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 1, z + 0) * 3
									 + 1];
								vertex_light[10] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 1, z + 1) * 3
									 + 1];
								vertex_light[11] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 0, z + 1) * 3
									 + 1];
								vertex_light[12] = light_data
									[CHUNK_LIGHT_INDEX(x + 2, y + 0, z + 0) * 3
									 + 1];
								vertex_light[13] = light_data
									[CHUNK_LIGHT_INDEX(x + 2, y + 1, z + 0) * 3
									 + 1];
								vertex_light[14] = light_data
									[CHUNK_LIGHT_INDEX(x + 2, y + 1, z + 1) * 3
									 + 1];
								vertex_light[15] = light_data
									[CHUNK_LIGHT_INDEX(x + 2, y + 0, z + 1) * 3
									 + 1];

								vertex_light[16] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 0, z + 0) * 3
									 + 2];
								vertex_light[17] = light_data
									[CHUNK_LIGHT_INDEX(x + 1, y + 0, z + 0) * 3
									 + 2];
								vertex_light[18] = light_data
									[CHUNK_LIGHT_INDEX(x + 1, y + 1, z + 0) * 3
									 + 2];
								vertex_light[19] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 1, z + 0) * 3
									 + 2];
								vertex_light[20] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 0, z + 2) * 3
									 + 2];
								vertex_light[21] = light_data
									[CHUNK_LIGHT_INDEX(x + 1, y + 0, z + 2) * 3
									 + 2];
								vertex_light[22] = light_data
									[CHUNK_LIGHT_INDEX(x + 1, y + 1, z + 2) * 3
									 + 2];
								vertex_light[23] = light_data
									[CHUNK_LIGHT_INDEX(x + 0, y + 1, z + 2) * 3
									 + 2];
							}

							vertices[dp_index]
								+= blocks[local.type]->renderBlock(
									   d + dp_index, &local_info, s,
									   &neighbours_info, vertex_light,
									   count_only)
								* 4;
						}
					}
				}
			}
		}
	}
}

void chunk_check_built(struct chunk* c) {
	assert(c);

	if(c->rebuild_displist) {
		uint8_t* light_data = malloc((CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)
									 * (CHUNK_SIZE + 2) * 3);
		assert(light_data);

		chunk_vertex_light(c, light_data);

		for(int k = 0; k < 13; k++) {
			if(c->has_displist[k]) {
				displaylist_destroy(c->mesh + k);
				c->has_displist[k] = false;
			}
		}

		size_t vertices[13];
		chunk_rebuild(c, c->mesh, light_data, true, vertices);

		for(int k = 0; k < 13; k++) {
			if(vertices[k] > 0 && vertices[k] <= 0xFFFF * 4) {
				displaylist_init(c->mesh + k, vertices[k], 3 * 2 + 2 * 1 + 1);
				displaylist_begin(c->mesh + k, GX_QUADS, GX_VTXFMT0,
								  vertices[k]);
				c->has_displist[k] = true;
			}
		}

		chunk_rebuild(c, c->mesh, light_data, false, vertices);

		for(int k = 0; k < 13; k++) {
			if(c->has_displist[k])
				displaylist_end(c->mesh + k);
		}

		free(light_data);

		chunk_test(c);

		c->rebuild_displist = false;
	}
}

void chunk_pre_render(struct chunk* c, Mtx view) {
	assert(c && view);

	Mtx model;
	guMtxTrans(model, c->x, c->y, c->z);
	guMtxConcat(view, model, c->model_view);
}

static void check_matrix_set(struct chunk* c, bool* needs_matrix) {
	if(*needs_matrix) {
		GX_LoadPosMtxImm(c->model_view, GX_PNMTX0);
		*needs_matrix = false;
	}
}

void chunk_render(struct chunk* c, bool pass, float x, float y, float z) {
	assert(c);

	chunk_check_built(c);

	bool needs_matrix = true;
	int offset = pass ? 6 : 0;

	if(y < c->y + CHUNK_SIZE && c->has_displist[SIDE_BOTTOM + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_BOTTOM + offset);
	}

	if(y > c->y && c->has_displist[SIDE_TOP + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_TOP + offset);
	}

	if(x < c->x + CHUNK_SIZE && c->has_displist[SIDE_LEFT + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_LEFT + offset);
	}

	if(x > c->x && c->has_displist[SIDE_RIGHT + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_RIGHT + offset);
	}

	if(z < c->z + CHUNK_SIZE && c->has_displist[SIDE_FRONT + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_FRONT + offset);
	}

	if(z > c->z && c->has_displist[SIDE_BACK + offset]) {
		check_matrix_set(c, &needs_matrix);
		displaylist_render(c->mesh + SIDE_BACK + offset);
	}

	if(!pass && c->has_displist[12]) {
		check_matrix_set(c, &needs_matrix);
		GX_SetCullMode(GX_CULL_NONE);
		displaylist_render(c->mesh + 12);
		GX_SetCullMode(GX_CULL_BACK);
	}
}
