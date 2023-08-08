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

bool inventory_create(struct inventory* inv, size_t capacity) {
	assert(inv && capacity > 0);

	inv->capacity = capacity;
	inv->items = malloc(sizeof(struct item_data) * capacity);

	if(!inv->items)
		return false;

	inventory_clear(inv);

	ilist_inventory_init_field(inv);

	return true;
}

void inventory_copy(struct inventory* inv, struct inventory* from) {
	assert(inv && from);

	inv->capacity = from->capacity;
	inv->picked_item = from->picked_item;
	inv->hotbar_slot = from->hotbar_slot;

	for(size_t k = 0; k < inv->capacity; k++)
		inv->items[k] = from->items[k];
}

void inventory_destroy(struct inventory* inv) {
	assert(inv && inv->items);
	free(inv->items);
}

void inventory_clear(struct inventory* inv) {
	assert(inv);

	for(size_t k = 0; k < inv->capacity; k++) {
		inv->items[k].id = 0;
		inv->items[k].durability = 0;
		inv->items[k].count = 0;
	}

	inv->picked_item.id = 0;
	inv->picked_item.durability = 0;
	inv->picked_item.count = 0;
	inv->hotbar_slot = 0;
}

void inventory_consume(struct inventory* inv, size_t slot) {
	assert(inv && slot < inv->capacity);

	if(inv->items[slot].count > 0) {
		inv->items[slot].count--;

		if(inv->items[slot].count == 0) {
			inv->items[slot].id = 0;
			inv->items[slot].durability = 0;
			inv->items[slot].count = 0;
		}
	}
}

#define min(a, b) ((a) < (b) ? (a) : (b))

bool inventory_collect(struct inventory* inv, struct item_data* item,
					   size_t slot_start, size_t slot_length, bool* mask) {
	assert(inv && item && item->id != 0 && mask);

	struct item* it = item_get(item);

	if(!it)
		return false;

	while(item->count > 0) {
		bool has_canidate_equal = false;
		size_t candidate_equal = 0;
		bool has_canidate_empty = false;
		size_t candidate_empty = 0;

		for(size_t k = slot_start; k < slot_start + slot_length; k++) {
			if(inv->items[k].id == item->id
			   && inv->items[k].durability == item->durability
			   && inv->items[k].count < it->max_stack) {
				has_canidate_equal = true;
				candidate_equal = k;
				break;
			}

			if(!has_canidate_empty && inv->items[k].id == 0) {
				has_canidate_empty = true;
				candidate_empty = k;
			}
		}

		if(has_canidate_equal || has_canidate_empty) {
			size_t candidate
				= has_canidate_equal ? candidate_equal : candidate_empty;
			size_t additional
				= min(it->max_stack - inv->items[candidate].count, item->count);
			inv->items[candidate].id = item->id;
			inv->items[candidate].durability = item->durability;
			inv->items[candidate].count += additional;
			item->count -= additional;
			mask[candidate] = true;
		} else {
			return false;
		}
	}

	return true;
}

size_t inventory_get_hotbar(struct inventory* inv) {
	assert(inv);
	return inv->hotbar_slot;
}

void inventory_set_hotbar(struct inventory* inv, size_t slot) {
	assert(inv && slot < INVENTORY_SIZE_HOTBAR);
	inv->hotbar_slot = slot;
}

bool inventory_get_hotbar_item(struct inventory* inv, struct item_data* item) {
	return inventory_get_slot(
		inv, inventory_get_hotbar(inv) + INVENTORY_SLOT_HOTBAR, item);
}

void inventory_set_slot(struct inventory* inv, size_t slot,
						struct item_data item) {
	assert(inv && slot < inv->capacity);
	inv->items[slot] = item;
}

void inventory_clear_slot(struct inventory* inv, size_t slot) {
	assert(inv && slot < inv->capacity);
	inv->items[slot].id = 0;
	inv->items[slot].durability = 0;
	inv->items[slot].count = 0;
}

bool inventory_get_slot(struct inventory* inv, size_t slot,
						struct item_data* item) {
	assert(inv && slot < inv->capacity);
	if(item)
		*item = inv->items[slot];
	return inv->items[slot].id > 0;
}

bool inventory_get_picked_item(struct inventory* inv, struct item_data* item) {
	assert(inv);

	if(inv->picked_item.id != 0) {
		if(item)
			*item = inv->picked_item;
		return true;
	} else {
		return false;
	}
}

static bool inventory_pick_item(struct inventory* inv, size_t slot) {
	if(inv->picked_item.id == 0 && inv->items[slot].id != 0) {
		inv->picked_item = inv->items[slot];
		inv->items[slot].id = 0;
		inv->items[slot].durability = 0;
		inv->items[slot].count = 0;
		return true;
	} else {
		return false;
	}
}

static bool inventory_pick_item_split(struct inventory* inv, size_t slot) {
	if(inv->picked_item.id == 0 && inv->items[slot].id != 0) {
		if(inv->items[slot].count == 1)
			return inventory_pick_item(inv, slot);

		int total = inv->items[slot].count;

		inv->picked_item = inv->items[slot];
		inv->picked_item.count = total - total / 2;
		inv->items[slot].count = total / 2;
		return true;
	} else {
		return false;
	}
}

static bool inventory_place_item(struct inventory* inv, size_t slot) {
	assert(inv->picked_item.id != 0);

	if(inv->items[slot].id == inv->picked_item.id
	   && inv->items[slot].durability == inv->picked_item.durability) {
		int total = inv->items[slot].count + inv->picked_item.count;
		int max_stack = item_get(&inv->picked_item)->max_stack;

		if(total <= max_stack) {
			inv->items[slot].count = total;
			inv->picked_item.id = 0;
			inv->picked_item.durability = 0;
			inv->picked_item.count = 0;
		} else {
			inv->items[slot].count = max_stack;
			inv->picked_item.count = total - max_stack;
		}
	} else {
		struct item_data tmp = inv->items[slot];
		inv->items[slot] = inv->picked_item;
		inv->picked_item = tmp;
	}

	return true;
}

bool inventory_action(struct inventory* inv, size_t slot, bool right) {
	assert(inv && slot < inv->capacity);

	if(right) {
		return (inv->picked_item.id == 0) ?
			inventory_pick_item_split(inv, slot) :
			false;
	} else {
		return (inv->picked_item.id == 0) ? inventory_pick_item(inv, slot) :
											inventory_place_item(inv, slot);
	}
}