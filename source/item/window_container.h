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

#ifndef WINDOW_CONTAINER_H
#define WINDOW_CONTAINER_H

#include <m-lib/m-i-list.h>
#include <stdint.h>

#include "inventory.h"

#define WINDOWC_INVENTORY 0
#define WINDOWC_CRAFTING 1 // local server use only

enum window_type {
	WINDOW_TYPE_CHEST = 0,
	WINDOW_TYPE_WORKBENCH = 1,
	WINDOW_TYPE_FURNACE = 2,
	WINDOW_TYPE_DISPENSER = 3,
	WINDOW_TYPE_INVENTORY = 255,
};

struct window_container {
	enum window_type type;
	size_t slot_count;
	uint16_t next_action_id;
	ilist_inventory_t invs;
};

bool windowc_create(struct window_container* wc, enum window_type type,
					size_t slot_count);
void windowc_destroy(struct window_container* wc);

struct inventory* windowc_get_latest(struct window_container* wc);
bool windowc_new_action(struct window_container* wc, uint16_t* action_id,
						bool action_type, size_t action_slot);
void windowc_action_apply_result(struct window_container* wc,
								 uint16_t action_id, bool accept);
void windowc_slot_change(struct window_container* wc, size_t slot,
						 struct item_data item);

#endif