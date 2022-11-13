#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_WOOD;
}

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	aabb_setsize(x, 1.0F, 0.5625F, 1.0F);
	return true;
}

// TODO: add missing bed legs to mask? (difficult, many states)
struct face_occlusion side_mask = {
	.mask = {0x00000000, 0x00000000, 0x00000000, 0x0000FFFF, 0xFFFFFFFF,
			 0xFFFFFFFF, 0xFFFF0000, 0x00000000},
};

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	switch(side) {
		case SIDE_TOP:
		case SIDE_BOTTOM: return face_occlusion_empty();
		default: return &side_mask;
	}
}

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_FULL;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	if(this->block->metadata & 0x8) {
		switch(side) {
			case SIDE_TOP: return TEXTURE_INDEX(7, 8);
			case SIDE_LEFT:
			case SIDE_RIGHT:
				return (this->block->metadata & 0x1) ? TEXTURE_INDEX(11, 8) :
													   TEXTURE_INDEX(10, 8);
			case SIDE_FRONT:
			case SIDE_BACK:
				return (this->block->metadata & 0x1) ? TEXTURE_INDEX(10, 8) :
													   TEXTURE_INDEX(11, 8);
			default: return TEXTURE_INDEX(4, 0);
		}
	} else {
		switch(side) {
			case SIDE_TOP: return TEXTURE_INDEX(6, 8);
			case SIDE_LEFT:
			case SIDE_RIGHT:
				return (this->block->metadata & 0x1) ? TEXTURE_INDEX(8, 8) :
													   TEXTURE_INDEX(9, 8);
			case SIDE_FRONT:
			case SIDE_BACK:
				return (this->block->metadata & 0x1) ? TEXTURE_INDEX(9, 8) :
													   TEXTURE_INDEX(8, 8);
			default: return TEXTURE_INDEX(4, 0);
		}
	}
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_bed = {
	.name = "Bed",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_bed,
	.luminance = 0,
	.double_sided = false,
	.can_see_through = true,
	.ignore_lighting = false,
	.flammable = false,
};
