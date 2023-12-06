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
		aabb_setsize(x, 1.0F, 1.0F, 1.0F);
	return 1;
}

struct face_occlusion sides_mask = {
	.mask = {0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA,
			 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA},
};

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return (it->block->type == this->block->type) ?
		((side == SIDE_RIGHT || side == SIDE_BACK || side == SIDE_BOTTOM) ?
			 face_occlusion_full() :
			 &sides_mask) :
		&sides_mask;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	switch(this->block->metadata & 0x3) {
		default:
		case 0: return tex_atlas_lookup(TEXAT_LEAVES_OAK);
		case 1: return tex_atlas_lookup(TEXAT_LEAVES_SPRUCE);
		case 2: return tex_atlas_lookup(TEXAT_LEAVES_BIRCH);
	}
}

static size_t getDroppedItem(struct block_info* this, struct item_data* it,
							 struct random_gen* g) {
	bool drop_sapling = (rand_gen(g) % 16) == 0;

	if(it && drop_sapling) {
		it->id = BLOCK_SAPLING;
		it->durability = this->block->metadata & 0x3;
		it->count = 1;
	}

	return drop_sapling ? 1 : 0;
}

static void onRandomTick(struct server_local* s, struct block_info* this) {
	for(int y = -4; y <= 4; y++) {
		for(int x = -4; x <= 4; x++) {
			for(int z = -4; z <= 4; z++) {
				struct block_data log;
				if((x != 0 || y != 0 || z != 0)
				   && server_world_get_block(&s->world, this->x + x,
											 this->y + y, this->z + z, &log)
				   && log.type == BLOCK_LOG)
					return;
			}
		}
	}

	server_world_set_block(&s->world, this->x, this->y, this->z,
						   (struct block_data) {
							   .type = BLOCK_AIR,
							   .metadata = 0,
						   });
	server_local_spawn_block_drops(s, this);
}

struct block block_leaves = {
	.name = "Leaves",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.getDroppedItem = getDroppedItem,
	.onRandomTick = onRandomTick,
	.onRightClick = NULL,
	.transparent = false,
	.renderBlock = render_block_full,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.opacity = 1,
	.ignore_lighting = false,
	.flammable = true,
	.place_ignore = false,
	.digging.hardness = 300,
	.digging.tool = TOOL_TYPE_ANY,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_ANY,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.onItemPlace = block_place_default,
		.render_data.block.has_default = false,
		.armor.is_armor = false,
		.tool.type = TOOL_TYPE_ANY,
	},
};
