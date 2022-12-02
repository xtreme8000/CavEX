#include <assert.h>
#include <gccore.h>
#include <stdint.h>

#include "chunk_mesher.h"
#include "platform/graphics/displaylist.h"
#include "stack.h"
#include "world.h"

#define BLK_INDEX(x, y, z)                                                     \
	((x) + ((z) + (y) * (CHUNK_SIZE + 2)) * (CHUNK_SIZE + 2))
#define BLK_INDEX2(x, y, z) ((x) + ((z) + (y)*CHUNK_SIZE) * CHUNK_SIZE)
#define BLK_DATA(b, x, y, z) ((b)[BLK_INDEX((x) + 1, (y) + 1, (z) + 1)])

struct chunk_mesher_rpc {
	struct chunk* chunk;
	// ingoing
	struct {
		struct block_data* blocks;
	} request;
	// outgoing
	struct {
		struct displaylist mesh[13];
		bool has_displist[13];
		uint8_t reachable[6];
	} result;
};

static struct chunk_mesher_rpc rpc_msg[CHUNK_MESHER_QLENGTH];
static mqbox_t mesher_requests;
static mqbox_t mesher_results;
static mqbox_t mesher_empty_msg;

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

static void chunk_test(struct block_data* bd, struct stack* queue,
					   bool* visited, uint8_t* reachable, c_coord_t x,
					   c_coord_t y, c_coord_t z) {
	assert(bd && queue && visited && reachable);

	if(visited[BLK_INDEX2(x, y, z)]
	   || (blocks[BLK_DATA(bd, x, y, z).type]
		   && !blocks[BLK_DATA(bd, x, y, z).type]->can_see_through))
		return;

	stack_clear(queue);
	stack_push(queue, (uint8_t[]) {x, y, z});
	visited[BLK_INDEX2(x, y, z)] = true;

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
			   && !visited[BLK_INDEX2(nx, ny, nz)]
			   && (!blocks[BLK_DATA(bd, nx, ny, nz).type]
				   || blocks[BLK_DATA(bd, nx, ny, nz).type]->can_see_through)) {
				stack_push(queue, (uint8_t[]) {nx, ny, nz});
				visited[BLK_INDEX2(nx, ny, nz)] = true;
			}
		}
	}

	for(int s = 0; s < 6; s++) {
		if(reached_sides & (1 << s))
			reachable[s] |= reached_sides;
	}
}

static void chunk_test_init(struct block_data* bd, uint8_t* reachable) {
	assert(bd && reachable);

	memset(reachable, 0, 6 * sizeof(uint8_t));

	bool* visited = malloc(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);
	assert(visited);

	memset(visited, false, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE);

	struct stack queue;
	stack_create(&queue, CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE / 4,
				 sizeof(uint8_t[3]));

	for(int y = 0; y < CHUNK_SIZE; y++) {
		for(int x = 0; x < CHUNK_SIZE; x++) {
			chunk_test(bd, &queue, visited, reachable, x, y, 0);
			chunk_test(bd, &queue, visited, reachable, x, 0, y);
			chunk_test(bd, &queue, visited, reachable, 0, x, y);
			chunk_test(bd, &queue, visited, reachable, x, y, CHUNK_SIZE - 1);
			chunk_test(bd, &queue, visited, reachable, x, CHUNK_SIZE - 1, y);
			chunk_test(bd, &queue, visited, reachable, CHUNK_SIZE - 1, x, y);
		}
	}

	stack_destroy(&queue);
	free(visited);
}

