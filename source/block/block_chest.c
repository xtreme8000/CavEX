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
			case SIDE_BOTTOM: return tex_atlas_lookup(TEXAT_CHEST_TOP);
			case SIDE_BACK: return tex_atlas_lookup(TEXAT_CHEST_FRONT_1);
			case SIDE_FRONT: return tex_atlas_lookup(TEXAT_CHEST_BACK_2);
			default: return tex_atlas_lookup(TEXAT_CHEST_SIDE);
		}
	}

	if(left->type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return tex_atlas_lookup(TEXAT_CHEST_TOP);
			case SIDE_BACK: return tex_atlas_lookup(TEXAT_CHEST_FRONT_2);
			case SIDE_FRONT: return tex_atlas_lookup(TEXAT_CHEST_BACK_1);
			default: return tex_atlas_lookup(TEXAT_CHEST_SIDE);
		}
	}

	if(back->type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return tex_atlas_lookup(TEXAT_CHEST_TOP);
			case SIDE_RIGHT: return tex_atlas_lookup(TEXAT_CHEST_FRONT_2);
			case SIDE_LEFT: return tex_atlas_lookup(TEXAT_CHEST_BACK_1);
			default: return tex_atlas_lookup(TEXAT_CHEST_SIDE);
		}
	}

	if(front->type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return tex_atlas_lookup(TEXAT_CHEST_TOP);
			case SIDE_RIGHT: return tex_atlas_lookup(TEXAT_CHEST_FRONT_1);
			case SIDE_LEFT: return tex_atlas_lookup(TEXAT_CHEST_BACK_2);
			default: return tex_atlas_lookup(TEXAT_CHEST_SIDE);
		}
	}

	uint8_t tex[SIDE_MAX] = {
		[SIDE_TOP] = tex_atlas_lookup(TEXAT_CHEST_TOP),
		[SIDE_BOTTOM] = tex_atlas_lookup(TEXAT_CHEST_TOP),
		[SIDE_BACK] = tex_atlas_lookup(TEXAT_CHEST_SIDE),
		[SIDE_FRONT] = tex_atlas_lookup(TEXAT_CHEST_SIDE),
		[SIDE_LEFT] = tex_atlas_lookup(TEXAT_CHEST_SIDE),
		[SIDE_RIGHT] = tex_atlas_lookup(TEXAT_CHEST_SIDE),
	};

	if(left->type && !right->type)
		tex[SIDE_RIGHT] = tex_atlas_lookup(TEXAT_CHEST_FRONT_SINGLE);
	else if(right->type && !left->type)
		tex[SIDE_LEFT] = tex_atlas_lookup(TEXAT_CHEST_FRONT_SINGLE);
	else if(front->type && !back->type)
		tex[SIDE_BACK] = tex_atlas_lookup(TEXAT_CHEST_FRONT_SINGLE);
	else if(back->type && !front->type)
		tex[SIDE_FRONT] = tex_atlas_lookup(TEXAT_CHEST_FRONT_SINGLE);
	else
		tex[SIDE_BACK] = tex_atlas_lookup(TEXAT_CHEST_FRONT_SINGLE);

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
	.place_ignore = false,
	.digging.hardness = 3750,
	.digging.tool = TOOL_TYPE_AXE,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_MAX,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.onItemPlace = block_place_default,
		.render_data.block.has_default = true,
		.render_data.block.default_metadata = 0,
		.render_data.block.default_rotation = 2,
		.armor.is_armor = false,
		.tool.type = TOOL_TYPE_ANY,
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
	.place_ignore = false,
	.digging.hardness = 3750,
	.digging.tool = TOOL_TYPE_AXE,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_MAX,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.onItemPlace = block_place_default,
		.render_data.block.has_default = true,
		.render_data.block.default_metadata = 0,
		.render_data.block.default_rotation = 2,
		.armor.is_armor = false,
		.tool.type = TOOL_TYPE_ANY,
	},
};
