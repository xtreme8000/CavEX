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

#include "items.h"

static struct item item_iron_sword = {
	.name = "Iron Sword",
	.has_damage = true,
	.max_damage = 251,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SWORD,
	.tool.tier = TOOL_TIER_IRON,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 4,
		},
	},
};

static struct item item_iron_shovel = {
	.name = "Iron Shovel",
	.has_damage = true,
	.max_damage = 251,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SHOVEL,
	.tool.tier = TOOL_TIER_IRON,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 5,
		},
	},
};

static struct item item_iron_pickaxe = {
	.name = "Iron Pickaxe",
	.has_damage = true,
	.max_damage = 251,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_PICKAXE,
	.tool.tier = TOOL_TIER_IRON,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 6,
		},
	},
};

static struct item item_iron_axe = {
	.name = "Iron Axe",
	.has_damage = true,
	.max_damage = 251,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_AXE,
	.tool.tier = TOOL_TIER_IRON,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 7,
		},
	},
};

static struct item item_iron_hoe = {
	.name = "Iron Hoe",
	.has_damage = true,
	.max_damage = 251,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_HOE,
	.tool.tier = TOOL_TIER_IRON,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 8,
		},
	},
};

static struct item item_wood_sword = {
	.name = "Wooden Sword",
	.has_damage = true,
	.max_damage = 60,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SWORD,
	.tool.tier = TOOL_TIER_WOOD,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 4,
		},
	},
};

static struct item item_wood_shovel = {
	.name = "Wooden Shovel",
	.has_damage = true,
	.max_damage = 60,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SHOVEL,
	.tool.tier = TOOL_TIER_WOOD,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 5,
		},
	},
};

static struct item item_wood_pickaxe = {
	.name = "Wooden Pickaxe",
	.has_damage = true,
	.max_damage = 60,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_PICKAXE,
	.tool.tier = TOOL_TIER_WOOD,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 6,
		},
	},
};

static struct item item_wood_axe = {
	.name = "Wooden Axe",
	.has_damage = true,
	.max_damage = 60,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_AXE,
	.tool.tier = TOOL_TIER_WOOD,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 7,
		},
	},
};

static struct item item_wood_hoe = {
	.name = "Wooden Hoe",
	.has_damage = true,
	.max_damage = 60,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_HOE,
	.tool.tier = TOOL_TIER_WOOD,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 8,
		},
	},
};

static struct item item_stone_sword = {
	.name = "Stone Sword",
	.has_damage = true,
	.max_damage = 132,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SWORD,
	.tool.tier = TOOL_TIER_STONE,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 4,
		},
	},
};

static struct item item_stone_shovel = {
	.name = "Stone Shovel",
	.has_damage = true,
	.max_damage = 132,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SHOVEL,
	.tool.tier = TOOL_TIER_STONE,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 5,
		},
	},
};

static struct item item_stone_pickaxe = {
	.name = "Stone Pickaxe",
	.has_damage = true,
	.max_damage = 132,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_PICKAXE,
	.tool.tier = TOOL_TIER_STONE,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 6,
		},
	},
};

static struct item item_stone_axe = {
	.name = "Stone Axe",
	.has_damage = true,
	.max_damage = 132,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_AXE,
	.tool.tier = TOOL_TIER_STONE,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 7,
		},
	},
};

static struct item item_stone_hoe = {
	.name = "Stone Hoe",
	.has_damage = true,
	.max_damage = 132,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_HOE,
	.tool.tier = TOOL_TIER_STONE,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 8,
		},
	},
};

static struct item item_diamond_sword = {
	.name = "Diamond Sword",
	.has_damage = true,
	.max_damage = 1562,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SWORD,
	.tool.tier = TOOL_TIER_DIAMOND,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 4,
		},
	},
};

static struct item item_diamond_shovel = {
	.name = "Diamond Shovel",
	.has_damage = true,
	.max_damage = 1562,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SHOVEL,
	.tool.tier = TOOL_TIER_DIAMOND,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 5,
		},
	},
};

static struct item item_diamond_pickaxe = {
	.name = "Diamond Pickaxe",
	.has_damage = true,
	.max_damage = 1562,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_PICKAXE,
	.tool.tier = TOOL_TIER_DIAMOND,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 6,
		},
	},
};

static struct item item_diamond_axe = {
	.name = "Diamond Axe",
	.has_damage = true,
	.max_damage = 1562,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_AXE,
	.tool.tier = TOOL_TIER_DIAMOND,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 7,
		},
	},
};

static struct item item_diamond_hoe = {
	.name = "Diamond Hoe",
	.has_damage = true,
	.max_damage = 1562,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_HOE,
	.tool.tier = TOOL_TIER_DIAMOND,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 8,
		},
	},
};

static struct item item_gold_sword = {
	.name = "Gold Sword",
	.has_damage = true,
	.max_damage = 33,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SWORD,
	.tool.tier = TOOL_TIER_GOLD,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 4,
		},
	},
};

