#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_ORGANIC;
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
		case SIDE_TOP:
			return (this->neighbours[SIDE_TOP].type == BLOCK_SNOW) ?
				TEXTURE_INDEX(2, 4) :
				TEXTURE_INDEX(5, 9);
		case SIDE_BOTTOM: return TEXTURE_INDEX(2, 0);
		default:
			return (this->neighbours[SIDE_TOP].type == BLOCK_SNOW) ?
				TEXTURE_INDEX(4, 4) :
				TEXTURE_INDEX(5, 10);
	}
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	switch(side) {
		case SIDE_TOP: return 0x619961;
		default: return 0xFFFFFF;
	}
}

struct block block_grass = {
	.name = "Grass",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_full,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = false,
	.ignore_lighting = false,
	.flammable = false,
};
