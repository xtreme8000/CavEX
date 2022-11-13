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
	switch(this->block->metadata) {
		default:
		case 0:
			switch(side) {
				case SIDE_TOP:
				case SIDE_BOTTOM: return TEXTURE_INDEX(5, 1);
				default: return TEXTURE_INDEX(4, 1);
			}
			break;
		case 1:
			switch(side) {
				case SIDE_TOP:
				case SIDE_BOTTOM: return TEXTURE_INDEX(5, 1);
				default: return TEXTURE_INDEX(4, 7);
			}
			break;
		case 2:
			switch(side) {
				case SIDE_TOP:
				case SIDE_BOTTOM: return TEXTURE_INDEX(5, 1);
				default: return TEXTURE_INDEX(5, 7);
			}
			break;
	}
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_log = {
	.name = "Log",
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
	.flammable = true,
};
