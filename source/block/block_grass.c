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
		aabb_setsize(x, 1.0F, 1.0F, 1.0F);
	return 1;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return face_occlusion_full();
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	switch(side) {
		case SIDE_TOP:
			return (this->neighbours[SIDE_TOP].type == BLOCK_SNOW) ?
				tex_atlas_lookup(TEXAT_SNOW) :
				tex_atlas_lookup(TEXAT_GRASS_TOP);
		case SIDE_BOTTOM: return tex_atlas_lookup(TEXAT_DIRT);
		default:
			return (this->neighbours[SIDE_TOP].type == BLOCK_SNOW) ?
				tex_atlas_lookup(TEXAT_GRASS_SIDE_SNOW) :
				tex_atlas_lookup(TEXAT_GRASS_SIDE);
	}
}

static size_t getDroppedItem(struct block_info* this, struct item_data* it,
							 struct random_gen* g) {
	if(it) {
		it->id = BLOCK_DIRT;
		it->durability = 0;
		it->count = 1;
	}

	return 1;
}

static void onRandomTick(struct server_local* s, struct block_info* this) {
	struct block_data top;
	if(server_world_get_block(&s->world, this->x, this->y + 1, this->z, &top)) {
		if(top.sky_light < 5 && top.torch_light < 5) {
			server_world_set_block(&s->world, this->x, this->y, this->z,
								   (struct block_data) {
									   .type = BLOCK_DIRT,
									   .metadata = 0,
								   });
		} else if(top.sky_light >= 9 || top.torch_light >= 9) {
			for(int k = 0; k < 4; k++) {
				int x = rand_gen_range(&s->rand_src, -1, 2);
				int y = rand_gen_range(&s->rand_src, -1, 5);
				int z = rand_gen_range(&s->rand_src, -1, 2);

				struct block_data neighbour, neighbour_top;
				if((x != 0 || y != 0 || z != 0)
				   && server_world_get_block(&s->world, this->x + x,
											 this->y + y, this->z + z,
											 &neighbour)
				   && neighbour.type == BLOCK_DIRT
				   && server_world_get_block(&s->world, this->x + x,
											 this->y + y + 1, this->z + z,
											 &neighbour_top)
				   && (!blocks[neighbour_top.type]
					   || blocks[neighbour_top.type]->can_see_through)
				   && neighbour_top.type != BLOCK_WATER_FLOW
				   && neighbour_top.type != BLOCK_WATER_STILL
				   && neighbour_top.type != BLOCK_LAVA_FLOW
				   && neighbour_top.type != BLOCK_LAVA_STILL) {
					server_world_set_block(&s->world, this->x + x, this->y + y,
										   this->z + z,
										   (struct block_data) {
											   .type = BLOCK_GRASS,
											   .metadata = 0,
										   });
				}
			}
		}
	}
}

struct block block_grass = {
	.name = "Grass",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.getDroppedItem = getDroppedItem,
	.onRandomTick = onRandomTick,
	.transparent = false,
	.renderBlock = render_block_full,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = false,
	.ignore_lighting = false,
	.flammable = false,
	.place_ignore = false,
	.digging.hardness = 900,
	.digging.tool = TOOL_TYPE_SHOVEL,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_MAX,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_block,
		.onItemPlace = block_place_default,
		.render_data.block.has_default = false,
		.armor.is_armor = false,
		.tool.type = TOOL_TYPE_ANY,
	},
};
