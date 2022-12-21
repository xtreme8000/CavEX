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
	aabb_setsize(x, 1.0F,
				 ((this->block->metadata & 0x7) > 1
				  && (this->block->metadata & 0x7) < 6) ?
					 0.625F :
					 0.125F,
				 1.0F);
	return !entity;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return face_occlusion_empty();
}

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_FLAT;
}

static uint8_t getTextureIndex1(struct block_info* this, enum side side) {
	return (this->block->metadata < 6) ? TEXTURE_INDEX(0, 8) :
										 TEXTURE_INDEX(0, 7);
}

static uint8_t getTextureIndex2(struct block_info* this, enum side side) {
	return (this->block->metadata & 0x8) ? TEXTURE_INDEX(3, 11) :
										   TEXTURE_INDEX(3, 10);
}

static uint8_t getTextureIndex3(struct block_info* this, enum side side) {
	return TEXTURE_INDEX(3, 12);
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_rail = {
	.name = "Rail",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex1,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_rail,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.render_block_data.rail_curved_possible = true,
	.ignore_lighting = false,
	.flammable = false,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_flat,
	},
};

struct block block_powered_rail = {
	.name = "Powered rail",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex2,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_rail,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.render_block_data.rail_curved_possible = false,
	.ignore_lighting = false,
	.flammable = false,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_flat,
	},
};

struct block block_detector_rail = {
	.name = "Detector rail",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex3,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_rail,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.render_block_data.rail_curved_possible = false,
	.ignore_lighting = false,
	.flammable = false,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_flat,
	},
};
