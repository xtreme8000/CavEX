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
#include <math.h>
#include <stdlib.h>

#include "../cglm/cglm.h"

#include "../item/window_container.h"
#include "../platform/thread.h"
#include "client_interface.h"
#include "inventory_logic.h"
#include "server_interface.h"
#include "server_local.h"

#define CHUNK_DIST2(x1, x2, z1, z2)                                            \
	(((x1) - (x2)) * ((x1) - (x2)) + ((z1) - (z2)) * ((z1) - (z2)))

struct entity* server_local_spawn_item(vec3 pos, struct item_data* it,
									   bool throw, struct server_local* s) {
	uint32_t entity_id = entity_gen_id(s->entities);
	struct entity** e_ptr = dict_entity_safe_get(s->entities, entity_id);
	*e_ptr = malloc(sizeof(struct entity));
	struct entity* e = *e_ptr;
	assert(e);

	entity_item(entity_id, e, true, &s->world, *it);
	e->teleport(e, pos);

	if(throw) {
		float rx = glm_rad(-s->player.rx
						   + (rand_gen_flt(&s->rand_src) - 0.5F) * 22.5F);
		float ry = glm_rad(s->player.ry + 90.0F
						   + (rand_gen_flt(&s->rand_src) - 0.5F) * 22.5F);
		e->vel[0] = sinf(rx) * sinf(ry) * 0.25F;
		e->vel[1] = cosf(ry) * 0.25F;
		e->vel[2] = cosf(rx) * sinf(ry) * 0.25F;
	} else {
		glm_vec3_copy((vec3) {rand_gen_flt(&s->rand_src) - 0.5F,
							  rand_gen_flt(&s->rand_src) - 0.5F,
							  rand_gen_flt(&s->rand_src) - 0.5F},
					  e->vel);
		glm_vec3_normalize(e->vel);
		glm_vec3_scale(
			e->vel, (2.0F * rand_gen_flt(&s->rand_src) + 0.5F) * 0.1F, e->vel);
	}

	clin_rpc_send(&(struct client_rpc) {
		.type = CRPC_SPAWN_ITEM,
		.payload.spawn_item.entity_id = e->id,
		.payload.spawn_item.item = e->data.item.item,
		.payload.spawn_item.pos = {e->pos[0], e->pos[1], e->pos[2]},
	});

	return e;
}

void server_local_spawn_block_drops(struct server_local* s,
									struct block_info* blk_info) {
	assert(s && blk_info);

	if(!blocks[blk_info->block->type])
		return;

	struct random_gen tmp = s->rand_src;
	size_t count
		= blocks[blk_info->block->type]->getDroppedItem(blk_info, NULL, &tmp);

	if(count > 0) {
		struct item_data items[count];
		blocks[blk_info->block->type]->getDroppedItem(blk_info, items,
													  &s->rand_src);

		for(size_t k = 0; k < count; k++)
			server_local_spawn_item((vec3) {blk_info->x + 0.5F,
											blk_info->y + 0.5F,
											blk_info->z + 0.5F},
									items + k, false, s);
	}
}

void server_local_send_inv_changes(set_inv_slot_t changes,
								   struct inventory* inv, uint8_t window) {
	assert(changes && inv);

	set_inv_slot_it_t it;
	set_inv_slot_it(it, changes);

	while(!set_inv_slot_end_p(it)) {
		size_t slot = *set_inv_slot_ref(it);

		clin_rpc_send(&(struct client_rpc) {
			.type = CRPC_INVENTORY_SLOT,
			.payload.inventory_slot.window = window,
			.payload.inventory_slot.slot = slot,
			.payload.inventory_slot.item = (slot == SPECIAL_SLOT_PICKED_ITEM) ?
				inv->picked_item :
				inv->items[slot],
		});

		set_inv_slot_next(it);
	}
}

