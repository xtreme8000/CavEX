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
	aabb_setsize(x, 1.0F, 1.0F, 1.0F);
	return true;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return face_occlusion_full();
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	struct block_data* right = this->neighbours + SIDE_RIGHT;
	struct block_data* left = this->neighbours + SIDE_LEFT;
	struct block_data* back = this->neighbours + SIDE_BACK;
	struct block_data* front = this->neighbours + SIDE_FRONT;

	// TODO: double chest wrong orientation

	if(right->type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return TEXTURE_INDEX(9, 1);
			case SIDE_BACK: return TEXTURE_INDEX(9, 2);
			case SIDE_FRONT: return TEXTURE_INDEX(10, 3);
			default: return TEXTURE_INDEX(10, 1);
		}
	}

	if(left->type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return TEXTURE_INDEX(9, 1);
			case SIDE_BACK: return TEXTURE_INDEX(10, 2);
			case SIDE_FRONT: return TEXTURE_INDEX(9, 3);
			default: return TEXTURE_INDEX(10, 1);
		}
	}

	if(back->type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return TEXTURE_INDEX(9, 1);
			case SIDE_RIGHT: return TEXTURE_INDEX(10, 2);
			case SIDE_LEFT: return TEXTURE_INDEX(9, 3);
			default: return TEXTURE_INDEX(10, 1);
		}
	}

	if(front->type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return TEXTURE_INDEX(9, 1);
			case SIDE_RIGHT: return TEXTURE_INDEX(9, 2);
			case SIDE_LEFT: return TEXTURE_INDEX(10, 3);
			default: return TEXTURE_INDEX(10, 1);
		}
	}

	uint8_t tex[SIDE_MAX] = {
		[SIDE_TOP] = TEXTURE_INDEX(9, 1),	[SIDE_BOTTOM] = TEXTURE_INDEX(9, 1),
		[SIDE_BACK] = TEXTURE_INDEX(10, 1), [SIDE_FRONT] = TEXTURE_INDEX(10, 1),
		[SIDE_LEFT] = TEXTURE_INDEX(10, 1), [SIDE_RIGHT] = TEXTURE_INDEX(10, 1),
	};

	if(left->type && !right->type)
		tex[SIDE_RIGHT] = TEXTURE_INDEX(11, 1);
	else if(right->type && !left->type)
		tex[SIDE_LEFT] = TEXTURE_INDEX(11, 1);
	else if(front->type && !back->type)
		tex[SIDE_BACK] = TEXTURE_INDEX(11, 1);
	else if(back->type && !front->type)
		tex[SIDE_FRONT] = TEXTURE_INDEX(11, 1);
	else
		tex[SIDE_BACK] = TEXTURE_INDEX(11, 1);

	return tex[side];
}

struct block block_chest = {
	.name = "Chest",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.renderBlock = render_block_full,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = false,
	.ignore_lighting = false,
	.flammable = false,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.render_data.block.has_default = true,
		.render_data.block.default_metadata = 0,
		.render_data.block.default_rotation = 2,
	},
};

struct block block_locked_chest = {
	.name = "Chest",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.renderBlock = render_block_full,
	.renderBlockAlways = NULL,
	.luminance = 15,
	.double_sided = false,
	.can_see_through = false,
	.ignore_lighting = false,
	.flammable = false,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.render_data.block.has_default = true,
		.render_data.block.default_metadata = 0,
		.render_data.block.default_rotation = 2,
	},
};
