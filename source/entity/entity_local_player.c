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

#include "../block/blocks_data.h"
#include "../platform/input.h"
#include "entity.h"

static void liquid_aabb(struct AABB* out, struct block_info* blk_info) {
	int block_height = (blk_info->block->metadata & 0x8) ?
		16 :
		(8 - blk_info->block->metadata) * 2 * 7 / 8;
	aabb_setsize(out, 1.0F, (float)block_height / 16.0F, 1.0F);
	aabb_translate(out, blk_info->x, blk_info->y, blk_info->z);
}

static bool test_in_lava(struct AABB* entity, struct block_info* blk_info) {
	if(blk_info->block->type != BLOCK_LAVA_FLOW
	   && blk_info->block->type != BLOCK_LAVA_STILL)
		return false;

	struct AABB bbox;
	liquid_aabb(&bbox, blk_info);
	return aabb_intersection(entity, &bbox);
}

static bool test_in_water(struct AABB* entity, struct block_info* blk_info) {
	if(blk_info->block->type != BLOCK_WATER_FLOW
	   && blk_info->block->type != BLOCK_WATER_STILL)
		return false;

	struct AABB bbox;
	liquid_aabb(&bbox, blk_info);
	return aabb_intersection(entity, &bbox);
}

static bool test_in_liquid(struct AABB* entity, struct block_info* blk_info) {
	return test_in_water(entity, blk_info) || test_in_lava(entity, blk_info);
}

static bool entity_tick(struct entity* e) {
	assert(e);

	glm_vec3_copy(e->pos, e->pos_old);
	glm_vec2_copy(e->orient, e->orient_old);

	for(int k = 0; k < 3; k++)
		if(fabsf(e->vel[k]) < 0.005F)
			e->vel[k] = 0.0F;

	struct AABB bbox;
	aabb_setsize_centered(&bbox, 0.6F, 1.0F, 0.6F);
	aabb_translate(&bbox, e->pos[0], e->pos[1] + 1.8F / 2.0F - 1.62F,
				   e->pos[2]);

	bool in_water = entity_intersection(e, &bbox, test_in_water);
	bool in_lava = entity_intersection(e, &bbox, test_in_lava);

	float slipperiness
		= (in_lava || in_water) ? 1.0F : (e->on_ground ? 0.6F : 1.0F);

	int forward = 0;
	int strafe = 0;
	bool jumping = false;

	if(e->data.local_player.capture_input) {
		if(input_held(IB_FORWARD))
			forward++;

		if(input_held(IB_BACKWARD))
			forward--;

		if(input_held(IB_RIGHT))
			strafe++;

		if(input_held(IB_LEFT))
			strafe--;

		jumping = input_held(IB_JUMP);
	}

	int dist = forward * forward + strafe * strafe;

	if(dist > 0) {
		float distf = fmaxf(sqrtf(dist), 1.0F);
		float dx = (forward * sinf(e->orient[0]) - strafe * cosf(e->orient[0]))
			/ distf;
		float dy = (strafe * sinf(e->orient[0]) + forward * cosf(e->orient[0]))
			/ distf;

		e->vel[0] += 0.1F * powf(0.6F / slipperiness, 3.0F) * dx;
		e->vel[2] += 0.1F * powf(0.6F / slipperiness, 3.0F) * dy;
	}

	if(e->data.local_player.jump_ticks > 0)
		e->data.local_player.jump_ticks--;

	if(jumping) {
		if(in_water || in_lava) {
			e->vel[1] += 0.04F;
		} else if(e->on_ground && e->data.local_player.jump_ticks == 0) {
			e->vel[1] = 0.42F;
			e->data.local_player.jump_ticks = 10;
		}
	} else {
		e->data.local_player.jump_ticks = 0;
	}

	float eye_height = 1.62F;

	aabb_setsize_centered(&bbox, 0.6F, 1.8F, 0.6F);
	aabb_translate(&bbox, 0.0F, 1.8F / 2.0F - eye_height, 0.0F);

	vec3 new_pos, new_vel;
	glm_vec3_copy(e->pos, new_pos);
	glm_vec3_copy(e->vel, new_vel);

	bool collision_xz = false;

	for(int k = 0; k < 3; k++)
		entity_try_move(e, e->pos, e->vel, &bbox, (size_t[]) {1, 0, 2}[k],
						&collision_xz, &e->on_ground);

	if(e->on_ground) {
		bool collision = false;
		bool ground = e->on_ground;

		new_vel[1] = 0.6F;
		entity_try_move(e, new_pos, new_vel, &bbox, 1, &collision, &ground);

		new_vel[1] = 0.0F;
		entity_try_move(e, new_pos, new_vel, &bbox, 0, &collision, &ground);
		entity_try_move(e, new_pos, new_vel, &bbox, 2, &collision, &ground);

		new_vel[1] = -0.6F;
		entity_try_move(e, new_pos, new_vel, &bbox, 1, &collision, &ground);

		if(glm_vec3_distance2(e->pos_old, e->pos)
		   < glm_vec3_distance2(e->pos_old, new_pos)) {
			collision_xz = collision;
			e->on_ground = ground;
			glm_vec3_copy(new_pos, e->pos);
			glm_vec3_copy(new_vel, e->vel);
		}
	}

	if(in_lava) {
		e->vel[0] *= 0.5F;
		e->vel[2] *= 0.5F;
		e->vel[1] = e->vel[1] * 0.5F - 0.02F;
	} else if(in_water) {
		e->vel[0] *= 0.8F;
		e->vel[2] *= 0.8F;
		e->vel[1] = e->vel[1] * 0.8F - 0.02F;
	} else {
		e->vel[0] *= slipperiness * 0.91F;
		e->vel[2] *= slipperiness * 0.91F;
		e->vel[1] -= 0.08F;

		struct block_data blk;
		if(entity_get_block(e, floorf(e->pos[0]),
							floorf(e->pos[1] - eye_height), floorf(e->pos[2]),
							&blk)
		   && blk.type == BLOCK_LADDER) {
			if(collision_xz)
				e->vel[1] = 0.12F;

			e->vel[0] = fmaxf(fminf(e->vel[0], 0.15F), -0.15F);
			e->vel[1] = fmaxf(e->vel[1], -0.15F);
			e->vel[2] = fmaxf(fminf(e->vel[2], 0.15F), -0.15F);
		}

		e->vel[1] *= 0.98F;
	}

	if(collision_xz && (in_lava || in_water)) {
		struct AABB tmp;
		aabb_setsize_centered(&tmp, 0.6F, 1.8F, 0.6F);
		aabb_translate(&tmp, e->pos[0] + e->vel[0],
					   e->pos[1] + e->vel[1] + 1.8F / 2.0F - 1.62F + 0.6F,
					   e->pos[2] + e->vel[2]);

		if(!entity_intersection(e, &tmp, test_in_liquid))
			e->vel[1] = 0.3F;
	}

	return false;
}

void entity_local_player(uint32_t id, struct entity* e, struct world* w) {
	assert(e && w);

	e->id = id;
	e->tick_server = NULL;
	e->tick_client = entity_tick;
	e->render = NULL;
	e->teleport = entity_default_teleport;
	e->type = ENTITY_LOCAL_PLAYER;
	e->data.local_player.capture_input = false;

	entity_default_init(e, false, w);
	e->data.local_player.jump_ticks = 0;
}
