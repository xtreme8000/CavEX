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

#ifndef ITEMS_H
#define ITEMS_H

#include <stdbool.h>
#include <stdint.h>

#include "../cglm/cglm.h"

struct item_data {
	uint16_t id;
	uint16_t durability;
	uint8_t count;
};

enum item_type {
	ITEM_COAL = 263,
	ITEM_DIAMOND = 264,
	ITEM_DIAMOND_PICKAXE = 278,
	ITEM_STICK = 280,
	ITEM_SEED = 295,
	ITEM_WHEAT = 296,
	ITEM_BREAD = 297,
	ITEM_STRING = 287,
	ITEM_FLINT = 318,
	ITEM_REDSTONE = 331,
	ITEM_SNOW_BALL = 332,
	ITEM_CLAY_BALL = 337,
	ITEM_REED = 338,
	ITEM_GLOWSTONE_DUST = 348,
	ITEM_DYE = 351,
};

#include "../block/blocks_data.h"
#include "../item/tool.h"
struct server_local;

enum armor_type {
	ARMOR_TYPE_HELMET = 0,
	ARMOR_TYPE_CHESTPLATE = 1,
	ARMOR_TYPE_LEGGINGS = 2,
	ARMOR_TYPE_BOOTS = 3,
};

enum armor_tier {
	ARMOR_TIER_BLOCK,
	ARMOR_TIER_CHAIN,
	ARMOR_TIER_CLOTH,
	ARMOR_TIER_GOLD,
	ARMOR_TIER_IRON,
	ARMOR_TIER_DIAMOND,
};

enum render_item_env {
	R_ITEM_ENV_INVENTORY,
	R_ITEM_ENV_FIRSTPERSON,
	R_ITEM_ENV_THIRDPERSON,
	R_ITEM_ENV_ENTITY,
};

struct item {
	char name[32];
	bool has_damage;
	uint16_t max_damage;
	uint8_t max_stack;
	void (*renderItem)(struct item*, struct item_data*, mat4, bool,
					   enum render_item_env);
	bool (*onItemPlace)(struct server_local*, struct item_data*,
						struct block_info*, struct block_info*, enum side);
	struct item_armor {
		bool is_armor;
		enum armor_type type;
		enum armor_tier tier;
	} armor;
	struct item_tool {
		enum tool_type type;
		enum tool_tier tier;
	} tool;
	union {
		struct {
			uint8_t texture_x : 4;
			uint8_t texture_y : 4;
		} item;
		struct {
			bool has_default;
			uint8_t default_metadata : 4;
			uint8_t default_rotation : 2;
		} block;
	} render_data;
};

#define ITEMS_MAX 512

extern struct item* items[ITEMS_MAX];

void items_init(void);
struct item* item_get(struct item_data* item);
bool item_is_block(struct item_data* item);

#endif
