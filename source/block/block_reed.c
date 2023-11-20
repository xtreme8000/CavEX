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

#include "../network/server_local.h"
#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_ORGANIC;
}

static size_t getBoundingBox(struct block_info* this, bool entity,
							 struct AABB* x) {
	if(x)
		aabb_setsize(x, 0.75F, 1.0F, 0.75F);
	return entity ? 0 : 1;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return face_occlusion_empty();
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	return tex_atlas_lookup(TEXAT_REED);
}

static size_t getDroppedItem(struct block_info* this, struct item_data* it,
							 struct random_gen* g) {
	if(it) {
		it->id = ITEM_REED;
		it->durability = 0;
		it->count = 1;
	}

	return 1;
}

static void onRandomTick(struct server_local* s, struct block_info* this) {
	// TODO: check for water
	bool has_support = false;
	int height = 1;
	for(int k = 0; k < 2; k++) {
		struct block_data below;
		if(!server_world_get_block(&s->world, this->x, this->y - k - 1, this->z,
								   &below))
			below.type = BLOCK_AIR;

		if(below.type == BLOCK_REED) {
			height++;
			has_support = true;
		} else {
			if(below.type == BLOCK_DIRT || below.type == BLOCK_GRASS)
				has_support = true;
			break;
		}
	}

	if(!has_support) {
		server_world_set_block(&s->world, this->x, this->y, this->z,
							   (struct block_data) {
								   .type = BLOCK_AIR,
								   .metadata = 0,
							   });
		server_local_spawn_block_drops(s, this);
	} else if(height < 3) {
		if(this->block->metadata == 0xF) {
			struct block_data above;
			if(!server_world_get_block(&s->world, this->x, this->y + 1, this->z,
									   &above)
			   || above.type == BLOCK_AIR)
				server_world_set_block(&s->world, this->x, this->y + 1, this->z,
									   (struct block_data) {
										   .type = BLOCK_REED,
										   .metadata = 0,
									   });
		}

		this->block->metadata++;
		server_world_set_block(&s->world, this->x, this->y, this->z,
							   *this->block);
	}
}

struct block block_reed = {
	.name = "Reed",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.getDroppedItem = getDroppedItem,
	.onRandomTick = onRandomTick,
	.transparent = false,
	.renderBlock = render_block_cross,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.opacity = 0,
	.render_block_data.cross_random_displacement = false,
	.ignore_lighting = false,
	.flammable = false,
	.place_ignore = false,
	.digging.hardness = 50,
	.digging.tool = TOOL_TYPE_ANY,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_ANY,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_flat,
		.onItemPlace = block_place_default,
		.armor.is_armor = false,
		.tool.type = TOOL_TYPE_ANY,
	},
};
