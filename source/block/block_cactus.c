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
	return MATERIAL_ORGANIC;
}

static size_t getBoundingBox(struct block_info* this, bool entity,
							 struct AABB* x) {
	if(x)
		aabb_setsize(x, 0.875F, 1.0F, 0.875F);
	return 1;
}

static struct face_occlusion side_top_bottom = {
	.mask = {0x00007FFE, 0x7FFE7FFE, 0x7FFE7FFE, 0x7FFE7FFE, 0x7FFE7FFE,
			 0x7FFE7FFE, 0x7FFE7FFE, 0x7FFE0000},
};

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	switch(side) {
		case SIDE_TOP:
		case SIDE_BOTTOM: return &side_top_bottom;
		default: return face_occlusion_empty();
	}
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	switch(side) {
		case SIDE_TOP: return tex_atlas_lookup(TEXAT_CACTUS_TOP);
		case SIDE_BOTTOM: return tex_atlas_lookup(TEXAT_CACTUS_BOTTOM);
		default: return tex_atlas_lookup(TEXAT_CACTUS_SIDE);
	}
}

static bool onItemPlace(struct server_local* s, struct item_data* it,
						struct block_info* where, struct block_info* on,
						enum side on_side) {
	struct block_data blk;
	if(!server_world_get_block(&s->world, where->x, where->y - 1, where->z,
							   &blk))
		return false;

	if(blk.type != BLOCK_SAND && blk.type != BLOCK_CACTUS)
		return false;

	return block_place_default(s, it, where, on, on_side);
}

static void onRandomTick(struct server_local* s, struct block_info* this) {
	bool has_support = false;
	int height = 1;
	for(int k = 0; k < 2; k++) {
		struct block_data below;
		if(!server_world_get_block(&s->world, this->x, this->y - k - 1, this->z,
								   &below))
			below.type = BLOCK_AIR;

		if(below.type == BLOCK_CACTUS) {
			height++;
			has_support = true;
		} else {
			if(below.type == BLOCK_SAND)
				has_support = true;
			break;
		}
	}

	if(!has_support) {
		server_world_set_block(&s->world, this->x, this->y, this->z,
							   (struct block_data) {
								   .type = BLOCK_AIR,
								   .metadata = 0,
							   });
		server_local_spawn_block_drops(s, this);
	} else if(height < 3) {
		if(this->block->metadata == 0xF) {
			struct block_data above;
			if(!server_world_get_block(&s->world, this->x, this->y + 1, this->z,
									   &above)
			   || above.type == BLOCK_AIR)
				server_world_set_block(&s->world, this->x, this->y + 1, this->z,
									   (struct block_data) {
										   .type = BLOCK_CACTUS,
										   .metadata = 0,
									   });
		}

		this->block->metadata++;
		server_world_set_block(&s->world, this->x, this->y, this->z,
							   *this->block);
	}
}

struct block block_cactus = {
	.name = "Cactus",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.getDroppedItem = block_drop_default,
	.onRandomTick = onRandomTick,
	.transparent = false,
	.renderBlock = render_block_cactus,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.opacity = 0,
	.ignore_lighting = false,
	.flammable = false,
	.place_ignore = false,
	.digging.hardness = 600,
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
