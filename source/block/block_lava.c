#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_STONE;
}

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	int block_height = (this->block->metadata & 0x8) ?
		16 :
		(8 - this->block->metadata) * 2 * 7 / 8;
	aabb_setsize(x, 1.0F, (float)block_height / 16.0F, 1.0F);
	return false;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	int block_height = (this->block->metadata & 0x8) ?
		16 :
		(8 - this->block->metadata) * 2 * 7 / 8;
	switch(side) {
		case SIDE_TOP:
			return (it->block->type == this->block->type) ?
				face_occlusion_full() :
				face_occlusion_empty();
		case SIDE_BOTTOM: return face_occlusion_full();
		default:
			return (world_get_block(this->world, this->x, this->y + 1, this->z)
						.type
					== this->block->type) ?
				face_occlusion_full() :
				face_occlusion_rect(block_height);
	}
}

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_FLUID;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	return TEXTURE_INDEX(2, 0);
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_lava = {
	.name = "Lava",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = true,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_fluid,
	.luminance = 15,
	.double_sided = false,
	.can_see_through = true,
	.ignore_lighting = false,
};
