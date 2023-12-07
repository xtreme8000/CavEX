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
#include "server_local.h"

static bool inv_match_crafting(struct inventory* inv,
							   struct item_data* result) {
	struct item_data slots[9];
	bool slot_empty[9];
	memset(slot_empty, true, sizeof(slot_empty));

	for(size_t k = 0; k < CRAFTING_SIZE_INPUT; k++)
		slot_empty[k]
			= !inventory_get_slot(inv, CRAFTING_SLOT_INPUT + k, slots + k);

	return recipe_match(recipes_crafting, slots, slot_empty, result);
}

static bool inv_pre_action(struct inventory* inv, size_t slot, bool right,
						   set_inv_slot_t changes) {
	if(slot == CRAFTING_SLOT_OUTPUT) {
		struct item_data output;
		if(!right && inventory_get_slot(inv, CRAFTING_SLOT_OUTPUT, &output)) {
			bool crafted = false;
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
					crafted = true;
				}
			} else {
				default_action = true;
				crafted = true;
			}

			if(crafted) {
				for(size_t k = CRAFTING_SLOT_INPUT;
					k < CRAFTING_SLOT_INPUT + CRAFTING_SIZE_INPUT; k++) {
					struct item_data it;

					if(inventory_get_slot(inv, k, &it) && it.count > 1) {
						it.count--;
						inventory_set_slot(inv, k, it);
					} else {
						inventory_clear_slot(inv, k);
					}

					set_inv_slot_push(changes, k);
				}

				return default_action;
			}
		}

		return false;
	}

	return true;
}

static void inv_post_action(struct inventory* inv, size_t slot, bool right,
							bool accepted, set_inv_slot_t changes) {
	if((slot >= CRAFTING_SLOT_INPUT
		&& slot < CRAFTING_SLOT_INPUT + CRAFTING_SIZE_INPUT)
	   || slot == CRAFTING_SLOT_OUTPUT) {
		struct item_data result;
		if(inv_match_crafting(inv, &result)) {
			inventory_set_slot(inv, CRAFTING_SLOT_OUTPUT, result);
		} else {
			inventory_clear_slot(inv, CRAFTING_SLOT_OUTPUT);
		}

		set_inv_slot_push(changes, CRAFTING_SLOT_OUTPUT);
	}
}

static void inv_on_close(struct inventory* inv) {
	struct server_local* s = inv->user;

	set_inv_slot_t changes;
	set_inv_slot_init(changes);

	inventory_clear_slot(inv, CRAFTING_SLOT_OUTPUT);
	set_inv_slot_push(changes, CRAFTING_SLOT_OUTPUT);

	for(size_t k = CRAFTING_SLOT_INPUT;
		k < CRAFTING_SLOT_INPUT + CRAFTING_SIZE_INPUT; k++) {
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

	server_local_send_inv_changes(changes, inv, WINDOWC_CRAFTING);
	set_inv_slot_clear(changes);

	inventory_destroy(inv);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

static bool inventory_collect(struct inventory* inv, struct item_data* item,
							  uint8_t* slot_priority, size_t slot_length,
							  set_inv_slot_t changes) {
	assert(inv && item && item->id != 0 && changes);

	struct item* it = item_get(item);

	if(!it)
		return false;

	while(item->count > 0) {
		bool has_canidate_equal = false;
		size_t candidate_equal = 0;
		bool has_canidate_empty = false;
		size_t candidate_empty = 0;

		for(size_t k = 0; k < slot_length; k++) {
			uint8_t slot = slot_priority[k];

			if(inv->items[slot].id == item->id
			   && inv->items[slot].durability == item->durability
			   && inv->items[slot].count < it->max_stack) {
				has_canidate_equal = true;
				candidate_equal = slot;
				break;
			}

			if(!has_canidate_empty && inv->items[slot].id == 0) {
				has_canidate_empty = true;
				candidate_empty = slot;
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
			set_inv_slot_push(changes, candidate);
		} else {
			return false;
		}
	}

	return true;
}

static bool inv_on_collect(struct inventory* inv, struct item_data* item) {
	uint8_t priorities[INVENTORY_SIZE_HOTBAR + INVENTORY_SIZE_MAIN];

	for(size_t k = 0; k < INVENTORY_SIZE_HOTBAR; k++)
		priorities[k] = k + CRAFTING_SLOT_HOTBAR;

	for(size_t k = 0; k < INVENTORY_SIZE_MAIN; k++)
		priorities[k + INVENTORY_SIZE_HOTBAR] = k + CRAFTING_SLOT_MAIN;

	set_inv_slot_t changes;
	set_inv_slot_init(changes);

	bool success
		= inventory_collect(inv, item, priorities,
							sizeof(priorities) / sizeof(*priorities), changes);
	server_local_send_inv_changes(changes, inv, WINDOWC_CRAFTING);
	set_inv_slot_clear(changes);

	return success;
}

static void inv_on_create(struct inventory* inv) {
	struct server_local* s = inv->user;

	set_inv_slot_t changes;
	set_inv_slot_init(changes);

	for(size_t k = 0; k < INVENTORY_SIZE_HOTBAR; k++) {
		inv->items[k + CRAFTING_SLOT_HOTBAR]
			= s->player.inventory.items[k + INVENTORY_SLOT_HOTBAR];
		set_inv_slot_push(changes, k + CRAFTING_SLOT_HOTBAR);
	}

	for(size_t k = 0; k < INVENTORY_SIZE_MAIN; k++) {
		inv->items[k + CRAFTING_SLOT_MAIN]
			= s->player.inventory.items[k + INVENTORY_SLOT_MAIN];
		set_inv_slot_push(changes, k + CRAFTING_SLOT_MAIN);
	}

	server_local_send_inv_changes(changes, inv, WINDOWC_CRAFTING);
	set_inv_slot_clear(changes);
}

static bool inv_on_destroy(struct inventory* inv) {
	struct server_local* s = inv->user;

	set_inv_slot_t changes;
	set_inv_slot_init(changes);

	for(size_t k = 0; k < INVENTORY_SIZE_HOTBAR; k++) {
		s->player.inventory.items[k + INVENTORY_SLOT_HOTBAR]
			= inv->items[k + CRAFTING_SLOT_HOTBAR];
		set_inv_slot_push(changes, k + INVENTORY_SLOT_HOTBAR);
	}

	for(size_t k = 0; k < INVENTORY_SIZE_MAIN; k++) {
		s->player.inventory.items[k + INVENTORY_SLOT_MAIN]
			= inv->items[k + CRAFTING_SLOT_MAIN];
		set_inv_slot_push(changes, k + INVENTORY_SLOT_MAIN);
	}

	server_local_send_inv_changes(changes, &s->player.inventory,
								  WINDOWC_INVENTORY);
	set_inv_slot_clear(changes);
	return true;
}

struct inventory_logic inventory_logic_crafting = {
	.pre_action = inv_pre_action,
	.post_action = inv_post_action,
	.on_collect = inv_on_collect,
	.on_create = inv_on_create,
	.on_destroy = inv_on_destroy,
	.on_close = inv_on_close,
};