#include "blocks.h"

static enum block_material getMaterial(struct block_info* this) {
	return MATERIAL_STONE;
}

static bool getBoundingBox(struct block_info* this, bool entity,
						   struct AABB* x) {
	aabb_setsize(x, 1.0F,
				 ((this->block->metadata & 0x7) > 1
				  && (this->block->metadata & 0x7) < 6) ?
					 0.625F :
					 0.125F,
				 1.0F);
	return !entity;
}

static struct face_occlusion*
getSideMask(struct block_info* this, enum side side, struct block_info* it) {
	return face_occlusion_empty();
}

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_FLAT;
}

static uint8_t getTextureIndex1(struct block_info* this, enum side side) {
	return (this->block->metadata < 6) ? TEXTURE_INDEX(0, 8) :
										 TEXTURE_INDEX(0, 7);
}

static uint8_t getTextureIndex2(struct block_info* this, enum side side) {
	return (this->block->metadata & 0x8) ? TEXTURE_INDEX(3, 11) :
										   TEXTURE_INDEX(3, 10);
}

static uint8_t getTextureIndex3(struct block_info* this, enum side side) {
	return TEXTURE_INDEX(3, 12);
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_rail = {
	.name = "Rail",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex1,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_rail,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.render_block_data.rail_curved_possible = true,
	.ignore_lighting = false,
	.flammable = false,
};

struct block block_powered_rail = {
	.name = "Powered rail",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex2,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_rail,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.render_block_data.rail_curved_possible = false,
	.ignore_lighting = false,
	.flammable = false,
};

struct block block_detector_rail = {
	.name = "Detector rail",
	.getRenderType = getRenderType,
	.getSideMask = getSideMask,
	.getBoundingBox = getBoundingBox,
	.getMaterial = getMaterial,
	.getTextureIndex = getTextureIndex3,
	.transparent = false,
	.getBaseColor = getBaseColor,
	.renderBlock = render_block_rail,
	.renderBlockAlways = NULL,
	.luminance = 0,
	.double_sided = true,
	.can_see_through = true,
	.render_block_data.rail_curved_possible = false,
	.ignore_lighting = false,
	.flammable = false,
};
