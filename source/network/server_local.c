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
#include "server_interface.h"
#include "server_local.h"

#define CHUNK_DIST2(x1, x2, z1, z2)                                            \
	(((x1) - (x2)) * ((x1) - (x2)) + ((z1) - (z2)) * ((z1) - (z2)))

static struct entity* spawn_item(vec3 pos, struct item_data* it, bool throw,
								 struct server_local* s) {
	uint32_t entity_id = entity_gen_id(s->entities);
	struct entity* e = dict_entity_safe_get(s->entities, entity_id);
	entity_item(entity_id, e, true, &s->world, *it);
	e->teleport(e, pos);

	if(throw) {
		float rx = glm_rad(-s->player.rx);
		float ry = glm_rad(s->player.ry + 90.0F);
		e->vel[0] = sinf(rx) * sinf(ry) * 0.25F;
		e->vel[1] = cosf(ry) * 0.25F;
		e->vel[2] = cosf(rx) * sinf(ry) * 0.25F;
	}

	clin_rpc_send(&(struct client_rpc) {
		.type = CRPC_SPAWN_ITEM,
		.payload.spawn_item.entity_id = e->id,
		.payload.spawn_item.item = e->data.item.item,
		.payload.spawn_item.pos = {e->pos[0], e->pos[1], e->pos[2]},
	});

	return e;
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
			bool accept = false;
			if(call->payload.window_click.window == WINDOWC_INVENTORY) {
				if(!(inventory_get_picked_item(&s->player.inventory, NULL)
					 && call->payload.window_click.slot
						 == INVENTORY_SLOT_OUTPUT))
					accept = inventory_action(
						&s->player.inventory, call->payload.window_click.slot,
						call->payload.window_click.right_click);
			}

			clin_rpc_send(&(struct client_rpc) {
				.type = CRPC_WINDOW_TRANSACTION,
				.payload.window_transaction.accepted = accept,
				.payload.window_transaction.action_id
				= call->payload.window_click.action_id,
				.payload.window_transaction.window
				= call->payload.window_click.window,
			});
			break;
		}
		case SRPC_WINDOW_CLOSE:
			if(call->payload.window_click.window == WINDOWC_INVENTORY) {
				bool slots_changed[INVENTORY_SIZE];
				memset(slots_changed, false, sizeof(slots_changed));

				inventory_clear_slot(&s->player.inventory,
									 INVENTORY_SLOT_OUTPUT);
				slots_changed[INVENTORY_SLOT_OUTPUT] = true;

				for(size_t k = INVENTORY_SLOT_CRAFTING;
					k < INVENTORY_SLOT_CRAFTING + INVENTORY_SIZE_CRAFTING;
					k++) {
					struct item_data item;
					inventory_get_slot(&s->player.inventory, k, &item);

					if(item.id != 0) {
						inventory_clear_slot(&s->player.inventory, k);
						slots_changed[k] = true;
						spawn_item(
							(vec3) {s->player.x, s->player.y, s->player.z},
							&item, true, s);
					}
				}

				for(size_t k = 0; k < INVENTORY_SIZE; k++) {
					if(slots_changed[k])
						clin_rpc_send(&(struct client_rpc) {
							.type = CRPC_INVENTORY_SLOT,
							.payload.inventory_slot.window = WINDOWC_INVENTORY,
							.payload.inventory_slot.slot = k,
							.payload.inventory_slot.item
							= s->player.inventory.items[k],
						});
				}

				struct item_data picked_item;
				if(inventory_get_picked_item(&s->player.inventory,
											 &picked_item)) {
					inventory_clear_picked_item(&s->player.inventory);
					spawn_item((vec3) {s->player.x, s->player.y, s->player.z},
							   &picked_item, true, s);

					clin_rpc_send(&(struct client_rpc) {
						.type = CRPC_INVENTORY_SLOT,
						.payload.inventory_slot.window = WINDOWC_INVENTORY,
						.payload.inventory_slot.slot = NETWORK_SLOT_PICKED_ITEM,
						.payload.inventory_slot.item
						= s->player.inventory.picked_item,
					});
				}
			}
			break;
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

					spawn_item((vec3) {call->payload.block_dig.x + 0.5F,
									   call->payload.block_dig.y + 0.5F,
									   call->payload.block_dig.z + 0.5F},
							   &(struct item_data) {.id = blk.type,
													.durability = blk.metadata,
													.count = 1},
							   false, s);
				}
			}
			break;
		case SRPC_BLOCK_PLACE:
			if(s->player.has_pos && call->payload.block_place.y >= 0
			   && call->payload.block_place.y < WORLD_HEIGHT) {
				int x, y, z;
				blocks_side_offset(call->payload.block_place.side, &x, &y, &z);

				struct item_data it_data;
				inventory_get_hotbar_item(&s->player.inventory, &it_data);
				struct item* it = item_get(&it_data);

				if(it && it->onItemPlace) {
					struct block_data blk_where, blk_on;
					if(server_world_get_block(
						   &s->world, call->payload.block_place.x + x,
						   call->payload.block_place.y + y,
						   call->payload.block_place.z + z, &blk_where)
					   && server_world_get_block(
						   &s->world, call->payload.block_place.x,
						   call->payload.block_place.y,
						   call->payload.block_place.z, &blk_on)) {
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

						if((!blocks[blk_where.type]
							|| blocks[blk_where.type]->place_ignore)
						   && it->onItemPlace(s, &it_data, &where, &on,
											  call->payload.block_place.side)) {
							size_t slot
								= inventory_get_hotbar(&s->player.inventory);
							inventory_consume(&s->player.inventory,
											  slot + INVENTORY_SLOT_HOTBAR);

							clin_rpc_send(&(struct client_rpc) {
								.type = CRPC_INVENTORY_SLOT,
								.payload.inventory_slot.window
								= WINDOWC_INVENTORY,
								.payload.inventory_slot.slot
								= slot + INVENTORY_SLOT_HOTBAR,
								.payload.inventory_slot.item
								= s->player.inventory
									  .items[slot + INVENTORY_SLOT_HOTBAR],
							});
						}
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

				if(level_archive_read(&s->level, LEVEL_TIME, &s->world_time, 0))
					s->world_time_start = time_get();

				dict_entity_reset(s->entities);

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

	dict_entity_it_t it;
	dict_entity_it(it, s->entities);

	while(!dict_entity_end_p(it)) {
		uint32_t key = dict_entity_ref(it)->key;
		struct entity* e = &dict_entity_ref(it)->value;

		if(e->tick_server) {
			bool remove = (e->delay_destroy == 0) || e->tick_server(e, s);
			dict_entity_next(it);

			if(remove) {
				clin_rpc_send(&(struct client_rpc) {
					.type = CRPC_ENTITY_DESTROY,
					.payload.entity_destroy.entity_id = key,
				});

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
	w_coord_t c_nearest_dist2;
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

		s->world_time_start = time_get();

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
	s->world_time = 0;
	s->world_time_start = time_get();
	s->player.has_pos = false;
	s->player.finished_loading = false;
	string_init(s->level_name);
	inventory_create(&s->player.inventory, INVENTORY_SIZE);
	dict_entity_init(s->entities);

	struct thread t;
	thread_create(&t, server_local_thread, s, 8);
}
