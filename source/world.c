/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	CavEX is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with CavEX.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>

#include "game/game_state.h"
#include "lighting.h"
#include "platform/gfx.h"
#include "world.h"

// params depend on fog texture
#define FOG_DIST_NO_RENDER 1.13F
#define FOG_DIST_NO_EFFECT 0.72F

#define FOG_DIST_LESS(c, dist)                                                 \
	(glm_vec2_distance2(                                                       \
		 (vec2) {(c)->x + CHUNK_SIZE / 2, (c)->z + CHUNK_SIZE / 2},            \
		 (vec2) {gstate.camera.x, gstate.camera.z})                            \
	 <= glm_pow2((dist)*gstate.config.fog_distance))

void world_unload_section(struct world* w, w_coord_t x, w_coord_t z) {
	assert(w);

	struct world_section* s
		= dict_wsection_get(w->sections, SECTION_TO_ID(x, z));

	if(s) {
		for(size_t k = 0; k < COLUMN_HEIGHT; k++) {
			struct chunk* c = s->column[k];
			if(c) {
				if(w->world_chunk_cache == c)
					w->world_chunk_cache = NULL;
				chunk_unref(c);
			}
		}

		dict_wsection_erase(w->sections, SECTION_TO_ID(x, z));
	}
}

void world_unload_all(struct world* w) {
	assert(w);

	dict_wsection_it_t it;
	dict_wsection_it(it, w->sections);

	while(!dict_wsection_end_p(it)) {
		struct world_section* s = &dict_wsection_ref(it)->value;
		for(size_t k = 0; k < COLUMN_HEIGHT; k++) {
			if(s->column[k])
				chunk_unref(s->column[k]);
		}

		dict_wsection_next(it);
	}

	stack_clear(&w->lighting_updates);
	dict_wsection_reset(w->sections);
	w->world_chunk_cache = NULL;
}

static void world_bfs(struct world* w, ilist_chunks_t render, float x, float y,
					  float z, vec4* planes) {
	assert(w && render && planes);

	dict_wsection_it_t it;
	dict_wsection_it(it, w->sections);

	while(!dict_wsection_end_p(it)) {
		struct world_section* s = &dict_wsection_ref(it)->value;
		for(size_t k = 0; k < COLUMN_HEIGHT; k++) {
			if(s->column[k])
				s->column[k]->tmp_data = (struct chunk_step) {
					.visited = false,
					.steps = 0,
				};
		}

		dict_wsection_next(it);
	}

	enum side sides[6]
		= {SIDE_TOP, SIDE_LEFT, SIDE_BACK, SIDE_BOTTOM, SIDE_RIGHT, SIDE_FRONT};

	struct chunk* c_camera = world_find_chunk(w, x, y, z);

	ilist_chunks_t queue;
	ilist_chunks_init(queue);

	if(!c_camera)
		return;

	ilist_chunks_push_back(queue, c_camera);
	c_camera->tmp_data = (struct chunk_step) {
		.from = SIDE_MAX,
		.used_exit_sides = 0,
		.steps = 0,
		.visited = true,
	};

	while(!ilist_chunks_empty_p(queue)) {
		struct chunk* current = ilist_chunks_pop_front(queue);
		ilist_chunks_push_back(render, current);
		chunk_ref(current);

		for(int s = 0; s < 6; s++) {
			struct chunk* neigh
				= world_find_chunk_neighbour(w, current, sides[s]);

			if(neigh && !neigh->tmp_data.visited && neigh->tmp_data.steps < 6
			   && !(current->tmp_data.used_exit_sides & (1 << sides[s]))
			   && (current->tmp_data.from == SIDE_MAX
				   || current->reachable[current->tmp_data.from]
					   & (1 << sides[s]))
			   && FOG_DIST_LESS(neigh, FOG_DIST_NO_RENDER)
			   && glm_aabb_frustum(
				   (vec3[2]) {{neigh->x, neigh->y, neigh->z},
							  {neigh->x + CHUNK_SIZE, neigh->y + CHUNK_SIZE,
							   neigh->z + CHUNK_SIZE}},
				   planes)) {
				ilist_chunks_push_back(queue, neigh);
				neigh->tmp_data = (struct chunk_step) {
					.from = blocks_side_opposite(sides[s]),
					.used_exit_sides = current->tmp_data.used_exit_sides
						| (1 << blocks_side_opposite(sides[s])),
					.steps = current->tmp_data.steps + (neigh->y < 64) ? 1 : 0,
					.visited = true,
				};
			}
		}
	}
}

