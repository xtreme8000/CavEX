#include <assert.h>

#include "platform/graphics/gfx.h"
#include "world.h"

char* chunk_files[] = {
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

static struct chunk chunks_base[sizeof(chunk_files) / sizeof(*chunk_files) * 8];
static struct chunk* chunks_end = chunks_base;

static void load_chunk(struct world* w, struct chunk** c, char* file) {
	uint8_t* chunk_data = malloc(16 * 16 * 128 * 3);

	int32_t chunk_x, chunk_z;

	FILE* f = fopen(file, "rb");
	assert(f);
	fread((uint8_t*)&chunk_x + 3, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_x + 2, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_x + 1, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_x + 0, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_z + 3, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_z + 2, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_z + 1, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_z + 0, sizeof(uint8_t), 1, f);
	fread(chunk_data, 1, 16 * 16 * 128 * 3, f);
	fclose(f);

	for(int k = 0; k < 8; k++) {
		chunk_init(*c, w, chunk_x * 16, k * 16, chunk_z * 16);
		dict_chunks_set_at(w->chunks, CHUNK_TO_ID(chunk_x, k, chunk_z), *c);

		for(int y = 0; y < CHUNK_SIZE; y++) {
			for(int z = 0; z < CHUNK_SIZE; z++) {
				for(int x = 0; x < CHUNK_SIZE; x++) {
					uint8_t blockid
						= chunk_data[(x + ((y + k * 16) * 16 + z) * 16) * 3
									 + 0];
					uint8_t metadata
						= chunk_data[(x + ((y + k * 16) * 16 + z) * 16) * 3
									 + 1];
					uint8_t light
						= chunk_data[(x + ((y + k * 16) * 16 + z) * 16) * 3
									 + 2];

					chunk_set_block(*c, x, y, z,
									(struct block_data) {
										.type = blockid,
										.metadata = metadata,
										.sky_light = light & 0xF,
										.torch_light = light >> 4,
									});
				}
			}
		}

		(*c)++;
	}

	free(chunk_data);
}

static void world_bfs(struct world* w, ilist_chunks_t render, float x, float y,
					  float z, vec4* planes) {
	assert(w && render && planes);

	for(struct chunk* c = chunks_base; c != chunks_end; c++) {
		c->tmp_data.visited = false;
		c->tmp_data.steps = 0;
	}

	enum side sides[6]
		= {SIDE_TOP, SIDE_LEFT, SIDE_BACK, SIDE_BOTTOM, SIDE_RIGHT, SIDE_FRONT};

	struct chunk* c_camera = world_find_chunk(w, x, y, z);

	ilist_chunks_t queue;
	ilist_chunks_init(queue);

	if(!c_camera)
		return;

	ilist_chunks_push_back(queue, c_camera);
	c_camera->tmp_data.from = SIDE_MAX;
	c_camera->tmp_data.used_exit_sides = 0;
	c_camera->tmp_data.steps = 0;
	c_camera->tmp_data.visited = true;

	while(!ilist_chunks_empty_p(queue)) {
		struct chunk* current = ilist_chunks_pop_front(queue);
		ilist_chunks_push_back(render, current);

		for(int s = 0; s < 6; s++) {
			struct chunk* neigh
				= world_find_chunk_neighbour(w, current, sides[s]);

			if(neigh && !neigh->tmp_data.visited && neigh->tmp_data.steps < 6
			   && !(current->tmp_data.used_exit_sides & (1 << sides[s]))
			   && (current->tmp_data.from == SIDE_MAX
				   || current->reachable[current->tmp_data.from]
					   & (1 << sides[s]))
			   && glm_aabb_frustum(
				   (vec3[2]) {{neigh->x, neigh->y, neigh->z},
							  {neigh->x + CHUNK_SIZE, neigh->y + CHUNK_SIZE,
							   neigh->z + CHUNK_SIZE}},
				   planes)) {
				ilist_chunks_push_back(queue, neigh);
				neigh->tmp_data.from = blocks_side_opposite(sides[s]);
				neigh->tmp_data.used_exit_sides
					= current->tmp_data.used_exit_sides
					| (1 << blocks_side_opposite(sides[s]));
				neigh->tmp_data.steps
					= current->tmp_data.steps + (neigh->y < 64) ? 1 : 0;
				neigh->tmp_data.visited = true;
			}
		}
	}
}

void world_create(struct world* w) {
	assert(w);

	dict_chunks_init(w->chunks);
	w->world_chunk_cache = NULL;

	for(size_t k = 0; k < sizeof(chunk_files) / sizeof(*chunk_files); k++)
		load_chunk(w, &chunks_end, chunk_files[k]);

	w->anim_timer = time_get();
}

void world_destroy(struct world* w) {
	dict_chunks_clear(w->chunks);
}

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

struct chunk* world_find_chunk_neighbour(struct world* w, struct chunk* c,
										 enum side s) {
	assert(w && c);

	int x, y, z;
	blocks_side_offset(s, &x, &y, &z);

	if(y + c->y / CHUNK_SIZE < 0
	   || y + c->y / CHUNK_SIZE >= WORLD_HEIGHT / CHUNK_SIZE)
		return NULL;

	struct chunk** res = dict_chunks_get(w->chunks,
										 CHUNK_TO_ID(x + c->x / CHUNK_SIZE,
													 y + c->y / CHUNK_SIZE,
													 z + c->z / CHUNK_SIZE));

	return res ? *res : NULL;
}

struct chunk* world_find_chunk(struct world* w, w_coord_t x, w_coord_t y,
							   w_coord_t z) {
	assert(w);

	if(y < 0 || y >= WORLD_HEIGHT)
		return NULL;

	int cx = WCOORD_CHUNK_OFFSET(x);
	int cy = y / CHUNK_SIZE;
	int cz = WCOORD_CHUNK_OFFSET(z);

	if(w->world_chunk_cache
	   && CHUNK_TO_ID(cx, cy, cz)
		   == CHUNK_TO_ID(w->world_chunk_cache->x / CHUNK_SIZE,
						  w->world_chunk_cache->y / CHUNK_SIZE,
						  w->world_chunk_cache->z / CHUNK_SIZE))
		return w->world_chunk_cache;

	struct chunk** res = dict_chunks_get(w->chunks, CHUNK_TO_ID(cx, cy, cz));

	if(res)
		w->world_chunk_cache = *res;

	return res ? *res : NULL;
}

void world_preload(struct world* w,
				   void (*progress)(struct world* w, float percent)) {
	assert(w);

	dict_chunks_it_t it;
	dict_chunks_it(it, w->chunks);

	size_t total = dict_chunks_size(w->chunks);
	size_t count = 0;

	while(!dict_chunks_end_p(it)) {
		chunk_check_built(dict_chunks_ref(it)->value);

		if(progress)
			progress(w, (float)count / (float)total);
		count++;
		dict_chunks_next(it);
	}
}

bool world_block_intersection(struct world* w, struct ray* r, w_coord_t x,
							  w_coord_t y, w_coord_t z, enum side* s) {
	assert(w && r && s);

	struct block_data blk = world_get_block(w, x, y, z);

	if(blocks[blk.type]) {
		struct AABB bbox;

		if(blocks[blk.type]->getBoundingBox(
			   &(struct block_info) {
				   .block = &blk, .world = w, .x = x, .y = y, .z = z},
			   false, &bbox)) {
			aabb_translate(&bbox, x, y, z);
			return aabb_intersection_ray(&bbox, r, s);
		} else {
			return false;
		}
	}

	return false;
}

void world_pre_render(struct world* w, struct camera* c, mat4 view) {
	assert(w && c && view);

	ilist_chunks_init(w->render);
	world_bfs(w, w->render, c->x, c->y, c->z, c->frustum_planes);

	ilist_chunks_it_t it;
	ilist_chunks_it(it, w->render);

	while(!ilist_chunks_end_p(it)) {
		chunk_pre_render(ilist_chunks_ref(it), view);
		ilist_chunks_next(it);
	}
}

size_t world_render(struct world* w, struct camera* c, bool pass) {
	assert(w && c);

	size_t in_view = 0;
	ilist_chunks_it_t it;

	if(!pass) {
		gfx_bind_texture(TEXTURE_TERRAIN);
		gfx_lighting(true);
		gfx_blending(MODE_OFF);
		gfx_alpha_test(true);

		ilist_chunks_it(it, w->render);

		while(!ilist_chunks_end_p(it)) {
			chunk_render(ilist_chunks_ref(it), false, c->x, c->y, c->z);
			in_view++;
			ilist_chunks_next(it);
		}
	} else {
		gfx_alpha_test(false);
		gfx_blending(MODE_BLEND);
		gfx_write_buffers(false, true);

		mat4 matrix_anim;
		int anim = (time_diff_ms(w->anim_timer, time_get()) * 3 / 400) % 24;
		glm_translate_make(
			matrix_anim,
			(vec3) {(anim / 14) * 0.3515625F, (anim % 14) * 0.0703125F, 0.0F});

		gfx_matrix_texture(true, matrix_anim);
		gfx_bind_texture(TEXTURE_ANIM);

		for(int t_pass = 0; t_pass < 2; t_pass++) {
			if(t_pass == 1)
				gfx_write_buffers(true, false);

			ilist_chunks_it(it, w->render);
			while(!ilist_chunks_end_p(it)) {
				chunk_render(ilist_chunks_ref(it), true, c->x, c->y, c->z);
				ilist_chunks_next(it);
			}
		}

		gfx_matrix_texture(false, NULL);
		gfx_write_buffers(true, true);
	}

	return in_view;
}