static struct item item_gold_shovel = {
	.name = "Gold Shovel",
	.has_damage = true,
	.max_damage = 33,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_SHOVEL,
	.tool.tier = TOOL_TIER_GOLD,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 5,
		},
	},
};

static struct item item_gold_pickaxe = {
	.name = "Gold Pickaxe",
	.has_damage = true,
	.max_damage = 33,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_PICKAXE,
	.tool.tier = TOOL_TIER_GOLD,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 6,
		},
	},
};

static struct item item_gold_axe = {
	.name = "Gold Axe",
	.has_damage = true,
	.max_damage = 33,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_AXE,
	.tool.tier = TOOL_TIER_GOLD,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 7,
		},
	},
};

static struct item item_gold_hoe = {
	.name = "Gold Hoe",
	.has_damage = true,
	.max_damage = 33,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_HOE,
	.tool.tier = TOOL_TIER_GOLD,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 8,
		},
	},
};

static struct item item_flint_steel = {
	.name = "Flint and Steel",
	.has_damage = true,
	.max_damage = 65,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 5,
			.texture_y = 0,
		},
	},
};

static struct item item_apple = {
	.name = "Apple",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 10,
			.texture_y = 0,
		},
	},
};

static struct item item_bow = {
	.name = "Bow",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 5,
			.texture_y = 1,
		},
	},
};

static struct item item_arrow = {
	.name = "Arrow",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 5,
			.texture_y = 2,
		},
	},
};

static struct item item_coal = {
	.name = "Coal",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 7,
			.texture_y = 0,
		},
	},
};

static struct item item_diamond = {
	.name = "Diamond",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 7,
			.texture_y = 3,
		},
	},
};

static struct item item_iron = {
	.name = "Iron Ingot",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 7,
			.texture_y = 1,
		},
	},
};

static struct item item_gold = {
	.name = "Gold Ingot",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 7,
			.texture_y = 2,
		},
	},
};

static struct item item_stick = {
	.name = "Stick",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 5,
			.texture_y = 3,
		},
	},
};

static struct item item_diamond_helmet = {
	.name = "Diamond Helmet",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_HELMET,
	.armor.tier = ARMOR_TIER_DIAMOND,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 0,
		},
	},
};

static struct item item_diamond_chestplate = {
	.name = "Diamond Chestplate",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_CHESTPLATE,
	.armor.tier = ARMOR_TIER_DIAMOND,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 1,
		},
	},
};

static struct item item_diamond_leggings = {
	.name = "Diamond Leggings",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_LEGGINGS,
	.armor.tier = ARMOR_TIER_DIAMOND,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 2,
		},
	},
};

static struct item item_diamond_boots = {
	.name = "Diamond Boots",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_BOOTS,
	.armor.tier = ARMOR_TIER_DIAMOND,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 3,
			.texture_y = 3,
		},
	},
};

static struct item item_leather_helmet = {
	.name = "Leather Helmet",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_HELMET,
	.armor.tier = ARMOR_TIER_CLOTH,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 0,
		},
	},
};

static struct item item_leather_chestplate = {
	.name = "Leather Chestplate",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_CHESTPLATE,
	.armor.tier = ARMOR_TIER_CLOTH,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 1,
		},
	},
};

static struct item item_leather_leggings = {
	.name = "Leather Leggings",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_LEGGINGS,
	.armor.tier = ARMOR_TIER_CLOTH,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 2,
		},
	},
};

static struct item item_leather_boots = {
	.name = "Leather Boots",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_BOOTS,
	.armor.tier = ARMOR_TIER_CLOTH,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 0,
			.texture_y = 3,
		},
	},
};

static struct item item_chain_helmet = {
	.name = "Chain Helmet",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_HELMET,
	.armor.tier = ARMOR_TIER_CHAIN,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 0,
		},
	},
};

static struct item item_chain_chestplate = {
	.name = "Chain Chestplate",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_CHESTPLATE,
	.armor.tier = ARMOR_TIER_CHAIN,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 1,
		},
	},
};

static struct item item_chain_leggings = {
	.name = "Chain Leggings",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_LEGGINGS,
	.armor.tier = ARMOR_TIER_CHAIN,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 2,
		},
	},
};

static struct item item_chain_boots = {
	.name = "Chain Boots",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_BOOTS,
	.armor.tier = ARMOR_TIER_CHAIN,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 1,
			.texture_y = 3,
		},
	},
};

static struct item item_iron_helmet = {
	.name = "Iron Helmet",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_HELMET,
	.armor.tier = ARMOR_TIER_IRON,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 0,
		},
	},
};

static struct item item_iron_chestplate = {
	.name = "Iron Chestplate",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_CHESTPLATE,
	.armor.tier = ARMOR_TIER_IRON,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 1,
		},
	},
};

static struct item item_iron_leggings = {
	.name = "Iron Leggings",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_LEGGINGS,
	.armor.tier = ARMOR_TIER_IRON,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 2,
		},
	},
};

