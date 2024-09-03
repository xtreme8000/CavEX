/*
	Copyright (c) 2024 ByteBit/xtreme8000

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

#include "../../network/server_local.h"

static bool onItemPlace(struct server_local* s, struct item_data* it,
						struct block_info* where, struct block_info* on,
						enum side on_side) {
	struct block_data b_support;
	if(!server_world_get_block(&s->world, where->x, where->y - 1, where->z,
							   &b_support))
		return false;

	if(!blocks[b_support.type] || blocks[b_support.type]->can_see_through)
		return false;

	struct block_data b_top;
	if(!server_world_get_block(&s->world, where->x, where->y + 1, where->z,
							   &b_top))
		return false;

	if(blocks[b_top.type] && !blocks[b_top.type]->place_ignore)
		return false;

	int type = (it->id == ITEM_DOOR_WOOD) ? BLOCK_WOODEN_DOOR : BLOCK_IRON_DOOR;
	int metadata = 0;
	double dx = s->player.x - (where->x + 0.5);
	double dz = s->player.z - (where->z + 0.5);

	if(fabs(dx) > fabs(dz)) {
		metadata = (dx >= 0) ? 2 : 0;
	} else {
		metadata = (dz >= 0) ? 3 : 1;
	}

	server_world_set_block(&s->world, where->x, where->y, where->z,
						   (struct block_data) {
							   .type = type,
							   .metadata = metadata,
						   });
	server_world_set_block(&s->world, where->x, where->y + 1, where->z,
						   (struct block_data) {
							   .type = type,
							   .metadata = metadata | 0x08,
						   });
	return true;
}

struct item item_door_wood = {
	.name = "Wooden Door",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.onItemPlace = onItemPlace,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 11,
			.texture_y = 2,
		},
	},
};

struct item item_door_iron = {
	.name = "Iron Door",
	.has_damage = false,
	.max_stack = 1,
	.renderItem = render_item_flat,
	.onItemPlace = onItemPlace,
	.armor.is_armor = false,
	.tool.type = TOOL_TYPE_ANY,
	.render_data = {
		.item = {
			.texture_x = 12,
			.texture_y = 2,
		},
	},
};
