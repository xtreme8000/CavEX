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
	ITEM_IRON_SHOVEL = 256,
	ITEM_IRON_PICKAXE = 257,
	ITEM_IRON_AXE = 258,
	ITEM_FLINT_STEEL = 259,
	ITEM_APPLE = 260,
	ITEM_BOW = 261,
	ITEM_ARROW = 262,
	ITEM_COAL = 263,
	ITEM_DIAMOND = 264,
	ITEM_IRON = 265,
	ITEM_GOLD = 266,
	ITEM_IRON_SWORD = 267,
	ITEM_WOOD_SWORD = 268,
	ITEM_WOOD_SHOVEL = 269,
	ITEM_WOOD_PICKAXE = 270,
	ITEM_WOOD_AXE = 271,
	ITEM_STONE_SWORD = 272,
	ITEM_STONE_SHOVEL = 273,
	ITEM_STONE_PICKAXE = 274,
	ITEM_STONE_AXE = 275,
	ITEM_DIAMOND_SWORD = 276,
	ITEM_DIAMOND_SHOVEL = 277,
	ITEM_DIAMOND_PICKAXE = 278,
	ITEM_DIAMOND_AXE = 279,
	ITEM_STICK = 280,
	ITEM_BOWL = 281,
	ITEM_MUSHROOM_STEW = 282,
	ITEM_GOLD_SWORD = 283,
	ITEM_GOLD_SHOVEL = 284,
	ITEM_GOLD_PICKAXE = 285,
	ITEM_GOLD_AXE = 286,
	ITEM_STRING = 287,
	ITEM_FEATHER = 288,
	ITEM_GUNPOWDER = 289,
	ITEM_WOOD_HOE = 290,
	ITEM_STONE_HOE = 291,
	ITEM_IRON_HOE = 292,
	ITEM_DIAMOND_HOE = 293,
	ITEM_GOLD_HOE = 294,
	ITEM_SEED = 295,
	ITEM_WHEAT = 296,
	ITEM_BREAD = 297,
	ITEM_LEATHER_HELMET = 298,
	ITEM_LEATHER_CHESTPLATE = 299,
	ITEM_LEATHER_LEGGINGS = 300,
	ITEM_LEATHER_BOOTS = 301,
	ITEM_CHAIN_HELMET = 302,
	ITEM_CHAIN_CHESTPLATE = 303,
	ITEM_CHAIN_LEGGINGS = 304,
	ITEM_CHAIN_BOOTS = 305,
	ITEM_IRON_HELMET = 306,
	ITEM_IRON_CHESTPLATE = 307,
	ITEM_IRON_LEGGINGS = 308,
	ITEM_IRON_BOOTS = 309,
	ITEM_DIAMOND_HELMET = 310,
	ITEM_DIAMOND_CHESTPLATE = 311,
	ITEM_DIAMOND_LEGGINGS = 312,
	ITEM_DIAMOND_BOOTS = 313,
	ITEM_GOLD_HELMET = 314,
	ITEM_GOLD_CHESTPLATE = 315,
	ITEM_GOLD_LEGGINGS = 316,
	ITEM_GOLD_BOOTS = 317,
	ITEM_FLINT = 318,
	ITEM_PORKCHOP = 319,
	ITEM_PORKCHOP_COOKED = 320,
	ITEM_APPLE_GOLDEN = 322,
	ITEM_DOOR_WOOD = 324,
	ITEM_SADDLE = 329,
	ITEM_DOOR_IRON = 330,
	ITEM_REDSTONE = 331,
	ITEM_SNOW_BALL = 332,
	ITEM_LEATHER = 334,
	ITEM_BRICK = 336,
	ITEM_CLAY_BALL = 337,
	ITEM_REED = 338,
	ITEM_PAPER = 339,
	ITEM_BOOK = 340,
	ITEM_GLOWSTONE_DUST = 348,
	ITEM_FISH = 349,
	ITEM_FISH_COOKED = 350,
	ITEM_DYE = 351,
	ITEM_SUGAR = 353,
	ITEM_BED = 355,
	ITEM_SHEARS = 359,
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