void world_create(struct world* w) {
	assert(w);

	dict_wsection_init(w->sections);
	ilist_chunks_init(w->render);
	ilist_chunks2_init(w->gpu_busy_chunks);
	stack_create(&w->lighting_updates, 16,
				 sizeof(struct world_modification_entry));
	w->world_chunk_cache = NULL;
	w->anim_timer = time_get();
}

void world_destroy(struct world* w) {
	assert(w);

	world_unload_all(w);
	stack_destroy(&w->lighting_updates);
	dict_wsection_clear(w->sections);
}

size_t world_loaded_chunks(struct world* w) {
	assert(w);
	return dict_wsection_size(w->sections);
}

struct block_data world_get_block(struct world* w, w_coord_t x, w_coord_t y,
								  w_coord_t z) {
	assert(w);
	struct chunk* c = world_find_chunk(w, x, y, z);

	return c ? chunk_get_block(c, W2C_COORD(x), W2C_COORD(y), W2C_COORD(z)) :
			   (struct block_data) {
				   .type = (y < WORLD_HEIGHT) ? 1 : 0,
				   .metadata = 0,
				   .sky_light = (y < WORLD_HEIGHT) ? 0 : 15,
				   .torch_light = 0,
			   };
}

w_coord_t world_get_height(struct world* w, w_coord_t x, w_coord_t z) {
	assert(w);

	w_coord_t cx = WCOORD_CHUNK_OFFSET(x);
	w_coord_t cz = WCOORD_CHUNK_OFFSET(z);
	struct world_section* s
		= dict_wsection_get(w->sections, SECTION_TO_ID(cx, cz));

	return s ? s->heightmap[W2C_COORD(x) + W2C_COORD(z) * CHUNK_SIZE] : 0;
}

void world_copy_heightmap(struct world* w, struct chunk* c,
						  uint8_t* heightmap) {
	assert(w && c && heightmap);
	struct world_section* s = dict_wsection_get(
		w->sections, SECTION_TO_ID(c->x / CHUNK_SIZE, c->z / CHUNK_SIZE));
	assert(s);

	memcpy(heightmap, s->heightmap, sizeof(s->heightmap));
}

static bool wsection_heightmap_get_block(void* user, c_coord_t x, w_coord_t y,
										 c_coord_t z, struct block_data* blk) {
	assert(user);
	struct world_section* s = user;
	struct chunk* c = s->column[y / CHUNK_SIZE];
	if(c)
		*blk = chunk_get_block(c, x, W2C_COORD(y), z);
	return c;
}

void world_set_block(struct world* w, w_coord_t x, w_coord_t y, w_coord_t z,
					 struct block_data blk, bool light_update) {
	assert(w);

	if(y < 0 || y >= WORLD_HEIGHT)
		return;

	if(light_update) {
		stack_push(&w->lighting_updates,
				   &(struct world_modification_entry) {
					   .x = x,
					   .y = y,
					   .z = z,
					   .blk = blk,
				   });
	} else {
		w_coord_t cx = WCOORD_CHUNK_OFFSET(x);
		w_coord_t cz = WCOORD_CHUNK_OFFSET(z);
		struct world_section* s
			= dict_wsection_get(w->sections, SECTION_TO_ID(cx, cz));
		struct chunk* c = world_chunk_from_section(w, s, y);

		if(!c) {
			c = malloc(sizeof(struct chunk));
			assert(c);

			w_coord_t cy = y / CHUNK_SIZE;
			chunk_init(c, w, cx * CHUNK_SIZE, cy * CHUNK_SIZE, cz * CHUNK_SIZE);
			chunk_ref(c);

			w->world_chunk_cache = c;

			if(!s) {
				s = dict_wsection_safe_get(w->sections, SECTION_TO_ID(cx, cz));
				assert(s);
				memset(s->heightmap, 0, sizeof(s->heightmap));
				memset(s->column, 0, sizeof(s->column));
			}

			assert(s->column[cy] == NULL);
			s->column[cy] = c;
		}

		chunk_set_block(c, W2C_COORD(x), W2C_COORD(y), W2C_COORD(z), blk);
		lighting_heightmap_update(s->heightmap, W2C_COORD(x), y, W2C_COORD(z),
								  blk.type, wsection_heightmap_get_block, s);
	}
}

static bool world_light_get_block(void* user, w_coord_t x, w_coord_t y,
								  w_coord_t z, struct block_data* blk,
								  uint8_t* height) {
	assert(user);
	struct world* w = user;

	struct world_section* s = dict_wsection_get(
		w->sections,
		SECTION_TO_ID(WCOORD_CHUNK_OFFSET(x), WCOORD_CHUNK_OFFSET(z)));

	if(!s)
		return false;

	struct chunk* c = world_chunk_from_section(w, s, y);

	if(!c)
		return false;

	if(blk)
		*blk = chunk_get_block(c, W2C_COORD(x), W2C_COORD(y), W2C_COORD(z));

	if(height)
		*height = s->heightmap[W2C_COORD(x) + W2C_COORD(z) * CHUNK_SIZE];

	return true;
}

