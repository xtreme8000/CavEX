/*
	Copyright (c) 2023 ByteBit/xtreme8000

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

#include "entity.h"
#include "../network/server_world.h"
#include "../platform/gfx.h"
#include "../world.h"

void entity_default_init(struct entity* e, bool server, void* world) {
	e->on_server = server;
	e->world = world;
	e->on_ground = true;
	e->delay_destroy = -1;

	glm_vec3_zero(e->pos);
	glm_vec3_zero(e->pos_old);
	glm_vec3_zero(e->network_pos);
	glm_vec3_zero(e->vel);
	glm_vec2_zero(e->orient);
	glm_vec2_zero(e->orient_old);
}

void entity_default_teleport(struct entity* e, vec3 pos) {
	e->on_ground = false;

	glm_vec3_copy(pos, e->pos);
	glm_vec3_copy(pos, e->pos_old);
	glm_vec3_copy(pos, e->network_pos);
}

bool entity_default_client_tick(struct entity* e) {
	assert(e);

	glm_vec3_copy(e->pos, e->pos_old);
	glm_vec3_copy(e->network_pos, e->pos);
	glm_vec2_copy(e->orient, e->orient_old);
	return false;
}

bool entity_get_block(struct entity* e, w_coord_t x, w_coord_t y, w_coord_t z,
					  struct block_data* blk) {
	assert(e && blk);

	if(e->on_server) {
		return server_world_get_block(e->world, x, y, z, blk);
	} else {
		*blk = world_get_block(e->world, x, y, z);
		return true;
	}
}

void entity_shadow(struct entity* e, struct AABB* a, mat4 view) {
	assert(e && a && view);

	w_coord_t min_x = floorf(a->x1);
	w_coord_t min_y = floorf(a->y1);
	w_coord_t min_z = floorf(a->z1);

	w_coord_t max_x = ceilf(a->x2) + 1;
	w_coord_t max_y = ceilf(a->y2) + 1;
	w_coord_t max_z = ceilf(a->z2) + 1;

	gfx_matrix_modelview(view);
	gfx_blending(MODE_BLEND);
	gfx_alpha_test(false);
	gfx_bind_texture(&texture_shadow);
	gfx_lighting(false);

	float offset = 0.01F;
	float du = 1.0F / (a->x2 - a->x1);
	float dv = 1.0F / (a->z2 - a->z1);

	for(w_coord_t x = min_x; x < max_x; x++) {
		for(w_coord_t z = min_z; z < max_z; z++) {
			for(w_coord_t y = min_y; y < max_y; y++) {
				struct block_data blk;

				if(entity_get_block(e, x, y, z, &blk) && blocks[blk.type]) {
					struct AABB b;
					if(blocks[blk.type]->getBoundingBox(
						   &(struct block_info) {.block = &blk,
												 .neighbours = NULL,
												 .x = x,
												 .y = y,
												 .z = z},
						   true, &b)) {
						aabb_translate(&b, x, y, z);
						if(a->y2 > b.y2 && aabb_intersection(a, &b)) {
							float u1 = (b.x1 - a->x1) * du;
							float u2 = (b.x2 - a->x1) * du;
							float v1 = (b.z1 - a->z1) * dv;
							float v2 = (b.z2 - a->z1) * dv;

							gfx_draw_quads_flt(
								4,
								(float[]) {b.x1, b.y2 + offset, b.z1, b.x2,
										   b.y2 + offset, b.z1, b.x2,
										   b.y2 + offset, b.z2, b.x1,
										   b.y2 + offset, b.z2},
								(uint8_t[]) {0xFF, 0xFF, 0xFF, 0x60, 0xFF, 0xFF,
											 0xFF, 0x60, 0xFF, 0xFF, 0xFF, 0x60,
											 0xFF, 0xFF, 0xFF, 0x60},
								(float[]) {u1, v1, u2, v1, u2, v2, u1, v2});
						}
					}
				}
			}
		}
	}

	gfx_blending(MODE_OFF);
	gfx_alpha_test(true);
	gfx_lighting(true);
}

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

bool entity_aabb_intersection(struct entity* e, struct AABB* a,
							  bool (*test)(struct block_data*, w_coord_t,
										   w_coord_t, w_coord_t)) {
	assert(e && a);

	w_coord_t min_x = floorf(a->x1);
	// need to look one further, otherwise fence block breaks
	w_coord_t min_y = max((w_coord_t)(floorf(a->y1) - 1), 0);
	w_coord_t min_z = floorf(a->z1);

	w_coord_t max_x = ceilf(a->x2) + 1;
	w_coord_t max_y = min((w_coord_t)(ceilf(a->y2) + 1), WORLD_HEIGHT - 1);
	w_coord_t max_z = ceilf(a->z2) + 1;

	for(w_coord_t x = min_x; x < max_x; x++) {
		for(w_coord_t z = min_z; z < max_z; z++) {
			for(w_coord_t y = min_y; y < max_y; y++) {
				struct block_data blk;

				if(entity_get_block(e, x, y, z, &blk) && blocks[blk.type]) {
					struct AABB b;
					if(blocks[blk.type]->getBoundingBox(
						   &(struct block_info) {.block = &blk,
												 .neighbours = NULL,
												 .x = x,
												 .y = y,
												 .z = z},
						   true, &b)
					   || (test && test(&blk, x, y, z))) {
						aabb_translate(&b, x, y, z);
						if(aabb_intersection(a, &b)
						   && (!test || test(&blk, x, y, z)))
							return true;
					}
				}
			}
		}
	}

	return false;
}

bool entity_intersection_threshold(struct entity* e, struct AABB* aabb,
								   vec3 old_pos, vec3 new_pos,
								   float* threshold) {
	assert(e && aabb && old_pos && new_pos && threshold);

	struct AABB tmp = *aabb;
	aabb_translate(&tmp, old_pos[0], old_pos[1], old_pos[2]);
	bool a = entity_aabb_intersection(e, &tmp, NULL);

	tmp = *aabb;
	aabb_translate(&tmp, new_pos[0], new_pos[1], new_pos[2]);
	bool b = entity_aabb_intersection(e, &tmp, NULL);

	if(!a && b) {
		float range_min = 0.0F;
		float range_max = 1.0F;

		while(1) {
			vec3 pos_min, pos_max;
			glm_vec3_lerp(old_pos, new_pos, range_min, pos_min);
			glm_vec3_lerp(old_pos, new_pos, range_max, pos_max);

			if(glm_vec3_distance2(pos_min, pos_max) < glm_pow2(0.01F))
				break;

			float mid = (range_max + range_min) / 2.0F;

			vec3 pos_mid;
			glm_vec3_lerp(old_pos, new_pos, mid, pos_mid);

			struct AABB dest = *aabb;
			aabb_translate(&dest, pos_mid[0], pos_mid[1], pos_mid[2]);

			if(entity_aabb_intersection(e, &dest, NULL)) {
				range_max = mid;
			} else {
				range_min = mid;
			}
		}

		*threshold = range_min;
		return true;
	} else if(a) {
		*threshold = 0.0F;
		return true;
	} else {
		*threshold = 1.0F;
		return false;
	}
}

void entity_try_move(struct entity* e, vec3 pos, vec3 vel, struct AABB* bbox,
					 size_t coord, bool* collision_xz, bool* on_ground) {
	assert(e && pos && vel && bbox && collision_xz && on_ground);

	vec3 tmp;
	glm_vec3_copy(pos, tmp);
	tmp[coord] += vel[coord];

	float threshold;
	if(entity_intersection_threshold(e, bbox, pos, tmp, &threshold)) {
		if(coord == 1 && vel[1] < 0.0F)
			*on_ground = true;

		if(coord == 0 || coord == 2)
			*collision_xz = true;

		vel[coord] = 0.0F;
	} else if(coord == 1) {
		*on_ground = false;
	}

	pos[coord] = pos[coord] * (1.0F - threshold) + tmp[coord] * threshold;
}

uint32_t entity_gen_id(dict_entity_t dict) {
	assert(dict);

	dict_entity_it_t it;
	dict_entity_it(it, dict);

	// id = 0 is reserved for local player

	uint32_t id = 0;

	while(!dict_entity_end_p(it)) {
		uint32_t key = dict_entity_ref(it)->key;

		if(key > id)
			id = key;

		dict_entity_next(it);
	}

	return id + 1;
}

void entities_client_tick(dict_entity_t dict) {
	dict_entity_it_t it;
	dict_entity_it(it, dict);

	while(!dict_entity_end_p(it)) {
		struct entity* e = &dict_entity_ref(it)->value;

		if(e->tick_client)
			e->tick_client(e);

		dict_entity_next(it);
	}
}

void entities_client_render(dict_entity_t dict, struct camera* c,
							float tick_delta) {
	dict_entity_it_t it;
	dict_entity_it(it, dict);

	while(!dict_entity_end_p(it)) {
		struct entity* e = &dict_entity_ref(it)->value;
		if(e->render
		   && glm_vec3_distance2(e->pos, (vec3) {c->x, c->y, c->z})
			   < glm_pow2(32.0F))
			e->render(e, c->view, tick_delta);
		dict_entity_next(it);
	}
}