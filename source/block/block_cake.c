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

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_FULL;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	switch(side) {
		case SIDE_TOP: return TEXTURE_INDEX(9, 7);
		case SIDE_BOTTOM: return TEXTURE_INDEX(12, 7);
		case SIDE_LEFT:
			return (this->block->metadata > 0) ? TEXTURE_INDEX(11, 7) :
												 TEXTURE_INDEX(10, 7);
		default: return TEXTURE_INDEX(10, 7);
	}
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_cake = {
	.name = "Cake",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_cake,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.ignore_lighting = false,
	.flammable = false,
};
