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
	return face_occlusion_empty();
}

/*static struct rectangle getSideMask(struct block_info* this, enum side side,
struct block_info* it) { switch(side) { case SIDE_TOP: return
rectangle_from(0,0,16,8); case SIDE_BOTTOM: return rectangle_full(); case
SIDE_LEFT: return rectangle_from(0,8,16,8); case SIDE_RIGHT: return
rectangle_full(); case SIDE_FRONT: case SIDE_BACK:
		{
			struct rectangle rect;
			rect.length = 2;
			rectangle_from2(&rect,0,0,8,16,8);
			rectangle_from2(&rect,1,8,0,8,8);
			return rect;
		}
	}
}*/

static enum block_render_type getRenderType(struct block_info* this) {
	return RENDERTYPE_STAIRS;
}

static uint8_t getTextureIndex(struct block_info* this, enum side side) {
	return TEXTURE_INDEX(4, 0);
}

static uint32_t getBaseColor(struct block_info* this, enum side side) {
	return 0xFFFFFF;
}

struct block block_stairs = {
	.name = "Stairs",
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
	.can_see_through = true,
	.ignore_lighting = true,
};
