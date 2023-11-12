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

#ifndef INVENTORY_H
#define INVENTORY_H

#include <m-lib/m-i-list.h>
#include <stdbool.h>
#include <stddef.h>

#include "items.h"

#define INVENTORY_SIZE 45
#define INVENTORY_SIZE_HOTBAR 9
#define INVENTORY_SIZE_ARMOR 4
#define INVENTORY_SIZE_MAIN 27
#define INVENTORY_SIZE_CRAFTING 4

#define INVENTORY_SLOT_OUTPUT 0
#define INVENTORY_SLOT_CRAFTING 1
#define INVENTORY_SLOT_ARMOR 5
#define INVENTORY_SLOT_MAIN 9
#define INVENTORY_SLOT_HOTBAR 36

#define SPECIAL_SLOT_PICKED_ITEM 255

struct inventory {
	struct item_data picked_item;
	struct item_data* items;
	size_t capacity;
	int hotbar_slot;
	struct {
		uint16_t action_id;
		bool action_type;
		size_t action_slot;
	} revision; // for window container
	ILIST_INTERFACE(ilist_inventory, struct inventory);
};

ILIST_DEF(ilist_inventory, struct inventory, M_POD_OPLIST)

bool inventory_create(struct inventory* inv, size_t capacity);
void inventory_copy(struct inventory* inv, struct inventory* from);
void inventory_destroy(struct inventory* inv);
void inventory_clear(struct inventory* inv);
void inventory_consume(struct inventory* inv, size_t slot);
bool inventory_collect(struct inventory* inv, struct item_data* item,
					   uint8_t* slot_priority, size_t slot_length, bool* mask);
bool inventory_collect_inventory(struct inventory* inv, struct item_data* item,
								 bool* mask);
size_t inventory_get_hotbar(struct inventory* inv);
void inventory_set_hotbar(struct inventory* inv, size_t slot);
bool inventory_get_hotbar_item(struct inventory* inv, struct item_data* item);
void inventory_set_slot(struct inventory* inv, size_t slot,
						struct item_data item);
void inventory_clear_slot(struct inventory* inv, size_t slot);
bool inventory_get_slot(struct inventory* inv, size_t slot,
						struct item_data* item);
void inventory_clear_picked_item(struct inventory* inv);
void inventory_set_picked_item(struct inventory* inv, struct item_data item);
bool inventory_get_picked_item(struct inventory* inv, struct item_data* item);
bool inventory_action(struct inventory* inv, size_t slot, bool right);

#endif
