#ifndef ITEMS_H
#define ITEMS_H

#include <stdint.h>

#include "../block/blocks.h"

struct item_data {
	uint16_t id;
	uint16_t durability;
	uint8_t count;
};

static bool item_is_block(struct item_data* item) {
	return item->id < 256 && blocks[item->id];
}

#endif
