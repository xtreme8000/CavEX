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

#include <stdarg.h>

#include "recipe.h"

array_recipe_t recipes_crafting;

void recipe_init() {
	array_recipe_init(recipes_crafting);

	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_PLANKS, .durability = 0, .count = 4}, 1,
		1, (uint8_t[]) {1}, (struct item_data) {.id = BLOCK_LOG}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_STICK, .durability = 0, .count = 4}, 1,
		2, (uint8_t[]) {1, 1}, (struct item_data) {.id = BLOCK_PLANKS}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_WORKBENCH, .durability = 0, .count = 1},
		2, 2, (uint8_t[]) {1, 1, 1, 1}, (struct item_data) {.id = BLOCK_PLANKS},
		false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_PICKAXE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 0, 2, 0, 0, 2, 0},
			   (struct item_data) {.id = ITEM_DIAMOND}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
}

void recipe_add(array_recipe_t recipes, struct item_data result, size_t width,
				size_t height, uint8_t* shape, ...) {
	assert(recipes && width > 0 && height > 0 && width * height <= 9 && shape);

	size_t count = 0;
	for(size_t k = 0; k < width * height; k++) {
		if(shape[k] > count)
			count = shape[k];
	}

	assert(count > 0 && count <= 9);

	struct recipe_ingredients ingredients[9];

	va_list inputs;
	va_start(inputs, shape);

	for(size_t k = 0; k < count; k++) {
		ingredients[k].item = va_arg(inputs, struct item_data);
		ingredients[k].match_durability = (bool)va_arg(inputs, int);
	}

	va_end(inputs);

	struct recipe r = (struct recipe) {
		.result = result,
		.width = width,
		.height = height,
	};

	for(size_t k = 0; k < width * height; k++) {
		if(shape[k] > 0) {
			r.shape[k] = ingredients[shape[k] - 1];
		} else {
			r.shape[k].item.id = 0;
		}
	}

	array_recipe_push_back(recipes, r);
}

bool recipe_match(array_recipe_t recipes, struct item_data slots[9],
				  bool slot_empty[9], struct item_data* result) {
	assert(recipes && slots && slot_empty && result);

	array_recipe_it_t it;
	array_recipe_it(it, recipes);

	while(!array_recipe_end_p(it)) {
		struct recipe* current = array_recipe_ref(it);

		for(size_t y = 0; y <= 3 - current->height; y++) {
			for(size_t x = 0; x <= 3 - current->width; x++) {
				bool match = true;

				// check that outside of pattern is empty
				for(size_t py = 0; py < 3 && match; py++) {
					for(size_t px = 0; px < 3 && match; px++) {
						if((px < x || px >= x + current->width || py < y
							|| py >= y + current->height)
						   && !slot_empty[px + py * 3])
							match = false;
					}
				}

				// check pattern itself
				for(size_t py = 0; py < current->height && match; py++) {
					for(size_t px = 0; px < current->width && match; px++) {
						size_t slots_idx = (px + x) + (py + y) * 3;
						size_t shape_idx = px + py * current->width;

						if(current->shape[shape_idx].item.id == 0) {
							if(!slot_empty[slots_idx])
								match = false;
						} else {
							if(slot_empty[slots_idx]
							   || current->shape[shape_idx].item.id
								   != slots[slots_idx].id
							   || (current->shape->match_durability
								   && current->shape[shape_idx].item.durability
									   != slots[slots_idx].durability))
								match = false;
						}
					}
				}

				if(match) {
					*result = current->result;
					return true;
				}
			}
		}

		array_recipe_next(it);
	}

	return false;
}