static void world_light_set_light(void* user, w_coord_t x, w_coord_t y,
								  w_coord_t z, uint8_t light) {
	assert(user);
	struct chunk* c = world_find_chunk((struct world*)user, x, y, z);
	assert(c);

	chunk_set_light(c, W2C_COORD(x), W2C_COORD(y), W2C_COORD(z), light);
}

void world_update_lighting(struct world* w) {
	assert(w);

	if(stack_empty(&w->lighting_updates))
		return;

	struct world_modification_entry source;
	stack_pop(&w->lighting_updates, &source);

	world_set_block(w, source.x, source.y, source.z, source.blk, false);
	lighting_update_at_block(source, false, world_light_get_block,
							 world_light_set_light, w);
}

struct chunk* world_find_chunk_neighbour(struct world* w, struct chunk* c,
										 enum side s) {
	assert(w && c);

	int x, y, z;
	blocks_side_offset(s, &x, &y, &z);

	if(y + c->y / CHUNK_SIZE < 0
	   || y + c->y / CHUNK_SIZE >= WORLD_HEIGHT / CHUNK_SIZE)
		return NULL;

	struct world_section* res = dict_wsection_get(
		w->sections,
		SECTION_TO_ID(x + c->x / CHUNK_SIZE, z + c->z / CHUNK_SIZE));

	return res ? res->column[y + c->y / CHUNK_SIZE] : NULL;
}

struct chunk* world_chunk_from_section(struct world* w, struct world_section* s,
									   w_coord_t y) {
	assert(w);
	return (y >= 0 && y < WORLD_HEIGHT && s) ? s->column[y / CHUNK_SIZE] : NULL;
}

struct chunk* world_find_chunk(struct world* w, w_coord_t x, w_coord_t y,
							   w_coord_t z) {
	assert(w);

	if(y < 0 || y >= WORLD_HEIGHT)
		return NULL;

	int cx = WCOORD_CHUNK_OFFSET(x);
	int cy = y / CHUNK_SIZE;
	int cz = WCOORD_CHUNK_OFFSET(z);

	if(w->world_chunk_cache && cx == w->world_chunk_cache->x / CHUNK_SIZE
	   && cy == w->world_chunk_cache->y / CHUNK_SIZE
	   && cz == w->world_chunk_cache->z / CHUNK_SIZE)
		return w->world_chunk_cache;

	struct world_section* res
		= dict_wsection_get(w->sections, SECTION_TO_ID(cx, cz));

	if(res)
		w->world_chunk_cache = res->column[cy];

	return res ? res->column[cy] : NULL;
}

void world_preload(struct world* w,
				   void (*progress)(struct world* w, float percent)) {
	assert(w);

	dict_wsection_it_t it;
	dict_wsection_it(it, w->sections);

	size_t total = dict_wsection_size(w->sections);
	size_t count = 0;

	while(!dict_wsection_end_p(it)) {
		struct world_section* s = &dict_wsection_ref(it)->value;
		for(size_t k = 0; k < COLUMN_HEIGHT; k++) {
			if(s->column[k])
				chunk_check_built(s->column[k]);
		}

		if(progress)
			progress(w, (float)count / (float)total);
		count++;
		dict_wsection_next(it);
	}
}

