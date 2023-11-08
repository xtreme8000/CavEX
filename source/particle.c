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

#include <assert.h>

#include "game/game_state.h"
#include "graphics/render_block.h"
#include "particle.h"
#include "platform/gfx.h"

#define PARTICLES_AREA 8
#define PARTICLES_VOLUME 64

ARRAY_DEF(array_particle, struct particle, M_POD_OPLIST)

array_particle_t particles;

void particle_init() {
	array_particle_init(particles);
}

static void particle_add(vec3 pos, vec3 vel, uint8_t tex) {
	assert(pos && vel);

	struct particle* p = array_particle_push_new(particles);

	if(p) {
		glm_vec3_copy(pos, p->pos);
		glm_vec3_copy(pos, p->pos_old);
		glm_vec3_copy(vel, p->vel);
		p->tex_uv[0]
			= (TEX_OFFSET(TEXTURE_X(tex)) + rand_flt() * 12.0F) / 256.0F;
		p->tex_uv[1]
			= (TEX_OFFSET(TEXTURE_Y(tex)) + rand_flt() * 12.0F) / 256.0F;
		p->age = 4.0F / (rand_flt() * 0.9F + 0.1F);
		p->size = (rand_flt() + 1.0F) * 0.03125F;
	}
}

void particle_generate_block(struct block_info* info) {
	assert(info && info->block && info->neighbours);

	if(!blocks[info->block->type])
		return;

	size_t count = blocks[info->block->type]->getBoundingBox(info, false, NULL);

	if(!count)
		return;

	struct AABB aabb[count];
	blocks[info->block->type]->getBoundingBox(info, false, aabb);

	// use only first AABB

	float volume
		= (aabb->x2 - aabb->x1) * (aabb->y2 - aabb->y1) * (aabb->z2 - aabb->z1);

	uint8_t tex = blocks[info->block->type]->getTextureIndex(info, SIDE_FRONT);

	for(int k = 0; k < volume * PARTICLES_VOLUME; k++) {
		float x = rand_flt() * (aabb->x2 - aabb->x1) + aabb->x1;
		float y = rand_flt() * (aabb->y2 - aabb->y1) + aabb->y1;
		float z = rand_flt() * (aabb->z2 - aabb->z1) + aabb->z1;

		vec3 vel = {rand_flt() - 0.5F, rand_flt() - 0.5F, rand_flt() - 0.5F};
		glm_vec3_normalize(vel);
		glm_vec3_scale(vel, (2.0F * rand_flt() + 0.5F) * 0.05F, vel);

		particle_add((vec3) {info->x + x, info->y + y, info->z + z}, vel, tex);
	}
}

void particle_generate_side(struct block_info* info, enum side s) {
	assert(info && info->block && info->neighbours);

	if(!blocks[info->block->type])
		return;

	size_t count = blocks[info->block->type]->getBoundingBox(info, false, NULL);

	if(!count)
		return;

	struct AABB aabb[count];
	blocks[info->block->type]->getBoundingBox(info, false, aabb);

	// use only first AABB

	float area;
	switch(s) {
		case SIDE_RIGHT:
		case SIDE_LEFT:
			area = (aabb->y2 - aabb->y1) * (aabb->z2 - aabb->z1);
			break;
		case SIDE_BOTTOM:
		case SIDE_TOP:
			area = (aabb->x2 - aabb->x1) * (aabb->z2 - aabb->z1);
			break;
		case SIDE_FRONT:
		case SIDE_BACK:
			area = (aabb->x2 - aabb->x1) * (aabb->y2 - aabb->y1);
			break;
		default: return;
	}

	uint8_t tex = blocks[info->block->type]->getTextureIndex(info, s);
	float offset = 0.0625F;

	for(int k = 0; k < area * PARTICLES_AREA; k++) {
		float x = rand_flt() * (aabb->x2 - aabb->x1) + aabb->x1;
		float y = rand_flt() * (aabb->y2 - aabb->y1) + aabb->y1;
		float z = rand_flt() * (aabb->z2 - aabb->z1) + aabb->z1;

		switch(s) {
			case SIDE_LEFT: x = aabb->x1 - offset; break;
			case SIDE_RIGHT: x = aabb->x2 + offset; break;
			case SIDE_BOTTOM: y = aabb->y1 - offset; break;
			case SIDE_TOP: y = aabb->y2 + offset; break;
			case SIDE_FRONT: z = aabb->z1 - offset; break;
			case SIDE_BACK: z = aabb->z2 + offset; break;
			default: return;
		}

		vec3 vel = {rand_flt() - 0.5F, rand_flt() - 0.5F, rand_flt() - 0.5F};
		glm_vec3_normalize(vel);
		glm_vec3_scale(vel, (2.0F * rand_flt() + 0.5F) * 0.05F, vel);

		particle_add((vec3) {info->x + x, info->y + y, info->z + z}, vel, tex);
	}
}

