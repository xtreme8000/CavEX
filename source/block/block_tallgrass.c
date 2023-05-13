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

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	aabb_setsize(x, 0.8125F, 0.8125F, 0.8125F);
	return !entity;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return face_occlusion_empty();
}

static uint8_t getTextureIndex1(struct block_info* this, enum side side) {
	switch(this->block->metadata) {
		case 1: return tex_atlas_lookup(TEXAT_TALLGRASS); // tallgrass
		case 2: return tex_atlas_lookup(TEXAT_FERN);	  // fern
		default: return tex_atlas_lookup(TEXAT_DEADBUSH); // deadbush
	}
}

static uint8_t getTextureIndex2(struct block_info* this, enum side side) {
	return tex_atlas_lookup(TEXAT_DEADBUSH); // deadbush
}

static bool onItemPlace1(struct server_local* s, struct item_data* it,
						 struct block_info* where, struct block_info* on,
						 enum side on_side) {
	struct block_data blk;
	if(!server_world_get_block(&s->world, where->x, where->y - 1, where->z,
							   &blk))
		return false;

	if(blk.type != BLOCK_DIRT && blk.type != BLOCK_GRASS)
		return false;

	return block_place_default(s, it, where, on, on_side);
}

static bool onItemPlace2(struct server_local* s, struct item_data* it,
						 struct block_info* where, struct block_info* on,
						 enum side on_side) {
	struct block_data blk;
	if(!server_world_get_block(&s->world, where->x, where->y - 1, where->z,
							   &blk))
		return false;

	if(!blocks[blk.type] || blocks[blk.type]->can_see_through)
		return false;

	return block_place_default(s, it, where, on, on_side);
}

struct block block_tallgrass = {
	.name = "Tallgrass",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex1,
	.transparent = false,
	.renderBlock = render_block_cross,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.opacity = 0,
	.render_block_data.cross_random_displacement = true,
	.ignore_lighting = false,
	.flammable = true,
	.place_ignore = false,
	.digging.hardness = 50,
	.digging.tool = TOOL_TYPE_ANY,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_ANY,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_flat,
		.onItemPlace = onItemPlace1,
		.tool.type = TOOL_TYPE_ANY,
	},
};

struct block block_deadbush = {
	.name = "Deadbush",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex2,
	.transparent = false,
	.renderBlock = render_block_cross,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.opacity = 0,
	.render_block_data.cross_random_displacement = false,
	.ignore_lighting = false,
	.flammable = true,
	.place_ignore = false,
	.digging.hardness = 50,
	.digging.tool = TOOL_TYPE_ANY,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_ANY,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_flat,
		.onItemPlace = onItemPlace2,
		.tool.type = TOOL_TYPE_ANY,
	},
};
