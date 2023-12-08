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

#include "../item/recipe.h"
#include "../item/window_container.h"
#include "inventory_logic.h"
#include "server_local.h"

static bool inv_pre_action(struct inventory* inv, size_t slot, bool right,
						   set_inv_slot_t changes) {
	if(slot == FURNACE_SLOT_OUTPUT) {
		struct item_data output;
		if(!right && inventory_get_slot(inv, FURNACE_SLOT_OUTPUT, &output)) {
			bool can_take = false;
			bool default_action;

			struct item_data picked;
			if(inventory_get_picked_item(inv, &picked)) {
				struct item* it_type = item_get(&picked);

				if(it_type && picked.id == output.id
				   && picked.durability == output.durability
				   && picked.count + output.count <= it_type->max_stack) {
					picked.count += output.count;
					inventory_set_picked_item(inv, picked);
					set_inv_slot_push(changes, SPECIAL_SLOT_PICKED_ITEM);
					default_action = false;
					can_take = true;
				}
			} else {
				default_action = true;
				can_take = true;
			}

			if(can_take) {
				struct item_data it;
				if(inventory_get_slot(inv, FURNACE_SLOT_INPUT, &it)
				   && it.count > 1) {
					it.count--;
					inventory_set_slot(inv, FURNACE_SLOT_INPUT, it);
				} else {
					inventory_clear_slot(inv, FURNACE_SLOT_INPUT);
				}

				set_inv_slot_push(changes, FURNACE_SLOT_INPUT);
				return default_action;
			}
		}

		return false;
	}

	return true;
}

static void inv_post_action(struct inventory* inv, size_t slot, bool right,
							bool accepted, set_inv_slot_t changes) {
	if(slot == FURNACE_SLOT_INPUT || slot == FURNACE_SLOT_OUTPUT) {
		struct item_data result = (struct item_data) {.count = 0};
		struct item_data it;

		if(inventory_get_slot(inv, FURNACE_SLOT_INPUT, &it)) {
			switch(it.id) {
				case ITEM_CLAY_BALL:
					result = (struct item_data) {
						.id = ITEM_BRICK, .durability = 0, .count = 1};
					break;
				case BLOCK_CACTUS:
					result = (struct item_data) {
						.id = ITEM_DYE, .durability = 2, .count = 1};
					break;
				case BLOCK_LOG:
					result = (struct item_data) {
						.id = ITEM_COAL, .durability = 1, .count = 1};
					break;
				case BLOCK_COBBLESTONE:
					result = (struct item_data) {
						.id = BLOCK_STONE, .durability = 0, .count = 1};
					break;
				case BLOCK_DIAMOND_ORE:
					result = (struct item_data) {
						.id = ITEM_DIAMOND, .durability = 0, .count = 1};
					break;
				case BLOCK_SAND:
					result = (struct item_data) {
						.id = BLOCK_GLASS, .durability = 0, .count = 1};
					break;
				case ITEM_FISH:
					result = (struct item_data) {
						.id = ITEM_FISH_COOKED, .durability = 0, .count = 1};
					break;
				case BLOCK_GOLD_ORE:
					result = (struct item_data) {
						.id = ITEM_GOLD, .durability = 0, .count = 1};
					break;
				case BLOCK_IRON_ORE:
					result = (struct item_data) {
						.id = ITEM_IRON, .durability = 0, .count = 1};
					break;
				case ITEM_PORKCHOP:
					result = (struct item_data) {.id = ITEM_PORKCHOP_COOKED,
												 .durability = 0,
												 .count = 1};
					break;
				default: break;
			}
		}

		if(result.count > 0) {
			inventory_set_slot(inv, FURNACE_SLOT_OUTPUT, result);
		} else {
			inventory_clear_slot(inv, FURNACE_SLOT_OUTPUT);
		}

		set_inv_slot_push(changes, FURNACE_SLOT_OUTPUT);
	}
}

