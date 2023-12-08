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
		aabb_setsize(x, 0.8125F, 0.8125F, 0.8125F);
	return entity ? 0 : 1;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return face_occlusion_empty();
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	switch(this->block->metadata & 0x3) {
		default:
		case 0: return tex_atlas_lookup(TEXAT_SAPLING_OAK);
		case 1: return tex_atlas_lookup(TEXAT_SAPLING_SPRUCE);
		case 2: return tex_atlas_lookup(TEXAT_SAPLING_BIRCH);
	}
}

static bool onItemPlace(struct server_local* s, struct item_data* it,
						struct block_info* where, struct block_info* on,
						enum side on_side) {
	struct block_data blk;
	if(!server_world_get_block(&s->world, where->x, where->y - 1, where->z,
							   &blk))
		return false;

	if(blk.type != BLOCK_DIRT && blk.type != BLOCK_GRASS)
		return false;

	return block_place_default(s, it, where, on, on_side);
}

static size_t getDroppedItem(struct block_info* this, struct item_data* it,
							 struct random_gen* g) {
	if(it) {
		it->id = this->block->type;
		it->durability = this->block->metadata & 3;
		it->count = 1;
	}

	return 1;
}

static void onRandomTick(struct server_local* s, struct block_info* this) {
	if(this->block->sky_light < 9 && this->block->torch_light < 9)
		return;

	int age = this->block->metadata >> 2;
	int tree_type = this->block->metadata & 3;

	if(age < 3) {
		this->block->metadata = ((age + 1) << 2) | tree_type;
		server_world_set_block(&s->world, this->x, this->y, this->z,
							   *this->block);
		return;
	}

	int height = rand_gen_range(&s->rand_src, 4, 7);

	for(int k = 1; k < height; k++) {
		struct block_data blk;
		if(server_world_get_block(&s->world, this->x, this->y + k, this->z,
								  &blk)
		   && blk.type != BLOCK_AIR) {
			return;
		}
	}

	for(int k = 0; k < height; k++)
		server_world_set_block(&s->world, this->x, this->y + k, this->z,
							   (struct block_data) {
								   .type = BLOCK_LOG,
								   .metadata = this->block->metadata & 0x3,
							   });

	for(int y = -3; y <= 0; y++) {
		int size = (y < -1) ? 2 : 1;

		for(int x = -size; x <= size; x++) {
			for(int z = -size; z <= size; z++) {
				struct block_data blk;
				if((x != 0 || z != 0 || y == 0)
				   && ((abs(x) != size || abs(z) != size)
					   || (rand_gen(&s->rand_src) & 1 && y < 0))
				   && server_world_get_block(&s->world, this->x + x,
											 this->y + height + y, this->z + z,
											 &blk)
				   && blk.type == BLOCK_AIR) {
					server_world_set_block(
						&s->world, this->x + x, this->y + height + y,
						this->z + z,
						(struct block_data) {
							.type = BLOCK_LEAVES,
							.metadata = this->block->metadata & 0x3,
						});
				}
			}
		}
	}
}

struct block block_sapling = {
	.name = "Sapling",
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.getDroppedItem = getDroppedItem,
	.onRandomTick = onRandomTick,
	.onRightClick = NULL,
	.transparent = false,
	.renderBlock = render_block_cross,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.opacity = 0,
	.render_block_data.cross_random_displacement = false,
	.ignore_lighting = false,
	.flammable = false,
	.place_ignore = false,
	.digging.hardness = 50,
	.digging.tool = TOOL_TYPE_ANY,
	.digging.min = TOOL_TIER_ANY,
	.digging.best = TOOL_TIER_ANY,
	.block_item = {
		.has_damage = false,
		.max_stack = 64,
		.renderItem = render_item_flat,
		.onItemPlace = onItemPlace,
		.armor.is_armor = false,
		.tool.type = TOOL_TYPE_ANY,
	},
};
