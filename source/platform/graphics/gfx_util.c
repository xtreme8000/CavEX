/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	CavEX is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with CavEX.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <gccore.h>

#include "../../game/game_state.h"
#include "gfx.h"
#include "gfx_util.h"

void gutil_sky_box(mat4 view_matrix, float celestial_angle, vec3 color_top,
				   vec3 color_bottom) {
	assert(view_matrix && color_top && color_bottom);

	gfx_alpha_test(false);
	gfx_write_buffers(true, false, false);
	gfx_blending(MODE_OFF);
	gfx_lighting(false);
	gfx_texture(false);
	gfx_fog(true);

	mat4 model_view;
	glm_translate_to(view_matrix,
					 (vec3) {gstate.camera.x, gstate.camera.y, gstate.camera.z},
					 model_view);
	gfx_matrix_modelview(model_view);
	gfx_fog_pos(0, 0, gstate.config.fog_distance);

	// render a bit larger for possible inaccuracy
	int16_t size = (gstate.config.fog_distance + 4) * 256;

	GX_Begin(GX_QUADS, GX_VTXFMT3, 8);
	GX_Position3s16(-size, 16 * 256, -size);
	GX_Color4u8(color_top[0], color_top[1], color_top[2], 255);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-size, 16 * 256, size);
	GX_Color4u8(color_top[0], color_top[1], color_top[2], 255);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(size, 16 * 256, size);
	GX_Color4u8(color_top[0], color_top[1], color_top[2], 255);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(size, 16 * 256, -size);
	GX_Color4u8(color_top[0], color_top[1], color_top[2], 255);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-size, -32 * 256, -size);
	GX_Color4u8(color_bottom[0], color_bottom[1], color_bottom[2], 255);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(size, -32 * 256, -size);
	GX_Color4u8(color_bottom[0], color_bottom[1], color_bottom[2], 255);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(size, -32 * 256, size);
	GX_Color4u8(color_bottom[0], color_bottom[1], color_bottom[2], 255);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-size, -32 * 256, size);
	GX_Color4u8(color_bottom[0], color_bottom[1], color_bottom[2], 255);
	GX_TexCoord2u8(0, 0);
	GX_End();

	gfx_blending(MODE_BLEND2);
	gfx_fog(false);
	gfx_texture(true);
	gfx_bind_texture(TEXTURE_GUI2);

	mat4 tmp;
	glm_translate_to(view_matrix,
					 (vec3) {gstate.camera.x, gstate.camera.y, gstate.camera.z},
					 tmp);
	glm_rotate_x(tmp, glm_rad(celestial_angle * 360.0F), model_view);
	gfx_matrix_modelview(model_view);

	GX_Begin(GX_QUADS, GX_VTXFMT3, 8);
	GX_Position3s16(-30 * 256, 100 * 256, -30 * 256);
	GX_Color4u8(255, 255, 255, 255);
	GX_TexCoord2u8(201, 65);
	GX_Position3s16(-30 * 256, 100 * 256, 30 * 256);
	GX_Color4u8(255, 255, 255, 255);
	GX_TexCoord2u8(201, 65 + 32);
	GX_Position3s16(30 * 256, 100 * 256, 30 * 256);
	GX_Color4u8(255, 255, 255, 255);
	GX_TexCoord2u8(201 + 32, 65 + 32);
	GX_Position3s16(30 * 256, 100 * 256, -30 * 256);
	GX_Color4u8(255, 255, 255, 255);
	GX_TexCoord2u8(201 + 32, 65);

	GX_Position3s16(-20 * 256, -100 * 256, -20 * 256);
	GX_Color4u8(255, 255, 255, 255);
	GX_TexCoord2u8(201, 32);
	GX_Position3s16(20 * 256, -100 * 256, -20 * 256);
	GX_Color4u8(255, 255, 255, 255);
	GX_TexCoord2u8(201 + 32, 32);
	GX_Position3s16(20 * 256, -100 * 256, 20 * 256);
	GX_Color4u8(255, 255, 255, 255);
	GX_TexCoord2u8(201 + 32, 32 + 32);
	GX_Position3s16(-20 * 256, -100 * 256, 20 * 256);
	GX_Color4u8(255, 255, 255, 255);
	GX_TexCoord2u8(201, 32 + 32);
	GX_End();

	gfx_blending(MODE_OFF);
	gfx_write_buffers(true, true, true);
}

void gutil_block_selection(mat4 view_matrix, struct block_info* this) {
	assert(view_matrix && this);

	int pad = 1;
	struct AABB box;

	if(!blocks[this->block->type]
	   || !blocks[this->block->type]->getBoundingBox(this, false, &box))
		return;

	gfx_fog(false);
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
