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

#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include "../item/items.h"
#include "../item/window_container.h"
#include "../world.h"

#include "../cglm/cglm.h"

enum client_rpc_type {
	CRPC_CHUNK,
	CRPC_UNLOAD_CHUNK,
	CRPC_INVENTORY_SLOT,
	CRPC_PLAYER_POS,
	CRPC_TIME_SET,
	CRPC_WORLD_RESET,
	CRPC_SET_BLOCK,
	CRPC_WINDOW_TRANSACTION,
	CRPC_SPAWN_ITEM,
	CRPC_PICKUP_ITEM,
	CRPC_ENTITY_DESTROY,
	CRPC_ENTITY_MOVE,
	CRPC_OPEN_WINDOW,
};

struct client_rpc {
	enum client_rpc_type type;
	union {
		struct {
			w_coord_t x, y, z;
			w_coord_t sx, sy, sz;
			uint8_t* ids;
			uint8_t* metadata;
			uint8_t* lighting_sky;
			uint8_t* lighting_torch;
		} chunk;
		struct {
			w_coord_t x, z;
		} unload_chunk;
		struct {
			uint8_t window;
			uint8_t slot;
			struct item_data item;
		} inventory_slot;
		struct {
			vec3 position;
			vec2 rotation;
		} player_pos;
		uint64_t time_set;
		struct {
			enum world_dim dimension;
			uint32_t local_entity;
		} world_reset;
		struct {
			w_coord_t x, y, z;
			struct block_data block;
		} set_block;
		struct {
			uint8_t window;
			uint16_t action_id;
			bool accepted;
		} window_transaction;
		struct {
			uint8_t window;
			enum window_type type;
			uint8_t slot_count;
		} window_open;
		struct {
			uint32_t entity_id;
			struct item_data item;
			vec3 pos;
		} spawn_item;
		struct {
			uint32_t entity_id;
			uint32_t collector_id;
		} pickup_item;
		struct {
			uint32_t entity_id;
		} entity_destroy;
		struct {
			uint32_t entity_id;
			vec3 pos;
		} entity_move;
	} payload;
};

void clin_init(void);
void clin_update(void);
void clin_rpc_send(struct client_rpc* call);

#endif
