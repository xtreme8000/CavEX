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

#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_WOOL;
}

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	aabb_setsize(x, 0.875F - 0.125F * this->block->metadata, 0.5F, 0.875F);
	aabb_translate(x, 0.0625F * this->block->metadata, 0, 0);
	return true;
}

// TODO: mask not correct for all states
struct face_occlusion bottom_mask = {
	.mask = {0x00007FFE, 0x7FFE7FFE, 0x7FFE7FFE, 0x7FFE7FFE, 0x7FFE7FFE,
			 0x7FFE7FFE, 0x7FFE7FFE, 0x7FFE0000},
};

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	switch(side) {
		case SIDE_BOTTOM: return &bottom_mask;
		default: return face_occlusion_empty();
	}
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	switch(side) {
		case SIDE_TOP: return tex_atlas_lookup(TEXAT_CAKE_TOP);
		case SIDE_BOTTOM: return tex_atlas_lookup(TEXAT_CAKE_BOTTOM);
		case SIDE_LEFT:
			return (this->block->metadata > 0) ?
				tex_atlas_lookup(TEXAT_CAKE_SIDE_CUT) :
				tex_atlas_lookup(TEXAT_CAKE_SIDE);
		default: return tex_atlas_lookup(TEXAT_CAKE_SIDE);
	}
}

struct block block_cake = {
	.name = "Cake",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.renderBlock = render_block_cake,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.opacity = 0,
	.ignore_lighting = false,
	.flammable = false,
	.place_ignore = false,
	.digging.hardness = 750,
	.digging.tool = TOOL_TYPE_ANY,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_ANY,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.onItemPlace = block_place_default,
		.render_data.block.has_default = false,
		.tool.type = TOOL_TYPE_ANY,
	},
};
