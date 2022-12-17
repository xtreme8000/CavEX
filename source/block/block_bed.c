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

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_FULL;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	if(this->block->metadata & 0x8) {
		switch(side) {
			case SIDE_TOP: return TEXTURE_INDEX(7, 8);
			case SIDE_LEFT:
			case SIDE_RIGHT:
				return (this->block->metadata & 0x1) ? TEXTURE_INDEX(11, 8) :
													   TEXTURE_INDEX(10, 8);
			case SIDE_FRONT:
			case SIDE_BACK:
				return (this->block->metadata & 0x1) ? TEXTURE_INDEX(10, 8) :
													   TEXTURE_INDEX(11, 8);
			default: return TEXTURE_INDEX(4, 0);
		}
	} else {
		switch(side) {
			case SIDE_TOP: return TEXTURE_INDEX(6, 8);
			case SIDE_LEFT:
			case SIDE_RIGHT:
				return (this->block->metadata & 0x1) ? TEXTURE_INDEX(8, 8) :
													   TEXTURE_INDEX(9, 8);
			case SIDE_FRONT:
			case SIDE_BACK:
				return (this->block->metadata & 0x1) ? TEXTURE_INDEX(9, 8) :
													   TEXTURE_INDEX(8, 8);
			default: return TEXTURE_INDEX(4, 0);
		}
	}
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_bed = {
	.name = "Bed",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_bed,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.ignore_lighting = false,
	.flammable = false,
};