static void server_local_process(struct server_rpc* call, void* user) {
	assert(call && user);

	struct server_local* s = user;

	switch(call->type) {
		case SRPC_PLAYER_POS:
			if(s->player.finished_loading) {
				s->player.x = call->payload.player_pos.x;
				s->player.y = call->payload.player_pos.y;
				s->player.z = call->payload.player_pos.z;
				s->player.rx = call->payload.player_pos.rx;
				s->player.ry = call->payload.player_pos.ry;
				s->player.has_pos = true;
			}
			break;
		case SRPC_HOTBAR_SLOT:
			if(s->player.has_pos
			   && call->payload.hotbar_slot.slot < INVENTORY_SIZE_HOTBAR)
				inventory_set_hotbar(&s->player.inventory,
									 call->payload.hotbar_slot.slot);
			break;
		case SRPC_WINDOW_CLICK: {
			set_inv_slot_t changes;
			set_inv_slot_init(changes);

			bool accept = inventory_action(
				s->player.active_inventory, call->payload.window_click.slot,
				call->payload.window_click.right_click, changes);

			clin_rpc_send(&(struct client_rpc) {
				.type = CRPC_WINDOW_TRANSACTION,
				.payload.window_transaction.accepted = accept,
				.payload.window_transaction.action_id
				= call->payload.window_click.action_id,
				.payload.window_transaction.window
				= call->payload.window_click.window,
			});

			server_local_send_inv_changes(changes, s->player.active_inventory,
										  call->payload.window_click.window);
			set_inv_slot_clear(changes);
			break;
		}
		case SRPC_WINDOW_CLOSE: {
			if(s->player.active_inventory->logic
			   && s->player.active_inventory->logic->on_close)
				s->player.active_inventory->logic->on_close(
					s->player.active_inventory);

			s->player.active_inventory = &s->player.inventory;
			break;
		}
		case SRPC_BLOCK_DIG:
			if(s->player.has_pos && call->payload.block_dig.y >= 0
			   && call->payload.block_dig.y < WORLD_HEIGHT
			   && call->payload.block_dig.finished) {
				struct block_data blk;
				if(server_world_get_block(&s->world, call->payload.block_dig.x,
										  call->payload.block_dig.y,
										  call->payload.block_dig.z, &blk)) {
					server_world_set_block(&s->world, call->payload.block_dig.x,
										   call->payload.block_dig.y,
										   call->payload.block_dig.z,
										   (struct block_data) {
											   .type = BLOCK_AIR,
											   .metadata = 0,
										   });

					struct item_data it_data;
					bool has_tool = inventory_get_hotbar_item(
						&s->player.inventory, &it_data);
					struct item* it = has_tool ? item_get(&it_data) : NULL;

					if(blocks[blk.type]
					   && ((it
							&& it->tool.type == blocks[blk.type]->digging.tool
							&& it->tool.tier >= blocks[blk.type]->digging.min)
						   || blocks[blk.type]->digging.min == TOOL_TIER_ANY
						   || blocks[blk.type]->digging.tool == TOOL_TYPE_ANY))
						server_local_spawn_block_drops(
							s,
							&(struct block_info) {
								.block = &blk,
								.neighbours = NULL,
								.x = call->payload.block_dig.x,
								.y = call->payload.block_dig.y,
								.z = call->payload.block_dig.z,
							});
				}
			}
			break;
		case SRPC_BLOCK_PLACE:
			if(s->player.has_pos && call->payload.block_place.y >= 0
			   && call->payload.block_place.y < WORLD_HEIGHT) {
				int x, y, z;
				blocks_side_offset(call->payload.block_place.side, &x, &y, &z);

				struct block_data blk_where, blk_on;
				if(server_world_get_block(
					   &s->world, call->payload.block_place.x + x,
					   call->payload.block_place.y + y,
					   call->payload.block_place.z + z, &blk_where)
				   && server_world_get_block(
					   &s->world, call->payload.block_place.x,
					   call->payload.block_place.y, call->payload.block_place.z,
					   &blk_on)) {
					struct block_info where = (struct block_info) {
						.block = &blk_where,
						.neighbours = NULL,
						.x = call->payload.block_place.x + x,
						.y = call->payload.block_place.y + y,
						.z = call->payload.block_place.z + z,
					};

					struct block_info on = (struct block_info) {
						.block = &blk_on,
						.neighbours = NULL,
						.x = call->payload.block_place.x,
						.y = call->payload.block_place.y,
						.z = call->payload.block_place.z,
					};

					struct item_data it_data;
					inventory_get_hotbar_item(&s->player.inventory, &it_data);
					struct item* it = item_get(&it_data);

					if(blocks[blk_on.type]
					   && blocks[blk_on.type]->onRightClick) {
						blocks[blk_on.type]->onRightClick(
							s, &it_data, &where, &on,
							call->payload.block_place.side);
					} else if((!blocks[blk_where.type]
							   || blocks[blk_where.type]->place_ignore)
							  && it && it->onItemPlace
							  && it->onItemPlace(
								  s, &it_data, &where, &on,
								  call->payload.block_place.side)) {
						size_t slot
							= inventory_get_hotbar(&s->player.inventory);
						inventory_consume(&s->player.inventory,
										  slot + INVENTORY_SLOT_HOTBAR);

						clin_rpc_send(&(struct client_rpc) {
							.type = CRPC_INVENTORY_SLOT,
							.payload.inventory_slot.window = WINDOWC_INVENTORY,
							.payload.inventory_slot.slot
							= slot + INVENTORY_SLOT_HOTBAR,
							.payload.inventory_slot.item
							= s->player.inventory
								  .items[slot + INVENTORY_SLOT_HOTBAR],
						});
					}
				}
			}
			break;
		case SRPC_UNLOAD_WORLD:
			// save chunks here, then destroy all
			clin_rpc_send(&(struct client_rpc) {
				.type = CRPC_WORLD_RESET,
				.payload.world_reset.dimension = s->player.dimension,
				.payload.world_reset.local_entity = 0,
			});

			level_archive_write_player(
				&s->level, (vec3) {s->player.x, s->player.y, s->player.z},
				(vec2) {s->player.rx, s->player.ry}, NULL, s->player.dimension);

			level_archive_write_inventory(&s->level, &s->player.inventory);
			level_archive_write(&s->level, LEVEL_TIME, &s->world_time);

			dict_entity_it_t it;
			dict_entity_it(it, s->entities);

			while(!dict_entity_end_p(it)) {
				free(dict_entity_ref(it)->value);
				dict_entity_next(it);
			}
			dict_entity_reset(s->entities);
			server_world_destroy(&s->world);
			level_archive_destroy(&s->level);

			s->player.has_pos = false;
			s->player.finished_loading = false;
			string_reset(s->level_name);
			break;
		case SRPC_LOAD_WORLD:
			assert(!s->player.has_pos);

			string_set(s->level_name, call->payload.load_world.name);
			string_clear(call->payload.load_world.name);

			if(level_archive_create(&s->level, s->level_name)) {
				vec3 pos;
				vec2 rot;
				enum world_dim dim;
				if(level_archive_read_player(&s->level, pos, rot, NULL, &dim)) {
					server_world_create(&s->world, s->level_name, dim);
					s->player.x = pos[0];
					s->player.y = pos[1];
					s->player.z = pos[2];
					s->player.rx = rot[0];
					s->player.ry = rot[1];
					s->player.dimension = dim;
					s->player.has_pos = true;
				}

				level_archive_read(&s->level, LEVEL_TIME, &s->world_time, 0);
				dict_entity_reset(s->entities);
				s->player.active_inventory = &s->player.inventory;

				clin_rpc_send(&(struct client_rpc) {
					.type = CRPC_WORLD_RESET,
					.payload.world_reset.dimension = dim,
					.payload.world_reset.local_entity = 0,
				});
			}
			break;
	}
}

