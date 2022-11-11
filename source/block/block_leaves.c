#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_ORGANIC;
}

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	aabb_setsize(x, 1.0F, 1.0F, 1.0F);
	return true;
}

struct face_occlusion sides_mask = {
	.mask = {0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA,
			 0xAAAAAAAA, 0xAAAAAAAA, 0xAAAAAAAA},
};

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return (it->block->type == this->block->type) ?
		((side == SIDE_RIGHT || side == SIDE_BACK || side == SIDE_BOTTOM) ?
			 face_occlusion_full() :
			 &sides_mask) :
		&sides_mask;
}

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_FULL;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	switch(this->block->metadata & 0x3) {
		case 1: return TEXTURE_INDEX(4, 8);
		case 2: return TEXTURE_INDEX(4, 3);
		default: return TEXTURE_INDEX(5, 11);
	}
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	switch(this->block->metadata & 0x3) {
		case 0: return 0x619961;
		case 1: return 0x48B518;
		case 2: return 0x80A755;
		default: return 0xFFFFFF;
	}
}

struct block block_leaves = {
	.name = "Leaves",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_full,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.ignore_lighting = false,
};
