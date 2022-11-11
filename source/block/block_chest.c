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

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_FULL;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	struct block_data right
		= world_get_block(this->world, this->x + 1, this->y, this->z);
	struct block_data left
		= world_get_block(this->world, this->x - 1, this->y, this->z);
	struct block_data back
		= world_get_block(this->world, this->x, this->y, this->z + 1);
	struct block_data front
		= world_get_block(this->world, this->x, this->y, this->z - 1);

	if(right.type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return TEXTURE_INDEX(9, 1);
			case SIDE_BACK: return TEXTURE_INDEX(9, 2);
			case SIDE_FRONT: return TEXTURE_INDEX(10, 3);
			default: return TEXTURE_INDEX(10, 1);
		}
	}

	if(left.type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return TEXTURE_INDEX(9, 1);
			case SIDE_BACK: return TEXTURE_INDEX(10, 2);
			case SIDE_FRONT: return TEXTURE_INDEX(9, 3);
			default: return TEXTURE_INDEX(10, 1);
		}
	}

	if(back.type == this->block->type) {
		switch(side) {
			case SIDE_TOP:
			case SIDE_BOTTOM: return TEXTURE_INDEX(9, 1);
			case SIDE_RIGHT: return TEXTURE_INDEX(10, 2);
			case SIDE_LEFT: return TEXTURE_INDEX(9, 3);
			default: return TEXTURE_INDEX(10, 1);
		}
	}

	if(front.type == this->block->type) {
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

	if(left.type && !right.type)
		tex[SIDE_RIGHT] = TEXTURE_INDEX(11, 1);
	else if(right.type && !left.type)
		tex[SIDE_LEFT] = TEXTURE_INDEX(11, 1);
	else if(front.type && !back.type)
		tex[SIDE_BACK] = TEXTURE_INDEX(11, 1);
	else if(back.type && !front.type)
		tex[SIDE_FRONT] = TEXTURE_INDEX(11, 1);
	else
		tex[SIDE_BACK] = TEXTURE_INDEX(11, 1);

	return tex[side];
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_chest = {
	.name = "Chest",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_full,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = false,
	.ignore_lighting = false,
};

struct block block_locked_chest = {
	.name = "Chest",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_full,
	.luminance = 15,
	.double_sided = false,
	.can_see_through = false,
	.ignore_lighting = false,
};
