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

#include "inventory.h"

#define NO_ITEM ((int)-1)

void inventory_clear(struct inventory* inv) {
	assert(inv);

	for(size_t k = 0; k < INVENTORY_SIZE; k++) {
		inv->items[k].id = 0;
		inv->items[k].durability = 0;
		inv->items[k].count = 0;
	}

	inv->picked_item = NO_ITEM;
	inv->hotbar_slot = 0;
}

void inventory_consume(struct inventory* inv, size_t slot) {
	assert(inv && slot < INVENTORY_SIZE_HOTBAR);

	if(inv->items[slot].count > 0) {
		inv->items[slot].count--;

		if(inv->items[slot].count == 0) {
			inv->items[slot].id = 0;
			inv->items[slot].durability = 0;
			inv->items[slot].count = 0;
		}
	}
}

size_t inventory_get_hotbar(struct inventory* inv) {
	assert(inv);
	return inv->hotbar_slot;
}

void inventory_set_hotbar(struct inventory* inv, size_t slot) {
	assert(inv && slot < INVENTORY_SIZE_HOTBAR);
	inv->hotbar_slot = slot;
}

void inventory_set_slot(struct inventory* inv, size_t slot,
						struct item_data item) {
	assert(inv && slot < INVENTORY_SIZE);
	inv->items[slot] = item;
}

bool inventory_get_slot(struct inventory* inv, size_t slot,
						struct item_data* item) {
	assert(inv && slot < INVENTORY_SIZE);
	if(item)
		*item = inv->items[slot];
	return inv->picked_item != (int)slot && inv->items[slot].id > 0;
}

bool inventory_get_picked_item(struct inventory* inv, struct item_data* item) {
	assert(inv && item);

	if(inv->picked_item != NO_ITEM) {
		*item = inv->items[inv->picked_item];
		return true;
	} else {
		return false;
	}
}

bool inventory_pick_item(struct inventory* inv, size_t slot) {
	if(inv->picked_item == NO_ITEM && inventory_get_slot(inv, slot, NULL)) {
		inv->picked_item = slot;
		return true;
	} else {
		return false;
	}
}

bool inventory_place_item(struct inventory* inv, size_t slot) {
	assert(inv && slot);

	if(!inventory_get_slot(inv, slot, NULL)) {
		inv->items[slot] = inv->items[inv->picked_item];
		inv->picked_item = NO_ITEM;
		return true;
	} else {
		return false;
	}
}
