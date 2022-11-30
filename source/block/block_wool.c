#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_WOOL;
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
	if(this->block->metadata >= 9)
		return TEXTURE_INDEX(1, 13 + 9 - this->block->metadata);

	if(this->block->metadata == 8)
		return TEXTURE_INDEX(3, 13);

	if(this->block->metadata >= 1)
		return TEXTURE_INDEX(2, 13 + 1 - this->block->metadata);

	return TEXTURE_INDEX(0, 4);
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_wool = {
	.name = "Wool",
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
