#include <assert.h>
#include <gccore.h>

#include "gfx.h"
#include "gfx_util.h"

void render_block_selection(mat4 view_matrix, struct block_info* this) {
	assert(this);

	int pad = 1;
	struct AABB box;

	if(!blocks[this->block->type]
	   || !blocks[this->block->type]->getBoundingBox(this, false, &box))
		return;

	gfx_lighting(false);
	gfx_blending(MODE_BLEND);
	gfx_texture(false);

	mat4 model_view;
	glm_translate_to(view_matrix, (vec3) {this->x, this->y, this->z},
					 model_view);
	gfx_matrix_modelview(model_view);

	GX_Begin(GX_LINES, GX_VTXFMT3, 24);

	// bottom
	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	// top
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	// vertical
	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_End();

	gfx_texture(true);
	gfx_lighting(true);
}
