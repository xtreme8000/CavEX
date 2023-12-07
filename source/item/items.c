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
#include "../graphics/render_item.h"
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
	items[281] = &item_bowl;
	items[282] = &item_mushroom_stew;
	items[283] = &item_gold_sword;
	items[284] = &item_gold_shovel;
	items[285] = &item_gold_pickaxe;
	items[286] = &item_gold_axe;
	items[287] = &item_string;
	items[288] = &item_feather;
	items[289] = &item_gunpowder;
	items[290] = &item_wood_hoe;
	items[291] = &item_stone_hoe;
	items[292] = &item_iron_hoe;
	items[293] = &item_diamond_hoe;
	items[294] = &item_gold_hoe;
	items[295] = &item_seeds;
	items[296] = &item_wheat;
	items[297] = &item_bread;
	items[298] = &item_leather_helmet;
	items[299] = &item_leather_chestplate;
	items[300] = &item_leather_leggings;
	items[301] = &item_leather_boots;
	items[302] = &item_chain_helmet;
	items[303] = &item_chain_chestplate;
	items[304] = &item_chain_leggings;
	items[305] = &item_chain_boots;
	items[306] = &item_iron_helmet;
	items[307] = &item_iron_chestplate;
	items[308] = &item_iron_leggings;
	items[309] = &item_iron_boots;
	items[310] = &item_diamond_helmet;
	items[311] = &item_diamond_chestplate;
	items[312] = &item_diamond_leggings;
	items[313] = &item_diamond_boots;
	items[314] = &item_gold_helmet;
	items[315] = &item_gold_chestplate;
	items[316] = &item_gold_leggings;
	items[317] = &item_gold_boots;
	items[318] = &item_flint;
	items[319] = &item_porkchop;
	items[320] = &item_porkchop_cooked;
	// painting
	items[322] = &item_apple_golden;
	// sign
	items[324] = &item_door_wood;
	// bucket
	// water bucket
	// lava bucket
	// minecart
	items[329] = &item_saddle;
	items[330] = &item_door_iron;
	items[331] = &item_redstone;
	items[332] = &item_snow_ball;
	// boat
	items[334] = &item_leather;
	// milk bucket
	items[336] = &item_brick;
	items[337] = &item_clay_ball;
	items[338] = &item_sugarcane;
	items[339] = &item_paper;
	items[340] = &item_book;
	items[341] = &item_slime_ball;
	// storage minecart
	// furnace minecart
	items[344] = &item_egg;
	// compass
	// fishing rod
	// clock
	items[348] = &item_glowstone_dust;
	items[349] = &item_fish;
	items[350] = &item_fish_cooked;
	items[351] = &item_dye;
	items[352] = &item_bone;
	items[353] = &item_sugar;
	items[354] = &item_cake;
	items[355] = &item_bed;
	// redstone repeater
	items[357] = &item_cookie;
	// map
	items[359] = &item_shears;

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
