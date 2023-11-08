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
#include "../game/game_state.h"
#include "../network/client_interface.h"
#include "../network/server_local.h"
#include "../platform/gfx.h"
#include "entity.h"

static bool entity_client_tick(struct entity* e) {
	entity_default_client_tick(e);
	e->data.item.age++;
	return false;
}

static bool entity_server_tick(struct entity* e, struct server_local* s) {
	assert(e);

	glm_vec3_copy(e->pos, e->pos_old);
	glm_vec2_copy(e->orient, e->orient_old);

	for(int k = 0; k < 3; k++)
		if(fabsf(e->vel[k]) < 0.005F)
			e->vel[k] = 0.0F;

	struct AABB bbox;
	aabb_setsize_centered(&bbox, 0.25F, 0.25F, 0.25F);

	struct AABB tmp = bbox;
	aabb_translate(&tmp, e->pos[0], e->pos[1], e->pos[2]);

	if(entity_aabb_intersection(e, &tmp)) { // is item stuck in block?
		// find possible new position, try top/bottom last
		enum side sides[6] = {SIDE_LEFT, SIDE_RIGHT, SIDE_FRONT,
							  SIDE_BACK, SIDE_TOP,	 SIDE_BOTTOM};
		for(int k = 0; k < 6; k++) {
			int x, y, z;
			blocks_side_offset(sides[k], &x, &y, &z);

			vec3 new_pos;
			glm_vec3_add(e->pos, (vec3) {x, y, z}, new_pos);

			struct AABB tmp2 = tmp;
			aabb_translate(&tmp2, x, y, z);

			if(!entity_aabb_intersection(e, &tmp2)) {
				float threshold;
				entity_intersection_threshold(e, &bbox, new_pos, e->pos,
											  &threshold);
				glm_vec3_lerp(new_pos, e->pos, threshold, e->pos);
				e->vel[0] = x * 0.1F;
				e->vel[1] = y * 0.1F;
				e->vel[2] = z * 0.1F;

				break;
			}
		}
	}

	bool collision_xz = false;

	for(int k = 0; k < 3; k++)
		entity_try_move(e, e->pos, e->vel, &bbox, (size_t[]) {1, 0, 2}[k],
						&collision_xz, &e->on_ground);

	e->vel[1] -= 0.04F;
	e->vel[0] *= (e->on_ground ? 0.6F : 1.0F) * 0.98F;
	e->vel[2] *= (e->on_ground ? 0.6F : 1.0F) * 0.98F;
	e->vel[1] *= 0.98F;

	e->data.item.age++;

	if(e->delay_destroy > 0) {
		e->delay_destroy--;
	} else if(e->data.item.age >= 2 * 20
			  && glm_vec3_distance2(
					 e->pos,
					 (vec3) {s->player.x, s->player.y - 0.6F, s->player.z})
				  < glm_pow2(2.0F)) { // allow pickup after 2s
		bool slots_changed[INVENTORY_SIZE];
		memset(slots_changed, false, sizeof(slots_changed));

		// TODO: case where item cannot be picked up completely
		inventory_collect_inventory(&s->player.inventory, &e->data.item.item,
									slots_changed);

		for(size_t k = 0; k < INVENTORY_SIZE; k++) {
			if(slots_changed[k])
				clin_rpc_send(&(struct client_rpc) {
					.type = CRPC_INVENTORY_SLOT,
					.payload.inventory_slot.window = WINDOWC_INVENTORY,
					.payload.inventory_slot.slot = k,
					.payload.inventory_slot.item = s->player.inventory.items[k],
				});
		}

		clin_rpc_send(&(struct client_rpc) {
			.type = CRPC_PICKUP_ITEM,
			.payload.pickup_item.entity_id = e->id,
			.payload.pickup_item.collector_id = 0, // local player
		});

		e->delay_destroy = 1;
	}

	return e->data.item.age >= 5 * 60 * 20; // destroy after 5 min
}

static void entity_render(struct entity* e, mat4 view, float tick_delta) {
	struct item* it = item_get(&e->data.item.item);

	if(it) {
		vec3 pos_lerp;
		glm_vec3_lerp(e->pos_old, e->pos, tick_delta, pos_lerp);

		struct block_data in_block;
		entity_get_block(e, floorf(pos_lerp[0]), floorf(pos_lerp[1]),
						 floorf(pos_lerp[2]), &in_block);
		render_item_update_light((in_block.torch_light << 4)
								 | in_block.sky_light);

		float ticks = e->data.item.age + tick_delta;

		mat4 model;
		glm_translate_make(model, pos_lerp);
		glm_translate_y(model, sinf(ticks / 30.0F * GLM_PIf) * 0.1F + 0.1F);
		glm_rotate_y(model, glm_rad(ticks * 3.0F), model);
		glm_scale_uni(model, 0.25F);
		glm_translate(model, (vec3) {-0.5F, -0.5F, -0.5F});

		mat4 mv;
		glm_mat4_mul(view, model, mv);

		int amount = 1;
		if(e->data.item.item.count > 1) {
			amount = 2;
		} else if(e->data.item.item.count > 5) {
			amount = 3;
		} else if(e->data.item.item.count > 20) {
			amount = 4;
		}

		vec3 displacement[4] = {
			{0.0F, 0.0F, 0.0F},
			{-0.701F, -0.331F, -0.239F},
			{0.139F, -0.276F, 0.211F},
			{0.443F, 0.512F, -0.101F},
		};

		for(int k = 0; k < amount; k++) {
			mat4 final;
			glm_translate_make(final, displacement[k]);
			glm_mat4_mul(mv, final, final);

			it->renderItem(it, &e->data.item.item, final, false,
						   R_ITEM_ENV_ENTITY);
		}

		struct AABB bbox;
		aabb_setsize_centered(&bbox, 0.25F, 0.25F, 0.25F);
		aabb_translate(&bbox, pos_lerp[0], pos_lerp[1] - 0.04F, pos_lerp[2]);
		entity_shadow(e, &bbox, view);
	}
}

void entity_item(uint32_t id, struct entity* e, bool server, void* world,
				 struct item_data it) {
	assert(e && world);

	e->id = id;
	e->tick_server = entity_server_tick;
	e->tick_client = entity_client_tick;
	e->render = entity_render;
	e->teleport = entity_default_teleport;
	e->type = ENTITY_ITEM;
	e->data.item.age = 0;
	e->data.item.item = it;

	entity_default_init(e, server, world);

	if(server) {
		glm_vec3_copy(
			(vec3) {rand_flt() - 0.5F, rand_flt() - 0.5F, rand_flt() - 0.5F},
			e->vel);
		glm_vec3_normalize(e->vel);
		glm_vec3_scale(e->vel, (2.0F * rand_flt() + 0.5F) * 0.1F, e->vel);
	}
}