static void inv_on_close(struct inventory* inv) {
	struct server_local* s = inv->user;

	set_inv_slot_t changes;
	set_inv_slot_init(changes);

	inventory_clear_slot(inv, FURNACE_SLOT_OUTPUT);
	set_inv_slot_push(changes, FURNACE_SLOT_OUTPUT);

	for(size_t k = FURNACE_SLOT_INPUT;
		k < FURNACE_SLOT_INPUT + FURNACE_SIZE_INPUT; k++) {
		struct item_data item;
		inventory_get_slot(inv, k, &item);

		if(item.id != 0) {
			inventory_clear_slot(inv, k);
			set_inv_slot_push(changes, k);
			server_local_spawn_item(
				(vec3) {s->player.x, s->player.y, s->player.z}, &item, true, s);
		}
	}

	struct item_data picked_item;
	if(inventory_get_picked_item(inv, &picked_item)) {
		inventory_clear_picked_item(inv);
		set_inv_slot_push(changes, SPECIAL_SLOT_PICKED_ITEM);
		server_local_spawn_item((vec3) {s->player.x, s->player.y, s->player.z},
								&picked_item, true, s);
	}

	server_local_send_inv_changes(changes, inv, WINDOWC_FURNACE);
	set_inv_slot_clear(changes);

	inventory_destroy(inv);
}

static bool inv_on_collect(struct inventory* inv, struct item_data* item) {
	uint8_t priorities[INVENTORY_SIZE_HOTBAR + INVENTORY_SIZE_MAIN];

	for(size_t k = 0; k < INVENTORY_SIZE_HOTBAR; k++)
		priorities[k] = k + FURNACE_SLOT_HOTBAR;

	for(size_t k = 0; k < INVENTORY_SIZE_MAIN; k++)
		priorities[k + INVENTORY_SIZE_HOTBAR] = k + FURNACE_SLOT_MAIN;

	set_inv_slot_t changes;
	set_inv_slot_init(changes);

	bool success
		= inventory_collect(inv, item, priorities,
							sizeof(priorities) / sizeof(*priorities), changes);
	server_local_send_inv_changes(changes, inv, WINDOWC_FURNACE);
	set_inv_slot_clear(changes);

	return success;
}

static void inv_on_create(struct inventory* inv) {
	struct server_local* s = inv->user;

	set_inv_slot_t changes;
	set_inv_slot_init(changes);

	for(size_t k = 0; k < INVENTORY_SIZE_HOTBAR; k++) {
		inv->items[k + FURNACE_SLOT_HOTBAR]
			= s->player.inventory.items[k + INVENTORY_SLOT_HOTBAR];
		set_inv_slot_push(changes, k + FURNACE_SLOT_HOTBAR);
	}

	for(size_t k = 0; k < INVENTORY_SIZE_MAIN; k++) {
		inv->items[k + FURNACE_SLOT_MAIN]
			= s->player.inventory.items[k + INVENTORY_SLOT_MAIN];
		set_inv_slot_push(changes, k + FURNACE_SLOT_MAIN);
	}

	server_local_send_inv_changes(changes, inv, WINDOWC_FURNACE);
	set_inv_slot_clear(changes);
}

static bool inv_on_destroy(struct inventory* inv) {
	struct server_local* s = inv->user;

	set_inv_slot_t changes;
	set_inv_slot_init(changes);

	for(size_t k = 0; k < INVENTORY_SIZE_HOTBAR; k++) {
		s->player.inventory.items[k + INVENTORY_SLOT_HOTBAR]
			= inv->items[k + FURNACE_SLOT_HOTBAR];
		set_inv_slot_push(changes, k + INVENTORY_SLOT_HOTBAR);
	}

	for(size_t k = 0; k < INVENTORY_SIZE_MAIN; k++) {
		s->player.inventory.items[k + INVENTORY_SLOT_MAIN]
			= inv->items[k + FURNACE_SLOT_MAIN];
		set_inv_slot_push(changes, k + INVENTORY_SLOT_MAIN);
	}

	server_local_send_inv_changes(changes, &s->player.inventory,
								  WINDOWC_INVENTORY);
	set_inv_slot_clear(changes);
	return true;
}

struct inventory_logic inventory_logic_furnace = {
	.pre_action = inv_pre_action,
	.post_action = inv_post_action,
	.on_collect = inv_on_collect,
	.on_create = inv_on_create,
	.on_destroy = inv_on_destroy,
	.on_close = inv_on_close,
};