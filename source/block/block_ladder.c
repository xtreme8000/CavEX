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
	return MATERIAL_STONE;
}

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	switch(this->block->metadata) {
		case 2:
			aabb_setsize(x, 1.0F, 1.0F, 0.125F);
			aabb_translate(x, 0, 0, 0.4375F);
			return true;
		case 3:
			aabb_setsize(x, 1.0F, 1.0F, 0.125F);
			aabb_translate(x, 0, 0, -0.4375F);
			return true;
		case 4:
			aabb_setsize(x, 0.125F, 1.0F, 1.0F);
			aabb_translate(x, 0.4375F, 0, 0);
			return true;
		case 5:
			aabb_setsize(x, 0.125F, 1.0F, 1.0F);
			aabb_translate(x, -0.4375F, 0, 0);
			return true;
		default: return false;
	}
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return face_occlusion_empty();
}

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_LADDER;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	return TEXTURE_INDEX(3, 5);
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_ladder = {
	.name = "Ladder",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_ladder,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.ignore_lighting = false,
	.flammable = false,
};
