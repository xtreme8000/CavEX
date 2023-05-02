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

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	return tex_atlas_lookup(TEXAT_LADDER);
}

static bool onItemPlace(struct server_local* s, struct item_data* it,
						struct block_info* where, struct block_info* on,
						enum side on_side) {
	if(!blocks[on->block->type] || blocks[on->block->type]->can_see_through)
		return false;

	int metadata = 0;
	switch(on_side) {
		case SIDE_LEFT: metadata = 4; break;
		case SIDE_RIGHT: metadata = 5; break;
		case SIDE_FRONT: metadata = 2; break;
		case SIDE_BACK: metadata = 3; break;
		default: return false;
	}

	server_world_set_block(&s->world, where->x, where->y, where->z,
						   (struct block_data) {
							   .type = it->id,
							   .metadata = metadata,
							   .sky_light = 0,
							   .torch_light = 0,
						   });
	return true;
}

struct block block_ladder = {
	.name = "Ladder",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.renderBlock = render_block_ladder,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.opacity = 0,
	.ignore_lighting = false,
	.flammable = false,
	.place_ignore = false,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_flat,
		.onItemPlace = onItemPlace,
	},
};
