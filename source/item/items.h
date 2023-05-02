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

#ifndef ITEMS_H
#define ITEMS_H

#include <stdbool.h>
#include <stdint.h>

#include "../cglm/cglm.h"

struct item_data {
	uint16_t id;
	uint16_t durability;
	uint8_t count;
};

#include "../block/blocks_data.h"
struct server_local;

struct item {
	char name[32];
	bool has_damage;
	uint8_t max_stack;
	void (*renderItem)(struct item*, struct item_data*, mat4, bool);
	bool (*onItemPlace)(struct server_local*, struct item_data*,
						struct block_info*, struct block_info*, enum side);
	union {
		struct {
			uint8_t texture_x : 4;
			uint8_t texture_y : 4;
		} item;
		struct {
			bool has_default;
			uint8_t default_metadata : 4;
			uint8_t default_rotation : 2;
		} block;
	} render_data;
};

#define ITEMS_MAX 512

extern struct item* items[ITEMS_MAX];

void items_init(void);
struct item* item_get(struct item_data* item);
bool item_is_block(struct item_data* item);

#endif
