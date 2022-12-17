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

#include <stdbool.h>
#include <stddef.h>

#include "items.h"

#define INVENTORY_SIZE (INVENTORY_SIZE_MAIN + 4)
#define INVENTORY_SIZE_MAIN 36
#define INVENTORY_SIZE_HOTBAR 9

#define INVENTORY_SLOT_HELMET (INVENTORY_SIZE_MAIN)
#define INVENTORY_SLOT_CHESTPLATE (INVENTORY_SIZE_MAIN + 1)
#define INVENTORY_SLOT_LEGGINGS (INVENTORY_SIZE_MAIN + 2)
#define INVENTORY_SLOT_BOOTS (INVENTORY_SIZE_MAIN + 3)

struct inventory {
	struct item_data items[INVENTORY_SIZE];
	int hotbar_slot;
	int picked_item;
};

void inventory_clear(struct inventory* inv);
size_t inventory_get_hotbar(struct inventory* inv);
void inventory_set_hotbar(struct inventory* inv, size_t slot);
void inventory_set_slot(struct inventory* inv, size_t slot,
						struct item_data item);
bool inventory_get_slot(struct inventory* inv, size_t slot,
						struct item_data* item);
bool inventory_get_picked_item(struct inventory* inv, struct item_data* item);
bool inventory_pick_item(struct inventory* inv, size_t slot);
bool inventory_place_item(struct inventory* inv, size_t slot);

#endif
