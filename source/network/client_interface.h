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
#include "../world.h"

#include "../cglm/cglm.h"

enum client_rpc_type {
	CRPC_CHUNK,
	CRPC_UNLOAD_CHUNK,
	CRPC_INVENTORY_SLOT,
	CRPC_PLAYER_POS,
	CRPC_TIME_SET,
	CRPC_WORLD_RESET,
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
		} world_reset;
	} payload;
};

void clin_init(void);
void clin_update(void);
void clin_rpc_send(struct client_rpc* call);

#endif