static void render_single(struct particle* p, vec3 camera, float delta) {
	assert(p && camera);

	vec3 pos_lerp;
	glm_vec3_lerp(p->pos_old, p->pos, delta, pos_lerp);

	vec3 v, s, t;
	glm_vec3_sub(pos_lerp, camera, v);
	glm_vec3_crossn(v, (vec3) {0.0F, 1.0F, 0.0F}, s);
	glm_vec3_crossn(v, s, t);

	glm_vec3_scale(s, p->size, s);
	glm_vec3_scale(t, p->size, t);

	struct block_data in_block
		= world_get_block(&gstate.world, floorf(pos_lerp[0]),
						  floorf(pos_lerp[1]), floorf(pos_lerp[2]));
	uint8_t light = roundf(
		gfx_lookup_light((in_block.torch_light << 4) | in_block.sky_light)
		* 255.0F * 0.8F);

	gfx_draw_quads_flt(
		4,
		(float[]) {-s[0] - t[0] + pos_lerp[0], -s[1] - t[1] + pos_lerp[1],
				   -s[2] - t[2] + pos_lerp[2], s[0] - t[0] + pos_lerp[0],
				   s[1] - t[1] + pos_lerp[1], s[2] - t[2] + pos_lerp[2],
				   s[0] + t[0] + pos_lerp[0], s[1] + t[1] + pos_lerp[1],
				   s[2] + t[2] + pos_lerp[2], -s[0] + t[0] + pos_lerp[0],
				   -s[1] + t[1] + pos_lerp[1], -s[2] + t[2] + pos_lerp[2]},
		(uint8_t[]) {light, light, light, 255, light, light, light, 255, light,
					 light, light, 255, light, light, light, 255},
		(float[]) {p->tex_uv[0], p->tex_uv[1], p->tex_uv[0] + 4.0F / 256.0F,
				   p->tex_uv[1], p->tex_uv[0] + 4.0F / 256.0F,
				   p->tex_uv[1] + 4.0F / 256.0F, p->tex_uv[0],
				   p->tex_uv[1] + 4.0F / 256.0F});
}

void particle_update() {
	array_particle_it_t it;
	array_particle_it(it, particles);

	while(!array_particle_end_p(it)) {
		struct particle* p = array_particle_ref(it);

		glm_vec3_copy(p->pos, p->pos_old);

		vec3 new_pos;
		glm_vec3_add(p->pos, p->vel, new_pos);

		w_coord_t bx = floorf(new_pos[0]);
		w_coord_t by = floorf(new_pos[1]);
		w_coord_t bz = floorf(new_pos[2]);
		struct block_data in_block = world_get_block(&gstate.world, bx, by, bz);

		bool intersect = false;
		if(blocks[in_block.type]) {
			struct block_info blk = (struct block_info) {
				.block = &in_block,
				.neighbours = NULL,
				.x = bx,
				.y = by,
				.z = bz,
			};

			size_t count
				= blocks[in_block.type]->getBoundingBox(&blk, true, NULL);
			if(count > 0) {
				struct AABB aabb[count];
				blocks[in_block.type]->getBoundingBox(&blk, true, aabb);

				for(size_t k = 0; k < count; k++) {
					aabb_translate(aabb + k, bx, by, bz);
					intersect = aabb_intersection_point(aabb + k, new_pos[0],
														new_pos[1], new_pos[2]);
					if(intersect)
						break;
				}
			}
		}

		if(!intersect) {
			glm_vec3_copy(new_pos, p->pos);
		} else {
			glm_vec3_zero(p->vel);
		}

		p->vel[1] -= 0.04F;
		glm_vec3_scale(p->vel, 0.98F, p->vel);
		p->age--;

		if(p->age > 0) {
			array_particle_next(it);
		} else {
			array_particle_remove(particles, it);
		}
	}
}

void particle_render(mat4 view, vec3 camera, float delta) {
	assert(view && camera);

	gfx_matrix_modelview(view);
	gfx_bind_texture(&texture_terrain);
	gfx_lighting(false);

	array_particle_it_t it;
	array_particle_it(it, particles);

	while(!array_particle_end_p(it)) {
		render_single(array_particle_ref(it), camera, delta);
		array_particle_next(it);
	}

	gfx_lighting(true);
}
