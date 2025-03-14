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

static void add_tools() {
	// wood
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_WOOD_SWORD, .durability = 0, .count = 1},
		1, 3, (uint8_t[]) {1, 1, 2}, (struct item_data) {.id = BLOCK_PLANKS},
		false, (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_WOOD_PICKAXE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 0, 2, 0, 0, 2, 0},
			   (struct item_data) {.id = BLOCK_PLANKS}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_WOOD_SHOVEL, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 2, 2},
			   (struct item_data) {.id = BLOCK_PLANKS}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_WOOD_AXE, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 1, 2, 0, 2},
		(struct item_data) {.id = BLOCK_PLANKS}, false,
		(struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_WOOD_HOE, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 0, 2, 0, 2},
		(struct item_data) {.id = BLOCK_PLANKS}, false,
		(struct item_data) {.id = ITEM_STICK}, false);

	// stone
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_STONE_SWORD, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 1, 2},
			   (struct item_data) {.id = BLOCK_COBBLESTONE}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_STONE_PICKAXE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 0, 2, 0, 0, 2, 0},
			   (struct item_data) {.id = BLOCK_COBBLESTONE}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_STONE_SHOVEL, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 2, 2},
			   (struct item_data) {.id = BLOCK_COBBLESTONE}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_STONE_AXE, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 1, 2, 0, 2},
		(struct item_data) {.id = BLOCK_COBBLESTONE}, false,
		(struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_STONE_HOE, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 0, 2, 0, 2},
		(struct item_data) {.id = BLOCK_COBBLESTONE}, false,
		(struct item_data) {.id = ITEM_STICK}, false);

	// gold
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_GOLD_SWORD, .durability = 0, .count = 1},
		1, 3, (uint8_t[]) {1, 1, 2}, (struct item_data) {.id = ITEM_GOLD},
		false, (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_GOLD_PICKAXE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 0, 2, 0, 0, 2, 0},
			   (struct item_data) {.id = ITEM_GOLD}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_GOLD_SHOVEL, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 2, 2},
			   (struct item_data) {.id = ITEM_GOLD}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_GOLD_AXE, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 1, 2, 0, 2},
		(struct item_data) {.id = ITEM_GOLD}, false,
		(struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_GOLD_HOE, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 0, 2, 0, 2},
		(struct item_data) {.id = ITEM_GOLD}, false,
		(struct item_data) {.id = ITEM_STICK}, false);

	// iron
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_IRON_SWORD, .durability = 0, .count = 1},
		1, 3, (uint8_t[]) {1, 1, 2}, (struct item_data) {.id = ITEM_IRON},
		false, (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_IRON_PICKAXE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 0, 2, 0, 0, 2, 0},
			   (struct item_data) {.id = ITEM_IRON}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_IRON_SHOVEL, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 2, 2},
			   (struct item_data) {.id = ITEM_IRON}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_IRON_AXE, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 1, 2, 0, 2},
		(struct item_data) {.id = ITEM_IRON}, false,
		(struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_IRON_HOE, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 0, 2, 0, 2},
		(struct item_data) {.id = ITEM_IRON}, false,
		(struct item_data) {.id = ITEM_STICK}, false);

	// diamond
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_SWORD, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 1, 2},
			   (struct item_data) {.id = ITEM_DIAMOND}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_PICKAXE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 0, 2, 0, 0, 2, 0},
			   (struct item_data) {.id = ITEM_DIAMOND}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_SHOVEL, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 2, 2},
			   (struct item_data) {.id = ITEM_DIAMOND}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_AXE, .durability = 0, .count = 1},
			   2, 3, (uint8_t[]) {1, 1, 1, 2, 0, 2},
			   (struct item_data) {.id = ITEM_DIAMOND}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_HOE, .durability = 0, .count = 1},
			   2, 3, (uint8_t[]) {1, 1, 0, 2, 0, 2},
			   (struct item_data) {.id = ITEM_DIAMOND}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
}

