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
	return MATERIAL_WOOD;
}

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	aabb_setsize(x, 1.0F, 0.5625F, 1.0F);
	return true;
}

// TODO: add missing bed legs to mask? (difficult, many states)
static struct face_occlusion side_mask = {
	.mask = {0x00000000, 0x00000000, 0x00000000, 0x0000FFFF, 0xFFFFFFFF,
			 0xFFFFFFFF, 0xFFFF0000, 0x00000000},
};

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	switch(side) {
		case SIDE_TOP:
		case SIDE_BOTTOM: return face_occlusion_empty();
		default: return &side_mask;
	}
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	if(this->block->metadata & 0x8) {
		switch(side) {
			case SIDE_TOP: return tex_atlas_lookup(TEXAT_BED_TOP_2);
			case SIDE_LEFT:
			case SIDE_RIGHT:
				return (this->block->metadata & 0x1) ?
					tex_atlas_lookup(TEXAT_BED_FRONT) :
					tex_atlas_lookup(TEXAT_BED_SIDE_2);
			case SIDE_FRONT:
			case SIDE_BACK:
				return (this->block->metadata & 0x1) ?
					tex_atlas_lookup(TEXAT_BED_SIDE_2) :
					tex_atlas_lookup(TEXAT_BED_FRONT);
			default: return tex_atlas_lookup(TEXAT_PLANKS);
		}
	} else {
		switch(side) {
			case SIDE_TOP: return tex_atlas_lookup(TEXAT_BED_TOP_1);
			case SIDE_LEFT:
			case SIDE_RIGHT:
				return (this->block->metadata & 0x1) ?
					tex_atlas_lookup(TEXAT_BED_BACK) :
					tex_atlas_lookup(TEXAT_BED_SIDE_1);
			case SIDE_FRONT:
			case SIDE_BACK:
				return (this->block->metadata & 0x1) ?
					tex_atlas_lookup(TEXAT_BED_SIDE_1) :
					tex_atlas_lookup(TEXAT_BED_BACK);
			default: return tex_atlas_lookup(TEXAT_PLANKS);
		}
	}
}

struct block block_bed = {
	.name = "Bed",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.renderBlock = render_block_bed,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.opacity = 1,
	.ignore_lighting = false,
	.flammable = false,
	.place_ignore = false,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.onItemPlace = block_place_default,
		.render_data.block.has_default = false,
	},
};
