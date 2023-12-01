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

#include <assert.h>

#include "window_container.h"

bool windowc_create(struct window_container* wc, enum window_type type) {
	assert(wc);

	ilist_inventory_init(wc->invs);

	wc->type = type;
	wc->next_action_id = 0;

	uint16_t unused;
	// no action performed because new inventory is empty
	return windowc_new_action(wc, &unused, false, 0);
}

void windowc_destroy(struct window_container* wc) {
	assert(wc);

	ilist_inventory_it_t it;
	ilist_inventory_it(it, wc->invs);

	while(!ilist_inventory_end_p(it)) {
		struct inventory* inv = ilist_inventory_ref(it);
		ilist_inventory_remove(wc->invs, it);
		inventory_destroy(inv);
		free(inv);
	}

	ilist_inventory_clear(wc->invs);
}

struct inventory* windowc_get_latest(struct window_container* wc) {
	assert(wc);
	assert(!ilist_inventory_empty_p(wc->invs));

	ilist_inventory_it_t it;
	ilist_inventory_it_last(it, wc->invs);
	return ilist_inventory_ref(it);
}

bool windowc_new_action(struct window_container* wc, uint16_t* action_id,
						bool action_type, size_t action_slot) {
	assert(wc && action_id);

	struct inventory* next = malloc(sizeof(struct inventory));

	if(!next)
		return false;

	if(!inventory_create(next, NULL, NULL, INVENTORY_SIZE)) // TODO
		return false;

	if(!ilist_inventory_empty_p(wc->invs))
		inventory_copy(next, windowc_get_latest(wc));

	next->revision.action_id = wc->next_action_id;
	next->revision.action_type = action_type;
	next->revision.action_slot = action_slot;

	inventory_action(next, action_slot, action_type, NULL);

	*action_id = wc->next_action_id;
	wc->next_action_id++;

	ilist_inventory_push_back(wc->invs, next);
	return true;
}

void windowc_action_apply_result(struct window_container* wc,
								 uint16_t action_id, bool accept) {
	assert(wc);
	assert(!ilist_inventory_empty_p(wc->invs));

	ilist_inventory_it_t it;
	ilist_inventory_it(it, wc->invs);

	if(accept) {
		while(!ilist_inventory_end_p(it)) {
			struct inventory* inv = ilist_inventory_ref(it);

			if(inv->revision.action_id == action_id)
				break;

			ilist_inventory_remove(wc->invs, it);
			inventory_destroy(inv);
			free(inv);
		}
	} else {
		bool found = false;

		while(!ilist_inventory_end_p(it)) {
			struct inventory* inv = ilist_inventory_ref(it);

			if(inv->revision.action_id == action_id)
				found = true;

			if(found) {
				ilist_inventory_remove(wc->invs, it);
				inventory_destroy(inv);
				free(inv);
			} else {
				ilist_inventory_next(it);
			}
		}
	}

	assert(!ilist_inventory_empty_p(wc->invs));
}

void windowc_slot_change(struct window_container* wc, size_t slot,
						 struct item_data item) {
	assert(wc);
	assert(!ilist_inventory_empty_p(wc->invs));

	ilist_inventory_it_t it;
	ilist_inventory_it(it, wc->invs);

	struct inventory* prev = ilist_inventory_ref(it);

	if(slot == SPECIAL_SLOT_PICKED_ITEM) {
		inventory_set_picked_item(prev, item);
	} else {
		inventory_set_slot(prev, slot, item);
	}

	ilist_inventory_next(it);

	while(!ilist_inventory_end_p(it)) {
		struct inventory* current = ilist_inventory_ref(it);
		inventory_copy(current, prev);
		inventory_action(current, current->revision.action_slot,
						 current->revision.action_type, NULL);

		prev = current;
		ilist_inventory_next(it);
	}
}