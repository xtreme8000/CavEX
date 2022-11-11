#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_STONE;
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
	switch(side) {
		case SIDE_FRONT:
			if(this->block->metadata == 2)
				return TEXTURE_INDEX(12, 2);
			else
				return TEXTURE_INDEX(13, 2);
		case SIDE_BACK:
			if(this->block->metadata == 3)
				return TEXTURE_INDEX(12, 2);
			else
				return TEXTURE_INDEX(13, 2);
		case SIDE_RIGHT:
			if(this->block->metadata == 5)
				return TEXTURE_INDEX(12, 2);
			else
				return TEXTURE_INDEX(13, 2);
		case SIDE_LEFT:
			if(this->block->metadata == 4)
				return TEXTURE_INDEX(12, 2);
			else
				return TEXTURE_INDEX(13, 2);
		default: return TEXTURE_INDEX(13, 4);
	}
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_furnaceoff = {
	.name = "Furnace",
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

struct block block_furnaceon = {
	.name = "Furnace",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_full,
	.luminance = 13,
	.double_sided = false,
	.can_see_through = false,
	.ignore_lighting = false,
};