static struct item item_iron_boots = {
	.name = "Iron Boots",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_BOOTS,
	.armor.tier = ARMOR_TIER_IRON,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 2,
			.texture_y = 3,
		},
	},
};

static struct item item_gold_helmet = {
	.name = "Gold Helmet",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_HELMET,
	.armor.tier = ARMOR_TIER_GOLD,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 0,
		},
	},
};

static struct item item_gold_chestplate = {
	.name = "Gold Chestplate",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_CHESTPLATE,
	.armor.tier = ARMOR_TIER_GOLD,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 1,
		},
	},
};

static struct item item_gold_leggings = {
	.name = "Gold Leggings",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_LEGGINGS,
	.armor.tier = ARMOR_TIER_GOLD,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 2,
		},
	},
};

static struct item item_gold_boots = {
	.name = "Gold Boots",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = true,
	.armor.type = ARMOR_TYPE_BOOTS,
	.armor.tier = ARMOR_TIER_GOLD,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 4,
			.texture_y = 3,
		},
	},
};

static struct item item_porkchop = {
	.name = "Raw Porkchop",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 7,
			.texture_y = 5,
		},
	},
};

static struct item item_porkchop_cooked = {
	.name = "Cooked Porkchop",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 8,
			.texture_y = 5,
		},
	},
};

static struct item item_seeds = {
	.name = "Seeds",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 9,
			.texture_y = 0,
		},
	},
};

static struct item item_wheat = {
	.name = "Wheat",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 9,
			.texture_y = 1,
		},
	},
};

static struct item item_bread = {
	.name = "Bread",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 9,
			.texture_y = 2,
		},
	},
};

static struct item item_flint = {
	.name = "Flint",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 6,
			.texture_y = 0,
		},
	},
};

static struct item item_string = {
	.name = "String",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 8,
			.texture_y = 0,
		},
	},
};

static struct item item_redstone = {
	.name = "Redstone",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 8,
			.texture_y = 3,
		},
	},
};

static struct item item_snow_ball = {
	.name = "Snowball",
	.has_damage = false,
	.max_stack = 16,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 14,
			.texture_y = 0,
		},
	},
};

static struct item item_clay_ball = {
	.name = "Clay",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 9,
			.texture_y = 3,
		},
	},
};

static struct item item_glowstone_dust = {
	.name = "Glowstone Dust",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 9,
			.texture_y = 4,
		},
	},
};

static struct item item_dye = {
	.name = "Ink Sac",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 14,
			.texture_y = 4,
		},
	},
};

static struct item item_bowl = {
	.name = "Bowl",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 7,
			.texture_y = 4,
		},
	},
};

static struct item item_mushroom_stew = {
	.name = "Mushroom Stew",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 8,
			.texture_y = 4,
		},
	},
};

static struct item item_feather = {
	.name = "Feather",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 8,
			.texture_y = 1,
		},
	},
};

static struct item item_gunpowder = {
	.name = "Gunpowder",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 8,
			.texture_y = 2,
		},
	},
};

static struct item item_leather = {
	.name = "Leather",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 7,
			.texture_y = 6,
		},
	},
};

static struct item item_brick = {
	.name = "Brick",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 6,
			.texture_y = 1,
		},
	},
};

static struct item item_paper = {
	.name = "Paper",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 10,
			.texture_y = 3,
		},
	},
};

static struct item item_book = {
	.name = "Book",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 11,
			.texture_y = 3,
		},
	},
};

static struct item item_sugar = {
	.name = "Sugar",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 13,
			.texture_y = 0,
		},
	},
};

static struct item item_cake = {
	.name = "Cake",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 13,
			.texture_y = 1,
		},
	},
};

static struct item item_bed = {
	.name = "Bed",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 13,
			.texture_y = 2,
		},
	},
};

static struct item item_shears = {
	.name = "Shears",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 13,
			.texture_y = 5,
		},
	},
};

static struct item item_fish = {
	.name = "Raw Fish",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 9,
			.texture_y = 5,
		},
	},
};

static struct item item_fish_cooked = {
	.name = "Cooked Fish",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 10,
			.texture_y = 5,
		},
	},
};

static struct item item_cookie = {
	.name = "Cookie",
	.has_damage = false,
	.max_stack = 8,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 12,
			.texture_y = 5,
		},
	},
};

static struct item item_apple_golden = {
	.name = "Golden Apple",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 11,
			.texture_y = 0,
		},
	},
};

static struct item item_slime_ball = {
	.name = "Slimeball",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 14,
			.texture_y = 1,
		},
	},
};

static struct item item_egg = {
	.name = "Egg",
	.has_damage = false,
	.max_stack = 16,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 12,
			.texture_y = 0,
		},
	},
};

static struct item item_bone = {
	.name = "Bone",
	.has_damage = false,
	.max_stack = 64,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 12,
			.texture_y = 1,
		},
	},
};

static struct item item_saddle = {
	.name = "Saddle",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 8,
			.texture_y = 6,
		},
	},
};

extern struct item item_sugarcane;
extern struct item item_door_wood;
extern struct item item_door_iron;