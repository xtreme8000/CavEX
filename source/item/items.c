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

#include "../block/blocks.h"
#include "../platform/graphics/render_item.h"
#include "items_object.h"

struct item* items[ITEMS_MAX];

void items_init() {
	for(int k = 0; k < ITEMS_MAX; k++)
		items[k] = NULL;

	render_item_init();

	for(int k = 0; k < 256; k++) {
		if(blocks[k]) {
			struct item* it = &blocks[k]->block_item;
			strncpy(it->name, blocks[k]->name, sizeof(it->name));
			it->name[sizeof(it->name) - 1] = 0;
			items[k] = it;
		}
	}

	items[256] = &item_iron_shovel;
	items[257] = &item_iron_pickaxe;
	items[258] = &item_iron_axe;
	items[259] = &item_flint_steel;
	items[260] = &item_apple;
	items[261] = &item_bow;
	items[262] = &item_arrow;
	items[263] = &item_coal;
	items[264] = &item_diamond;
	items[265] = &item_iron;
	items[266] = &item_gold;
	items[267] = &item_iron_sword;
	items[268] = &item_wood_sword;
	items[269] = &item_wood_shovel;
	items[270] = &item_wood_pickaxe;
	items[271] = &item_wood_axe;
	items[272] = &item_stone_sword;
	items[273] = &item_stone_shovel;
	items[274] = &item_stone_pickaxe;
	items[275] = &item_stone_axe;
	items[276] = &item_diamond_sword;
	items[277] = &item_diamond_shovel;
	items[278] = &item_diamond_pickaxe;
	items[279] = &item_diamond_axe;
	items[280] = &item_stick;
	// bowl
	// soup
	items[283] = &item_gold_sword;
	items[284] = &item_gold_shovel;
	items[285] = &item_gold_pickaxe;
	items[286] = item_gold_axe;
	// string
	// feather
	// gunpowder
	items[290] = &item_wood_hoe;
	items[291] = &item_stone_hoe;
	items[292] = &item_iron_hoe;
	items[293] = &item_diamond_hoe;
	items[294] = &item_gold_hoe;
	// seeds
	// wheat
	// bread
	// leather helmet
	// leather chestplate
	// leather leggings
	// leather boots
	// chain helmet
	// chain chestplate
	// chain leggings
	// chain boots
	// iron helmet
	// iron chestplate
	// iron leggings
	// iron boots
	// diamond helmet
	// diamond chestplate
	// diamond leggings
	// diamond boots
	// gold helmet
	// gold chestplate
	// gold leggings
	// gold boots
	// flint
	// porkchop
	// cooked porkchop
	// painting
	// golden apple
	// sign
	// wood door
	// bucket
	// water bucket
	// lava bucket
	// minecart
	// saddle
	// iron door
	// redstone
	// snowball
	// boat
	// leather
	// milk bucket
	// brick
	// clay
	// sugarcane
	// paper
	// book
	// slimeball
	// storage minecart
	// furnace minecart
	// egg
	// compass
	// fishing rod
	// clock
	// glowstone dust
	// fish
	// cooked fish
	// colored dye
	// bone
	// sugar
	// cake
	// bed
	// redstone repeater
	// cookie
	// map
	// shears

	// golden record
	// green record
}

struct item* item_get(struct item_data* item) {
	assert(item);
	return item->id < ITEMS_MAX ? items[item->id] : NULL;
}

bool item_is_block(struct item_data* item) {
	assert(item);
	return item_get(item) && item->id < 256 && blocks[item->id];
}
