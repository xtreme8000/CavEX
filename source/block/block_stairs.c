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

static enum block_material getMaterial1(struct block_info* this) {
	return MATERIAL_WOOD;
}

static enum block_material getMaterial2(struct block_info* this) {
	return MATERIAL_STONE;
}

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	aabb_setsize(x, 1.0F, 1.0F, 1.0F);
	return true;
}

static struct face_occlusion side_mask = {
	.mask = {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFFFFFFFF,
			 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
};

static struct face_occlusion side_mask_mirrored = {
	.mask = {0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0xFF00FF00, 0xFFFFFFFF,
			 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
};

static struct face_occlusion side_top_mask_1 = {
	.mask = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000,
			 0x00000000, 0x00000000, 0x00000000},
};

static struct face_occlusion side_top_mask_2 = {
	.mask = {0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF, 0x00FF00FF,
			 0x00FF00FF, 0x00FF00FF, 0x00FF00FF},
};

static struct face_occlusion side_top_mask_3 = {
	.mask = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF,
			 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
};

static struct face_occlusion side_top_mask_4 = {
	.mask = {0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00, 0xFF00FF00,
			 0xFF00FF00, 0xFF00FF00, 0xFF00FF00},
};

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	enum side facing = (enum side[4]) {SIDE_RIGHT, SIDE_LEFT, SIDE_BACK,
									   SIDE_FRONT}[this->block->metadata & 3];

	if(side == facing || side == SIDE_BOTTOM) {
		return face_occlusion_full();
	} else if(side == blocks_side_opposite(facing)) {
		return face_occlusion_rect(8);
	}

	if(side == SIDE_TOP) {
		switch(facing) {
			case SIDE_FRONT: return &side_top_mask_1;
			case SIDE_BACK: return &side_top_mask_3;
			case SIDE_RIGHT: return &side_top_mask_2;
			case SIDE_LEFT: return &side_top_mask_4;
			default:
				return face_occlusion_empty();
				// case never reached, just for -pedantic
		}
	} else {
		switch(facing) {
			case SIDE_FRONT: return &side_mask;
			case SIDE_BACK: return &side_mask_mirrored;
			case SIDE_RIGHT: return &side_mask;
			case SIDE_LEFT: return &side_mask_mirrored;
			default:
				return face_occlusion_empty();
				// case never reached, just for -pedantic
		}
	}
}

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_STAIRS;
}

static uint8_t getTextureIndex1(struct block_info* this, enum side side) {
	return TEXTURE_INDEX(4, 0);
}

static uint8_t getTextureIndex2(struct block_info* this, enum side side) {
	return TEXTURE_INDEX(0, 1);
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_wooden_stairs = {
	.name = "Stairs",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial1,
	.getTextureIndex = getTextureIndex1,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_stairs,
	.renderBlockAlways = render_block_stairs_always,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.ignore_lighting = true,
	.flammable = true,
};

struct block block_stone_stairs = {
	.name = "Stairs",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial2,
	.getTextureIndex = getTextureIndex2,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_stairs,
	.renderBlockAlways = render_block_stairs_always,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.ignore_lighting = true,
	.flammable = false,
};
