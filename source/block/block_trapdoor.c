/*
	Copyright (c) 2023 ByteBit/xtreme8000

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
	return MATERIAL_WOOD;
}

static size_t getBoundingBox(struct block_info* this, bool entity,
							 struct AABB* x) {
	if(x) {
		if(this->block->metadata & 0x04) {
			switch(this->block->metadata & 0x03) {
				case 0:
					aabb_setsize(x, 1.0F, 1.0F, 0.1875F);
					aabb_translate(x, 0, 0, 0.40625F);
					break;
				case 1:
					aabb_setsize(x, 1.0F, 1.0F, 0.1875F);
					aabb_translate(x, 0, 0, -0.40625F);
					break;
				case 2:
					aabb_setsize(x, 0.1875F, 1.0F, 1.0F);
					aabb_translate(x, 0.40625F, 0, 0);
					break;
				case 3:
					aabb_setsize(x, 0.1875F, 1.0F, 1.0F);
					aabb_translate(x, -0.40625F, 0, 0);
					break;
			}
		} else {
			aabb_setsize(x, 1.0F, 0.1875F, 1.0F);
		}
	}

	return 1;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	if(this->block->metadata & 0x04) {
		// could be improved
		return face_occlusion_empty();
	} else {
		switch(side) {
			case SIDE_TOP: return face_occlusion_empty();
			case SIDE_BOTTOM:
				return (it->block->type == this->block->type) ?
					face_occlusion_full() :
					face_occlusion_empty();
			default: return face_occlusion_rect(3);
		}
	}
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	return tex_atlas_lookup(TEXAT_TRAPDOOR);
}

static bool onItemPlace(struct server_local* s, struct item_data* it,
						struct block_info* where, struct block_info* on,
						enum side on_side) {
	if(!blocks[on->block->type]
	   || (blocks[on->block->type]->can_see_through
		   && blocks[on->block->type]->opacity < 15))
		return false;

	int metadata = 0;
	switch(on_side) {
		case SIDE_LEFT: metadata = 2; break;
		case SIDE_RIGHT: metadata = 3; break;
		case SIDE_FRONT: metadata = 0; break;
		case SIDE_BACK: metadata = 1; break;
		default: return false;
	}

	struct block_data blk = (struct block_data) {
		.type = it->id,
		.metadata = metadata,
		.sky_light = 0,
		.torch_light = 0,
	};

	struct block_info blk_info = *where;
	blk_info.block = &blk;

	if(entity_local_player_block_collide(
		   (vec3) {s->player.x, s->player.y, s->player.z}, &blk_info))
		return false;

	server_world_set_block(&s->world, where->x, where->y, where->z, blk);
	return true;
}

struct block block_trapdoor = {
	.name = "Trapdoor",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.getDroppedItem = block_drop_default,
	.onRandomTick = NULL,
	.onRightClick = NULL,
	.transparent = false,
	.renderBlock = render_block_trapdoor,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.opacity = 1,
	.ignore_lighting = false,
	.flammable = false,
	.place_ignore = false,
	.digging.hardness = 4500, // TODO: might not be correct
	.digging.tool = TOOL_TYPE_ANY,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_ANY,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.onItemPlace = onItemPlace,
		.render_data.block.has_default = false,
		.armor.is_armor = false,
		.tool.type = TOOL_TYPE_ANY,
	},
};
