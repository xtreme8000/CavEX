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

#ifndef RECIPE_H
#define RECIPE_H

#include <m-lib/m-array.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "items.h"

struct recipe {
	struct item_data result;
	size_t width, height;
	struct recipe_ingredients {
		struct item_data item;
		bool match_durability;
	} shape[9];
};

ARRAY_DEF(array_recipe, struct recipe, M_POD_OPLIST)

extern array_recipe_t recipes_crafting;

void recipe_init(void);
void recipe_add(array_recipe_t recipes, struct item_data result, size_t width,
				size_t height, uint8_t* shape, ...);
bool recipe_match(array_recipe_t recipes, struct item_data slots[9],
				  bool slot_empty[9], struct item_data* result);

#endif