bool world_block_intersection(struct world* w, struct ray* r, w_coord_t x,
							  w_coord_t y, w_coord_t z, enum side* s) {
	assert(w && r && s);

	struct block_data blk = world_get_block(w, x, y, z);

	if(blocks[blk.type]) {
		struct AABB bbox;

		if(blocks[blk.type]->getBoundingBox(
			   &(struct block_info) {.block = &blk, .x = x, .y = y, .z = z},
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
		struct chunk* c = ilist_chunks_ref(it);
		bool has_fog = !FOG_DIST_LESS(c, FOG_DIST_NO_EFFECT);

		chunk_pre_render(c, view, has_fog);

		if(has_fog) {
			ilist_chunks_remove(w->render, it);
			ilist_chunks_push_front(w->render, c);
		} else {
			ilist_chunks_next(it);
		}
	}
}

void world_pre_render_clear(struct world* w) {
	assert(w);
	ilist_chunks_init(w->render);
}

size_t world_build_chunks(struct world* w, size_t tokens) {
	ilist_chunks_it_t it;
	ilist_chunks_it(it, w->render);

	while(tokens > 0 && !ilist_chunks_end_p(it)) {
		if(chunk_check_built(ilist_chunks_ref(it)))
			tokens--;
		ilist_chunks_next(it);
	}

	dict_wsection_it_t it2;
	dict_wsection_it(it2, w->sections);

	while(tokens > 0 && !dict_wsection_end_p(it2)) {
		struct world_section* s = &dict_wsection_ref(it2)->value;
		for(size_t k = 0; k < COLUMN_HEIGHT; k++) {
			if(s->column[k] && chunk_check_built(s->column[k]))
				tokens--;
		}

		dict_wsection_next(it2);
	}

	return tokens;
}

void world_render_completed(struct world* w, bool new_render) {
	assert(w);

	ilist_chunks2_it_t it;
	ilist_chunks2_it(it, w->gpu_busy_chunks);

	while(!ilist_chunks2_end_p(it)) {
		chunk_unref(ilist_chunks2_ref(it));
		ilist_chunks2_next(it);
	}

	ilist_chunks2_reset(w->gpu_busy_chunks);

	if(new_render) {
		ilist_chunks_it_t it2;
		ilist_chunks_it(it2, w->render);

		while(!ilist_chunks_end_p(it2)) {
			// move imaginary reference token from "render" to "gpu_busy_chunks"
			ilist_chunks2_push_back(w->gpu_busy_chunks, ilist_chunks_ref(it2));
			ilist_chunks_next(it2);
		}
	}
}

size_t world_render(struct world* w, struct camera* c, bool pass) {
	assert(w && c);

	size_t in_view = 0;
	ilist_chunks_it_t it;

	gfx_fog(true);
	gfx_lighting(true);

	if(!pass) {
		gfx_bind_texture(TEXTURE_TERRAIN);
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
		gfx_write_buffers(false, true, true);

		mat4 matrix_anim;
		int anim = (time_diff_ms(w->anim_timer, time_get()) * 7 / 800) % 28;
		glm_translate_make(
			matrix_anim,
			(vec3) {(anim / 14) * 0.4921875F, (anim % 14) * 0.0703125F, 0.0F});

		gfx_matrix_texture(true, matrix_anim);
		gfx_bind_texture(TEXTURE_ANIM);

		for(int t_pass = 0; t_pass < 2; t_pass++) {
			if(t_pass == 1)
				gfx_write_buffers(true, false, true);

			ilist_chunks_it(it, w->render);
			while(!ilist_chunks_end_p(it)) {
				chunk_render(ilist_chunks_ref(it), true, c->x, c->y, c->z);
				ilist_chunks_next(it);
			}
		}

		gfx_matrix_texture(false, NULL);
		gfx_write_buffers(true, true, true);
	}

	return in_view;
}

bool world_aabb_intersection(struct world* w, struct AABB* a) {
	assert(w && a);

	w_coord_t min_x = floorf(a->x1) - 1;
	w_coord_t min_y = floorf(a->y1) - 1;
	w_coord_t min_z = floorf(a->z1) - 1;

	w_coord_t max_x = ceilf(a->x2) + 1;
	w_coord_t max_y = ceilf(a->y2) + 1;
	w_coord_t max_z = ceilf(a->z2) + 1;

	for(w_coord_t x = min_x; x < max_x; x++) {
		for(w_coord_t z = min_z; z < max_z; z++) {
			for(w_coord_t y = min_y; y < max_y; y++) {
				struct block_data blk = world_get_block(w, x, y, z);

				if(blocks[blk.type]) {
					struct AABB b;
					if(blocks[blk.type]->getBoundingBox(
						   &(struct block_info) {.block = &blk,
												 .neighbours = NULL,
												 .x = x,
												 .y = y,
												 .z = z},
						   true, &b)) {
						aabb_translate(&b, x, y, z);
						if(aabb_intersection(a, &b))
							return true;
					}
				}
			}
		}
	}

	return false;
}

static const float light_lookup_overworld[16] = {
	0.05F,	0.067F, 0.085F, 0.106F, 0.129F, 0.156F, 0.186F, 0.221F,
	0.261F, 0.309F, 0.367F, 0.437F, 0.525F, 0.638F, 0.789F, 1.0F,
};

static const float light_lookup_nether[16] = {
	0.1F, 0.116F, 0.133F, 0.153F, 0.175F, 0.2F,	  0.229F, 0.262F,
	0.3F, 0.345F, 0.4F,	  0.467F, 0.55F,  0.657F, 0.8F,	  1.0F,
};

const float* world_dimension_light(struct world* w) {
	assert(w);

	return (w->dimension == WORLD_DIM_OVERWORLD) ? light_lookup_overworld :
												   light_lookup_nether;
}
