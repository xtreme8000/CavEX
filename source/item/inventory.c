#include <assert.h>

#include "inventory.h"

#define NO_ITEM (-1)

void inventory_clear(struct inventory* inv) {
	assert(inv);

	for(size_t k = 0; k < INVENTORY_SIZE; k++)
		inv->items[k].id = 0;

	inv->picked_item = NO_ITEM;
	inv->hotbar_slot = 0;
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