static void server_local_update(struct server_local* s) {
	assert(s);

	svin_process_messages(server_local_process, s, false);

	if(!s->player.has_pos)
		return;

	s->world_time++;

	dict_entity_it_t it;
	dict_entity_it(it, s->entities);

	while(!dict_entity_end_p(it)) {
		uint32_t key = dict_entity_ref(it)->key;
		struct entity* e = dict_entity_ref(it)->value;

		if(e->tick_server) {
			bool remove = (e->delay_destroy == 0) || e->tick_server(e, s);
			dict_entity_next(it);

			if(remove) {
				clin_rpc_send(&(struct client_rpc) {
					.type = CRPC_ENTITY_DESTROY,
					.payload.entity_destroy.entity_id = key,
				});

				free(e);
				dict_entity_erase(s->entities, key);
			} else if(e->delay_destroy < 0) {
				clin_rpc_send(&(struct client_rpc) {
					.type = CRPC_ENTITY_MOVE,
					.payload.entity_move.entity_id = key,
					.payload.entity_move.pos
					= {e->pos[0], e->pos[1], e->pos[2]},
				});
			}
		} else {
			dict_entity_next(it);
		}
	}

	w_coord_t px = WCOORD_CHUNK_OFFSET(floor(s->player.x));
	w_coord_t pz = WCOORD_CHUNK_OFFSET(floor(s->player.z));

	server_world_random_tick(&s->world, &s->rand_src, s, px, pz,
							 MAX_VIEW_DISTANCE - 2);

	w_coord_t cx, cz;
	if(server_world_furthest_chunk(&s->world, MAX_VIEW_DISTANCE, px, pz, &cx,
								   &cz)) {
		// unload just one chunk
		server_world_save_chunk(&s->world, true, cx, cz);
		clin_rpc_send(&(struct client_rpc) {
			.type = CRPC_UNLOAD_CHUNK,
			.payload.unload_chunk.x = cx,
			.payload.unload_chunk.z = cz,
		});
	}

	// iterate over all chunks that should be loaded
	bool c_nearest = false;
	w_coord_t c_nearest_x, c_nearest_z;
	w_coord_t c_nearest_dist2 = INT_MAX;
	for(w_coord_t z = pz - MAX_VIEW_DISTANCE; z <= pz + MAX_VIEW_DISTANCE;
		z++) {
		for(w_coord_t x = px - MAX_VIEW_DISTANCE; x <= px + MAX_VIEW_DISTANCE;
			x++) {
			w_coord_t d = CHUNK_DIST2(px, x, pz, z);
			if(!server_world_is_chunk_loaded(&s->world, x, z)
			   && (d < c_nearest_dist2 || !c_nearest)
			   && server_world_disk_has_chunk(&s->world, x, z)) {
				c_nearest_dist2 = d;
				c_nearest_x = x;
				c_nearest_z = z;
				c_nearest = true;
			}
		}
	}

	// load just one chunk
	struct server_chunk* sc;
	if(c_nearest
	   && server_world_load_chunk(&s->world, c_nearest_x, c_nearest_z, &sc)) {
		size_t sz = CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT;
		void* ids = malloc(sz);
		void* metadata = malloc(sz / 2);
		void* lighting_sky = malloc(sz / 2);
		void* lighting_torch = malloc(sz / 2);

		memcpy(ids, sc->ids, sz);
		memcpy(metadata, sc->metadata, sz / 2);
		memcpy(lighting_sky, sc->lighting_sky, sz / 2);
		memcpy(lighting_torch, sc->lighting_torch, sz / 2);

		clin_rpc_send(&(struct client_rpc) {
			.type = CRPC_CHUNK,
			.payload.chunk.x = c_nearest_x * CHUNK_SIZE,
			.payload.chunk.y = 0,
			.payload.chunk.z = c_nearest_z * CHUNK_SIZE,
			.payload.chunk.sx = CHUNK_SIZE,
			.payload.chunk.sy = WORLD_HEIGHT,
			.payload.chunk.sz = CHUNK_SIZE,
			.payload.chunk.ids = ids,
			.payload.chunk.metadata = metadata,
			.payload.chunk.lighting_sky = lighting_sky,
			.payload.chunk.lighting_torch = lighting_torch,
		});
	} else if(!s->player.finished_loading) {
		struct client_rpc pos;
		pos.type = CRPC_PLAYER_POS;
		if(level_archive_read_player(&s->level, pos.payload.player_pos.position,
									 pos.payload.player_pos.rotation, NULL,
									 NULL))
			clin_rpc_send(&pos);

		clin_rpc_send(&(struct client_rpc) {
			.type = CRPC_TIME_SET,
			.payload.time_set = s->world_time,
		});

		if(level_archive_read_inventory(&s->level, &s->player.inventory)) {
			for(size_t k = 0; k < INVENTORY_SIZE; k++) {
				if(s->player.inventory.items[k].id > 0) {
					clin_rpc_send(&(struct client_rpc) {
						.type = CRPC_INVENTORY_SLOT,
						.payload.inventory_slot.window = WINDOWC_INVENTORY,
						.payload.inventory_slot.slot = k,
						.payload.inventory_slot.item
						= s->player.inventory.items[k],
					});
				}
			}
		}

		s->player.finished_loading = true;
	}
}

static void* server_local_thread(void* user) {
	while(1) {
		server_local_update(user);
		thread_msleep(50);
	}

	return NULL;
}

void server_local_create(struct server_local* s) {
	assert(s);
	rand_gen_seed(&s->rand_src);
	s->world_time = 0;
	s->player.has_pos = false;
	s->player.finished_loading = false;
	string_init(s->level_name);

	inventory_create(&s->player.inventory, &inventory_logic_player, s,
					 INVENTORY_SIZE);
	s->player.active_inventory = &s->player.inventory;
	dict_entity_init(s->entities);

	struct thread t;
	thread_create(&t, server_local_thread, s, 8);
}