static void add_armor() {
	// leather
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_LEATHER_HELMET, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 1, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_LEATHER}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_LEATHER_CHESTPLATE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 0, 1, 1, 1, 1, 1, 1, 1},
			   (struct item_data) {.id = ITEM_LEATHER}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_LEATHER_LEGGINGS, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 1, 0, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_LEATHER}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_LEATHER_BOOTS, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 0, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_LEATHER}, false);

	// chain
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_CHAIN_HELMET, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 1, 1, 1, 0, 1},
			   (struct item_data) {.id = BLOCK_FIRE}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_CHAIN_CHESTPLATE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 0, 1, 1, 1, 1, 1, 1, 1},
			   (struct item_data) {.id = BLOCK_FIRE}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_CHAIN_LEGGINGS, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 1, 0, 1, 1, 0, 1},
			   (struct item_data) {.id = BLOCK_FIRE}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_CHAIN_BOOTS, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 0, 1, 1, 0, 1},
			   (struct item_data) {.id = BLOCK_FIRE}, false);

	// iron
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_IRON_HELMET, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 1, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_IRON}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_IRON_CHESTPLATE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 0, 1, 1, 1, 1, 1, 1, 1},
			   (struct item_data) {.id = ITEM_IRON}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_IRON_LEGGINGS, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 1, 0, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_IRON}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_IRON_BOOTS, .durability = 0, .count = 1},
		3, 2, (uint8_t[]) {1, 0, 1, 1, 0, 1},
		(struct item_data) {.id = ITEM_IRON}, false);

	// gold
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_GOLD_HELMET, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 1, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_GOLD}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_GOLD_CHESTPLATE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 0, 1, 1, 1, 1, 1, 1, 1},
			   (struct item_data) {.id = ITEM_GOLD}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_GOLD_LEGGINGS, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 1, 0, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_GOLD}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_GOLD_BOOTS, .durability = 0, .count = 1},
		3, 2, (uint8_t[]) {1, 0, 1, 1, 0, 1},
		(struct item_data) {.id = ITEM_GOLD}, false);

	// diamond
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_HELMET, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 1, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_DIAMOND}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_CHESTPLATE, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 0, 1, 1, 1, 1, 1, 1, 1},
			   (struct item_data) {.id = ITEM_DIAMOND}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_LEGGINGS, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 1, 0, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_DIAMOND}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_DIAMOND_BOOTS, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 0, 1, 1, 0, 1},
			   (struct item_data) {.id = ITEM_DIAMOND}, false);
}

