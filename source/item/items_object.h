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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.render_data = {
		.item = {
			.texture_x = 10,
			.texture_y = 0,
		},
	},
};

static struct item item_bow = {
	.name = "Bow",
	.has_damage = true,
	.max_stack = 1,
	.renderItem = render_item_flat,
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
	.render_data = {
		.item = {
			.texture_x = 5,
			.texture_y = 3,
		},
	},
};