static void chunk_mesher_vertex_light(struct block_data* bd,
									  uint8_t* light_data) {
	assert(bd && light_data);

	for(c_coord_t y = 0; y < CHUNK_SIZE + 2; y++) {
		for(c_coord_t z = 0; z < CHUNK_SIZE + 2; z++) {
			for(c_coord_t x = 0; x < CHUNK_SIZE + 2; x++) {
				struct block_data b1[4] = {
					BLK_DATA(bd, x + 0, y - 1, z + 0),
					BLK_DATA(bd, x - 1, y - 1, z + 0),
					BLK_DATA(bd, x + 0, y - 1, z - 1),
					BLK_DATA(bd, x - 1, y - 1, z - 1),
				};

				struct block_data b2[4] = {
					BLK_DATA(bd, x - 1, y + 0, z + 0),
					b1[1],
					BLK_DATA(bd, x - 1, y + 0, z - 1),
					b1[3],
				};

				struct block_data b3[4] = {
					BLK_DATA(bd, x + 0, y + 0, z - 1),
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

				light_data[BLK_INDEX(x, y, z) * 3 + 0]
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

				light_data[BLK_INDEX(x, y, z) * 3 + 1]
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

				light_data[BLK_INDEX(x, y, z) * 3 + 2]
					= (sum_torch << 4) | sum_sky;
			}
		}
	}
}

static void chunk_mesher_rebuild(struct block_data* bd, w_coord_t cx,
								 w_coord_t cy, w_coord_t cz,
								 struct displaylist* d, bool count_only,
								 size_t* vertices) {
	assert(bd && d && vertices);

	uint8_t* light_data = NULL;

	for(int k = 0; k < 13; k++)
		vertices[k] = 0;

	for(c_coord_t y = 0; y < CHUNK_SIZE; y++) {
		for(c_coord_t z = 0; z < CHUNK_SIZE; z++) {
			for(c_coord_t x = 0; x < CHUNK_SIZE; x++) {
				struct block_data local = BLK_DATA(bd, x, y, z);

				if(blocks[local.type]) {
					struct block_data neighbours[6];
					struct block_info neighbours_info[6];

					for(int k = 0; k < SIDE_MAX; k++) {
						enum side s = (enum side)k;

						int ox, oy, oz;
						blocks_side_offset(s, &ox, &oy, &oz);

						neighbours[k] = BLK_DATA(bd, x + ox, y + oy, z + oz);

						neighbours_info[k] = (struct block_info) {
							.block = neighbours + k,
							.neighbours = NULL,
							.x = cx + x + ox,
							.y = cy + y + oy,
							.z = cz + z + oz,
						};
					}

					struct block_info local_info = (struct block_info) {
						.block = &local,
						.neighbours = neighbours,
						.x = cx + x,
						.y = cy + y,
						.z = cz + z,
					};

					uint8_t vertex_light[24];
					bool light_loaded = count_only;

					for(int k = 0; k < SIDE_MAX; k++) {
						enum side s = (enum side)k;

						bool face_visible = true;

						if(blocks[neighbours[k].type]
						   && ((!blocks[local.type]->transparent
								&& !blocks[neighbours[k].type]->transparent)
							   || blocks[local.type]->transparent)) {
							struct face_occlusion* a
								= blocks[local.type]->getSideMask(
									&local_info, s, neighbours_info + k);
							struct face_occlusion* b
								= blocks[neighbours[k].type]->getSideMask(
									neighbours_info + k,
									blocks_side_opposite(s), &local_info);

							face_visible = face_occlusion_test(a, b);
						}

						int dp_index = k;

						if(blocks[local.type]->transparent)
							dp_index += 6;

						if(blocks[local.type]->double_sided)
							dp_index = 12;

						if(face_visible) {
							if(!light_data) {
								light_data
									= malloc((CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)
											 * (CHUNK_SIZE + 2) * 3);
								assert(light_data);
								chunk_mesher_vertex_light(bd, light_data);
							}

							if(!light_loaded) {
								light_loaded = true;
								vertex_light[0]
									= light_data[BLK_INDEX(x + 0, y + 0, z + 0)
													 * 3
												 + 0];
								vertex_light[1]
									= light_data[BLK_INDEX(x + 1, y + 0, z + 0)
													 * 3
												 + 0];
								vertex_light[2]
									= light_data[BLK_INDEX(x + 1, y + 0, z + 1)
													 * 3
												 + 0];
								vertex_light[3]
									= light_data[BLK_INDEX(x + 0, y + 0, z + 1)
													 * 3
												 + 0];
								vertex_light[4]
									= light_data[BLK_INDEX(x + 0, y + 2, z + 0)
													 * 3
												 + 0];
								vertex_light[5]
									= light_data[BLK_INDEX(x + 1, y + 2, z + 0)
													 * 3
												 + 0];
								vertex_light[6]
									= light_data[BLK_INDEX(x + 1, y + 2, z + 1)
													 * 3
												 + 0];
								vertex_light[7]
									= light_data[BLK_INDEX(x + 0, y + 2, z + 1)
													 * 3
												 + 0];

								vertex_light[8]
									= light_data[BLK_INDEX(x + 0, y + 0, z + 0)
													 * 3
												 + 1];
								vertex_light[9]
									= light_data[BLK_INDEX(x + 0, y + 1, z + 0)
													 * 3
												 + 1];
								vertex_light[10]
									= light_data[BLK_INDEX(x + 0, y + 1, z + 1)
													 * 3
												 + 1];
								vertex_light[11]
									= light_data[BLK_INDEX(x + 0, y + 0, z + 1)
													 * 3
												 + 1];
								vertex_light[12]
									= light_data[BLK_INDEX(x + 2, y + 0, z + 0)
													 * 3
												 + 1];
								vertex_light[13]
									= light_data[BLK_INDEX(x + 2, y + 1, z + 0)
													 * 3
												 + 1];
								vertex_light[14]
									= light_data[BLK_INDEX(x + 2, y + 1, z + 1)
													 * 3
												 + 1];
								vertex_light[15]
									= light_data[BLK_INDEX(x + 2, y + 0, z + 1)
													 * 3
												 + 1];

								vertex_light[16]
									= light_data[BLK_INDEX(x + 0, y + 0, z + 0)
													 * 3
												 + 2];
								vertex_light[17]
									= light_data[BLK_INDEX(x + 1, y + 0, z + 0)
													 * 3
												 + 2];
								vertex_light[18]
									= light_data[BLK_INDEX(x + 1, y + 1, z + 0)
													 * 3
												 + 2];
								vertex_light[19]
									= light_data[BLK_INDEX(x + 0, y + 1, z + 0)
													 * 3
												 + 2];
								vertex_light[20]
									= light_data[BLK_INDEX(x + 0, y + 0, z + 2)
													 * 3
												 + 2];
								vertex_light[21]
									= light_data[BLK_INDEX(x + 1, y + 0, z + 2)
													 * 3
												 + 2];
								vertex_light[22]
									= light_data[BLK_INDEX(x + 1, y + 1, z + 2)
													 * 3
												 + 2];
								vertex_light[23]
									= light_data[BLK_INDEX(x + 0, y + 1, z + 2)
													 * 3
												 + 2];
							}

							vertices[dp_index]
								+= blocks[local.type]->renderBlock(
									   d + dp_index, &local_info, s,
									   neighbours_info + k, vertex_light,
									   count_only)
								* 4;
						}
					}
				}
			}
		}
	}

	if(light_data)
		free(light_data);
}

static void chunk_mesher_build(struct chunk_mesher_rpc* req) {
	for(int k = 0; k < 13; k++) {
		req->result.has_displist[k] = false;
		displaylist_init(req->result.mesh + k, 256, 3 * 2 + 2 * 1 + 1);
	}

	size_t vertices[13];
	chunk_mesher_rebuild(req->request.blocks, req->chunk->x, req->chunk->y,
						 req->chunk->z, req->result.mesh, false, vertices);

	for(int k = 0; k < 13; k++) {
		if(vertices[k] > 0 && vertices[k] <= 0xFFFF * 4) {
			displaylist_finalize(req->result.mesh + k, GX_QUADS, GX_VTXFMT0,
								 vertices[k]);
			req->result.has_displist[k] = true;
		} else {
			displaylist_destroy(req->result.mesh + k);
		}
	}

	chunk_test_init(req->request.blocks, req->result.reachable);

	free(req->request.blocks);
}

static void* chunk_mesher_local_thread(void* user) {
	while(1) {
		struct chunk_mesher_rpc* request;
		MQ_Receive(mesher_requests, (mqmsg_t*)&request, MQ_MSG_BLOCK);
		chunk_mesher_build(request);
		MQ_Send(mesher_results, request, MQ_MSG_BLOCK);
	}

	return NULL;
}

void chunk_mesher_init() {
	MQ_Init(&mesher_requests, CHUNK_MESHER_QLENGTH);
	MQ_Init(&mesher_results, CHUNK_MESHER_QLENGTH);
	MQ_Init(&mesher_empty_msg, CHUNK_MESHER_QLENGTH);

	for(int k = 0; k < CHUNK_MESHER_QLENGTH; k++)
		MQ_Send(mesher_empty_msg, rpc_msg + k, MQ_MSG_BLOCK);

	lwp_t thread;
	LWP_CreateThread(&thread, chunk_mesher_local_thread, NULL, NULL, 0, 4);
}

void chunk_mesher_receive() {
	struct chunk_mesher_rpc* result;
	while(MQ_Receive(mesher_results, (mqmsg_t*)&result, MQ_MSG_NOBLOCK)) {
		for(int k = 0; k < 13; k++) {
			if(result->chunk->has_displist[k])
				displaylist_destroy(result->chunk->mesh + k);

			result->chunk->mesh[k] = result->result.mesh[k];
			result->chunk->has_displist[k] = result->result.has_displist[k];
		}

		for(int k = 0; k < 6; k++)
			result->chunk->reachable[k] = result->result.reachable[k];

		chunk_unref(result->chunk);

		MQ_Send(mesher_empty_msg, result, MQ_MSG_BLOCK);
	}
}

bool chunk_mesher_send(struct chunk* c) {
	assert(c);

	struct chunk_mesher_rpc* request;
	if(!MQ_Receive(mesher_empty_msg, (mqmsg_t*)&request, MQ_MSG_NOBLOCK))
		return false;

	struct block_data* bd
		= malloc((CHUNK_SIZE + 2) * (CHUNK_SIZE + 2) * (CHUNK_SIZE + 2)
				 * sizeof(struct block_data));

	if(!bd) {
		MQ_Send(mesher_empty_msg, request, MQ_MSG_BLOCK);
		return false;
	}

	chunk_ref(c);

	request->chunk = c;
	request->request.blocks = bd;

	for(w_coord_t y = -1; y < CHUNK_SIZE + 1; y++) {
		for(w_coord_t z = -1; z < CHUNK_SIZE + 1; z++) {
			for(w_coord_t x = -1; x < CHUNK_SIZE + 1; x++) {
				bd[BLK_INDEX(x + 1, y + 1, z + 1)]
					= chunk_lookup_block(c, x, y, z);
			}
		}
	}

	MQ_Send(mesher_requests, request, MQ_MSG_BLOCK);
	return true;
}