void recipe_init() {
	array_recipe_init(recipes_crafting);

	recipe_add(recipes_crafting,
			   (struct item_data) {.id = ITEM_BOW, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {0, 1, 2, 1, 0, 2, 0, 1, 2},
			   (struct item_data) {.id = ITEM_STICK}, false,
			   (struct item_data) {.id = ITEM_STRING}, false);

	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_GOLD_CAST, .durability = 0, .count = 1},
		3, 3, (uint8_t[]) {1, 1, 1, 1, 1, 1, 1, 1, 1},
		(struct item_data) {.id = ITEM_GOLD}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_IRON_CAST, .durability = 0, .count = 1},
		3, 3, (uint8_t[]) {1, 1, 1, 1, 1, 1, 1, 1, 1},
		(struct item_data) {.id = ITEM_IRON}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_DIAMOND_CAST, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 1, 1, 1, 1, 1, 1},
			   (struct item_data) {.id = ITEM_DIAMOND}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_LAPIS_CAST, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 1, 1, 1, 1, 1, 1},
			   (struct item_data) {.id = ITEM_DYE, .durability = 4}, true);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_CHEST, .durability = 0, .count = 1}, 3,
		3, (uint8_t[]) {1, 1, 1, 1, 0, 1, 1, 1, 1},
		(struct item_data) {.id = BLOCK_PLANKS}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_FURNACE, .durability = 0, .count = 1},
		3, 3, (uint8_t[]) {1, 1, 1, 1, 0, 1, 1, 1, 1},
		(struct item_data) {.id = BLOCK_COBBLESTONE}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_JUKEBOX, .durability = 0, .count = 1},
		3, 3, (uint8_t[]) {1, 1, 1, 1, 2, 1, 1, 1, 1},
		(struct item_data) {.id = BLOCK_PLANKS}, false,
		(struct item_data) {.id = ITEM_DIAMOND}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_NOTEBLOCK, .durability = 0, .count = 1},
		3, 3, (uint8_t[]) {1, 1, 1, 1, 2, 1, 1, 1, 1},
		(struct item_data) {.id = BLOCK_PLANKS}, false,
		(struct item_data) {.id = ITEM_REDSTONE}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_BOOKSHELF, .durability = 0, .count = 1},
		3, 3, (uint8_t[]) {1, 1, 1, 2, 2, 2, 1, 1, 1},
		(struct item_data) {.id = BLOCK_PLANKS}, false,
		(struct item_data) {.id = ITEM_BOOK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_TNT, .durability = 0, .count = 1}, 3, 3,
		(uint8_t[]) {1, 2, 1, 2, 1, 2, 1, 2, 1},
		(struct item_data) {.id = ITEM_GUNPOWDER}, false,
		(struct item_data) {.id = BLOCK_SAND}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_LADDER, .durability = 0, .count = 2}, 3,
		3, (uint8_t[]) {1, 0, 1, 1, 1, 1, 1, 0, 1},
		(struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_RAIL, .durability = 0, .count = 16}, 3,
		3, (uint8_t[]) {1, 0, 1, 1, 2, 1, 1, 0, 1},
		(struct item_data) {.id = ITEM_IRON}, false,
		(struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_POWERED_RAIL, .durability = 0, .count = 6},
			   3, 3, (uint8_t[]) {1, 0, 1, 1, 2, 1, 1, 3, 1},
			   (struct item_data) {.id = ITEM_GOLD}, false,
			   (struct item_data) {.id = ITEM_STICK}, false,
			   (struct item_data) {.id = ITEM_REDSTONE}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_DETECTOR_RAIL, .durability = 0, .count = 6},
			   3, 3, (uint8_t[]) {1, 0, 1, 1, 2, 1, 1, 3, 1},
			   (struct item_data) {.id = ITEM_IRON}, false,
			   (struct item_data) {.id = BLOCK_STONE_PRESSURE_PLATE}, false,
			   (struct item_data) {.id = ITEM_REDSTONE}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_WOODEN_STAIRS, .durability = 0, .count = 4},
			   3, 3, (uint8_t[]) {1, 0, 0, 1, 1, 0, 1, 1, 1},
			   (struct item_data) {.id = BLOCK_PLANKS}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_STONE_STAIRS, .durability = 0, .count = 4},
			   3, 3, (uint8_t[]) {1, 0, 0, 1, 1, 0, 1, 1, 1},
			   (struct item_data) {.id = BLOCK_COBBLESTONE}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_APPLE_GOLDEN, .durability = 0, .count = 1},
			   3, 3, (uint8_t[]) {1, 1, 1, 1, 2, 1, 1, 1, 1},
			   (struct item_data) {.id = BLOCK_GOLD_CAST}, false,
			   (struct item_data) {.id = ITEM_APPLE}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_DISPENSER, .durability = 0, .count = 1},
		3, 3, (uint8_t[]) {1, 1, 1, 1, 2, 1, 1, 3, 1},
		(struct item_data) {.id = BLOCK_COBBLESTONE}, false,
		(struct item_data) {.id = ITEM_BOW}, false,
		(struct item_data) {.id = ITEM_REDSTONE}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_FENCE, .durability = 0, .count = 2}, 3,
		2, (uint8_t[]) {1, 1, 1, 1, 1, 1},
		(struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_DOOR_WOOD, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 1, 1, 1, 1},
		(struct item_data) {.id = BLOCK_PLANKS}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_TRAP_DOOR, .durability = 0, .count = 2},
		3, 2, (uint8_t[]) {1, 1, 1, 1, 1, 1},
		(struct item_data) {.id = BLOCK_PLANKS}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_DOOR_IRON, .durability = 0, .count = 1},
		2, 3, (uint8_t[]) {1, 1, 1, 1, 1, 1},
		(struct item_data) {.id = ITEM_IRON}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_BOWL, .durability = 0, .count = 4}, 3, 2,
		(uint8_t[]) {1, 0, 1, 0, 1, 0}, (struct item_data) {.id = BLOCK_PLANKS},
		false);
	recipe_add(recipes_crafting,
			   (struct item_data) {.id = ITEM_BED, .durability = 0, .count = 1},
			   3, 2, (uint8_t[]) {1, 1, 1, 2, 2, 2},
			   (struct item_data) {.id = BLOCK_WOOL}, false,
			   (struct item_data) {.id = BLOCK_PLANKS}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_SHEARS, .durability = 0, .count = 1}, 2,
		2, (uint8_t[]) {0, 1, 1, 0}, (struct item_data) {.id = ITEM_IRON},
		false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_WORKBENCH, .durability = 0, .count = 1},
		2, 2, (uint8_t[]) {1, 1, 1, 1}, (struct item_data) {.id = BLOCK_PLANKS},
		false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_SANDSTONE, .durability = 0, .count = 1},
		2, 2, (uint8_t[]) {1, 1, 1, 1}, (struct item_data) {.id = BLOCK_SAND},
		false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_SNOW_BLOCK, .durability = 0, .count = 1},
			   2, 2, (uint8_t[]) {1, 1, 1, 1},
			   (struct item_data) {.id = ITEM_SNOW_BALL}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_CLAY, .durability = 0, .count = 1}, 2,
		2, (uint8_t[]) {1, 1, 1, 1}, (struct item_data) {.id = ITEM_CLAY_BALL},
		false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_BRICKS, .durability = 0, .count = 1}, 2,
		2, (uint8_t[]) {1, 1, 1, 1}, (struct item_data) {.id = ITEM_BRICK},
		false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_GLOWSTONE, .durability = 0, .count = 1},
		2, 2, (uint8_t[]) {1, 1, 1, 1},
		(struct item_data) {.id = ITEM_GLOWSTONE_DUST}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_WOOL, .durability = 0, .count = 1}, 2,
		2, (uint8_t[]) {1, 1, 1, 1}, (struct item_data) {.id = ITEM_STRING},
		false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_FLINT_STEEL, .durability = 0, .count = 1},
			   2, 2, (uint8_t[]) {1, 0, 0, 2},
			   (struct item_data) {.id = ITEM_IRON}, false,
			   (struct item_data) {.id = ITEM_FLINT}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_ARROW, .durability = 0, .count = 4}, 1,
		3, (uint8_t[]) {1, 2, 3}, (struct item_data) {.id = ITEM_FLINT}, false,
		(struct item_data) {.id = ITEM_STICK}, false,
		(struct item_data) {.id = ITEM_FEATHER}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_MUSHROOM_STEW, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 2, 3},
			   (struct item_data) {.id = BLOCK_RED_MUSHROOM}, false,
			   (struct item_data) {.id = BLOCK_BROWM_MUSHROOM}, false,
			   (struct item_data) {.id = ITEM_BOWL}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = ITEM_MUSHROOM_STEW, .durability = 0, .count = 1},
			   1, 3, (uint8_t[]) {1, 2, 3},
			   (struct item_data) {.id = BLOCK_BROWM_MUSHROOM}, false,
			   (struct item_data) {.id = BLOCK_RED_MUSHROOM}, false,
			   (struct item_data) {.id = ITEM_BOWL}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_PAPER, .durability = 0, .count = 3}, 3,
		1, (uint8_t[]) {1, 1, 1}, (struct item_data) {.id = ITEM_REED}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_BOOK, .durability = 0, .count = 1}, 1, 3,
		(uint8_t[]) {1, 1, 1}, (struct item_data) {.id = ITEM_PAPER}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_SLAB, .durability = 3, .count = 3}, 3,
		1, (uint8_t[]) {1, 1, 1}, (struct item_data) {.id = BLOCK_COBBLESTONE},
		false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_SLAB, .durability = 0, .count = 3}, 3,
		1, (uint8_t[]) {1, 1, 1}, (struct item_data) {.id = BLOCK_STONE},
		false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_SLAB, .durability = 1, .count = 3}, 3,
		1, (uint8_t[]) {1, 1, 1}, (struct item_data) {.id = BLOCK_SANDSTONE},
		false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_SLAB, .durability = 2, .count = 3}, 3,
		1, (uint8_t[]) {1, 1, 1}, (struct item_data) {.id = BLOCK_PLANKS},
		false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_BREAD, .durability = 0, .count = 1}, 3,
		1, (uint8_t[]) {1, 1, 1}, (struct item_data) {.id = ITEM_WHEAT}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_STICK, .durability = 0, .count = 4}, 1,
		2, (uint8_t[]) {1, 1}, (struct item_data) {.id = BLOCK_PLANKS}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_TORCH, .durability = 0, .count = 4}, 1,
		2, (uint8_t[]) {1, 2}, (struct item_data) {.id = ITEM_COAL}, false,
		(struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_PUMPKIN_LIT, .durability = 0, .count = 1},
			   1, 2, (uint8_t[]) {1, 2},
			   (struct item_data) {.id = BLOCK_PUMPKIN}, false,
			   (struct item_data) {.id = BLOCK_TORCH}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {
				   .id = BLOCK_REDSTONE_TORCH, .durability = 0, .count = 1},
			   1, 2, (uint8_t[]) {1, 2},
			   (struct item_data) {.id = ITEM_REDSTONE}, false,
			   (struct item_data) {.id = ITEM_STICK}, false);
	recipe_add(recipes_crafting,
			   (struct item_data) {.id = BLOCK_STONE_PRESSURE_PLATE,
								   .durability = 0,
								   .count = 1},
			   2, 1, (uint8_t[]) {1, 1}, (struct item_data) {.id = BLOCK_STONE},
			   false);
	recipe_add(recipes_crafting,
			   (struct item_data) {.id = BLOCK_WOOD_PRESSURE_PLATE,
								   .durability = 0,
								   .count = 1},
			   2, 1, (uint8_t[]) {1, 1},
			   (struct item_data) {.id = BLOCK_PLANKS}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_GOLD, .durability = 0, .count = 9}, 1, 1,
		(uint8_t[]) {1}, (struct item_data) {.id = BLOCK_GOLD_CAST}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_IRON, .durability = 0, .count = 9}, 1, 1,
		(uint8_t[]) {1}, (struct item_data) {.id = BLOCK_IRON_CAST}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_DIAMOND, .durability = 0, .count = 9}, 1,
		1, (uint8_t[]) {1}, (struct item_data) {.id = BLOCK_DIAMOND_CAST},
		false);
	recipe_add(recipes_crafting,
			   (struct item_data) {.id = ITEM_DYE, .durability = 4, .count = 9},
			   1, 1, (uint8_t[]) {1},
			   (struct item_data) {.id = BLOCK_LAPIS_CAST}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = ITEM_SUGAR, .durability = 0, .count = 1}, 1,
		1, (uint8_t[]) {1}, (struct item_data) {.id = ITEM_REED}, false);
	recipe_add(
		recipes_crafting,
		(struct item_data) {.id = BLOCK_PLANKS, .durability = 0, .count = 4}, 1,
		1, (uint8_t[]) {1}, (struct item_data) {.id = BLOCK_LOG}, false);

	add_tools();
	add_armor();
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
				bool match_outside_empty = true;

				// check that outside of pattern is empty
				for(size_t py = 0; py < 3 && match_outside_empty; py++) {
					for(size_t px = 0; px < 3 && match_outside_empty; px++) {
						if((px < x || px >= x + current->width || py < y
							|| py >= y + current->height)
						   && !slot_empty[px + py * 3])
							match_outside_empty = false;
					}
				}

				// check pattern itself
				for(int flip = 0; flip <= 1; flip++) {
					bool match = match_outside_empty;

					for(size_t py = 0; py < current->height && match; py++) {
						for(size_t px = 0; px < current->width && match; px++) {
							size_t slots_idx = (px + x) + (py + y) * 3;
							size_t shape_idx
								= (flip ? (current->width - 1 - px) : px)
								+ py * current->width;

							if(current->shape[shape_idx].item.id == 0) {
								if(!slot_empty[slots_idx])
									match = false;
							} else {
								if(slot_empty[slots_idx]
								   || current->shape[shape_idx].item.id
									   != slots[slots_idx].id
								   || (current->shape->match_durability
									   && current->shape[shape_idx]
											   .item.durability
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
		}

		array_recipe_next(it);
	}

	return false;
}