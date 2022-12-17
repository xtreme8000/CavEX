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
