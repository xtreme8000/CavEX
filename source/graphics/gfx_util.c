/*
	Copyright (c) 2023 ByteBit/xtreme8000

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

#include "../daytime.h"
#include "../game/game_state.h"
#include "../platform/gfx.h"
#include "gfx_util.h"

void gutil_clouds(mat4 view_matrix, float brightness) {
	assert(view_matrix);

	float cloud_pos = (gstate.world_time
					   + time_diff_s(gstate.world_time_start, time_get())
						   * 1000.0F / DAY_TICK_MS)
		* 12.0F / 400.0F;

	vec3 shift = {roundf(gstate.camera.x / 12.0F) * 12.0F
					  - (cloud_pos - roundf(cloud_pos / 12.0F) * 12.0F),
				  108.5F, roundf(gstate.camera.z / 12.0F) * 12.0F};

	mat4 model_view;
	glm_translate_to(view_matrix, shift, model_view);
	gfx_matrix_modelview(model_view);

	gfx_fog(true);
	gfx_fog_pos(shift[0] - gstate.camera.x, shift[2] - gstate.camera.z,
				gstate.config.fog_distance * 1.5F);
	gfx_texture(false);
	gfx_blending(MODE_BLEND);
	gfx_lighting(false);
	gfx_cull_func(MODE_NONE);
	gfx_write_buffers(false, true, true);

	int ox = roundf(gstate.camera.x / 12.0F) + roundf(cloud_pos / 12.0F);
	int oy = roundf(gstate.camera.z / 12.0F);

	uint8_t shade[6] = {
		roundf(brightness * 1.0F * 255),  roundf(brightness * 0.75F * 255),
		roundf(brightness * 0.84F * 255), roundf(brightness * 0.84F * 255),
		roundf(brightness * 0.92F * 255), roundf(brightness * 0.92F * 255),
	};

	for(int k = 0; k < 2; k++) {
		if(k == 1)
			gfx_write_buffers(true, false, true);

		for(int y = -16; y <= 16; y++) {
			for(int x = -16; x <= 16; x++) {
				uint8_t color[4];
				tex_gfx_lookup(&texture_clouds, ox + x, oy + y, color);

				if(color[3] >= 128) {
					int16_t min[] = {x * 12, y * 12, 0};
					int16_t box[] = {x * 12 + 12, y * 12 + 12, 4};

					// top, bottom, back, front, right, left
					gfx_draw_quads(
						24,
						(int16_t[]) {
							min[0], box[2], min[1], box[0], box[2], min[1],
							box[0], box[2], box[1], min[0], box[2], box[1],

							min[0], min[2], min[1], min[0], min[2], box[1],
							box[0], min[2], box[1], box[0], min[2], min[1],

							min[0], box[2], min[1], min[0], min[2], min[1],
							box[0], min[2], min[1], box[0], box[2], min[1],

							min[0], box[2], box[1], box[0], box[2], box[1],
							box[0], min[2], box[1], min[0], min[2], box[1],

							box[0], min[2], min[1], box[0], min[2], box[1],
							box[0], box[2], box[1], box[0], box[2], min[1],

							min[0], min[2], min[1], min[0], box[2], min[1],
							min[0], box[2], box[1], min[0], min[2], box[1],
						},
						(uint8_t[]) {
							shade[0], shade[0], shade[0], 0xBF,
							shade[0], shade[0], shade[0], 0xBF,
							shade[0], shade[0], shade[0], 0xBF,
							shade[0], shade[0], shade[0], 0xBF,

							shade[1], shade[1], shade[1], 0xBF,
							shade[1], shade[1], shade[1], 0xBF,
							shade[1], shade[1], shade[1], 0xBF,
							shade[1], shade[1], shade[1], 0xBF,

							shade[2], shade[2], shade[2], 0xBF,
							shade[2], shade[2], shade[2], 0xBF,
							shade[2], shade[2], shade[2], 0xBF,
							shade[2], shade[2], shade[2], 0xBF,

							shade[3], shade[3], shade[3], 0xBF,
							shade[3], shade[3], shade[3], 0xBF,
							shade[3], shade[3], shade[3], 0xBF,
							shade[3], shade[3], shade[3], 0xBF,

							shade[4], shade[4], shade[4], 0xBF,
							shade[4], shade[4], shade[4], 0xBF,
							shade[4], shade[4], shade[4], 0xBF,
							shade[4], shade[4], shade[4], 0xBF,

							shade[5], shade[5], shade[5], 0xBF,
							shade[5], shade[5], shade[5], 0xBF,
							shade[5], shade[5], shade[5], 0xBF,
							shade[5], shade[5], shade[5], 0xBF,
						},
						(uint16_t[]) {
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
						});
				}
			}
		}
	}

	gfx_write_buffers(true, true, true);
	gfx_blending(MODE_OFF);
	gfx_texture(true);
	gfx_cull_func(MODE_BACK);
	gfx_fog(false);
}

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
	float size = gstate.config.fog_distance + 4;

	gfx_draw_quads(4,
				   (int16_t[]) {-size, 16, -size, -size, 16, size, size, 16,
								size, size, 16, -size},
				   (uint8_t[]) {color_top[0], color_top[1], color_top[2], 0xFF,
								color_top[0], color_top[1], color_top[2], 0xFF,
								color_top[0], color_top[1], color_top[2], 0xFF,
								color_top[0], color_top[1], color_top[2], 0xFF},
				   (uint16_t[]) {0, 0, 0, 0, 0, 0, 0, 0});

	gfx_draw_quads(
		4,
		(int16_t[]) {-size, -32, -size, size, -32, -size, size, -32, size,
					 -size, -32, size},
		(uint8_t[]) {color_bottom[0], color_bottom[1], color_bottom[2], 0xFF,
					 color_bottom[0], color_bottom[1], color_bottom[2], 0xFF,
					 color_bottom[0], color_bottom[1], color_bottom[2], 0xFF,
					 color_bottom[0], color_bottom[1], color_bottom[2], 0xFF},
		(uint16_t[]) {0, 0, 0, 0, 0, 0, 0, 0});

	gfx_fog(false);
	gfx_texture(true);
	gfx_blending(MODE_BLEND2);
	gfx_bind_texture(&texture_gui2);

	mat4 tmp;
	glm_translate_to(view_matrix,
					 (vec3) {gstate.camera.x, gstate.camera.y, gstate.camera.z},
					 tmp);
	glm_rotate_x(tmp, glm_rad(celestial_angle * 360.0F), model_view);
	gfx_matrix_modelview(model_view);

	gfx_draw_quads(
		4, (int16_t[]) {-30, 100, -30, -30, 100, 30, 30, 100, 30, 30, 100, -30},
		(uint8_t[]) {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
					 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		(uint16_t[]) {201, 65, 201, 65 + 32, 201 + 32, 65 + 32, 201 + 32, 65});

	gfx_draw_quads(
		4,
		(int16_t[]) {-20, -100, -20, 20, -100, -20, 20, -100, 20, -20, -100,
					 20},
		(uint8_t[]) {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
					 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
		(uint16_t[]) {201, 32, 201, 32 + 32, 201 + 32, 32 + 32, 201 + 32, 32});

	gfx_blending(MODE_OFF);
	gfx_write_buffers(true, true, true);
	gfx_alpha_test(true);
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

	gfx_draw_lines(
		24,
		(int16_t[]) {
			// bottom
			-pad + box.x1 * 256, -pad + box.y1 * 256, -pad + box.z1 * 256,
			box.x2 * 256 + pad, -pad + box.y1 * 256, -pad + box.z1 * 256,

			-pad + box.x1 * 256, -pad + box.y1 * 256, -pad + box.z1 * 256,
			-pad + box.x1 * 256, -pad + box.y1 * 256, box.z2 * 256 + pad,

			box.x2 * 256 + pad, -pad + box.y1 * 256, box.z2 * 256 + pad,
			box.x2 * 256 + pad, -pad + box.y1 * 256, -pad + box.z1 * 256,

			box.x2 * 256 + pad, -pad + box.y1 * 256, box.z2 * 256 + pad,
			-pad + box.x1 * 256, -pad + box.y1 * 256, box.z2 * 256 + pad,

			// top
			-pad + box.x1 * 256, box.y2 * 256 + pad, -pad + box.z1 * 256,
			box.x2 * 256 + pad, box.y2 * 256 + pad, -pad + box.z1 * 256,

			-pad + box.x1 * 256, box.y2 * 256 + pad, -pad + box.z1 * 256,
			-pad + box.x1 * 256, box.y2 * 256 + pad, box.z2 * 256 + pad,

			box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad,
			box.x2 * 256 + pad, box.y2 * 256 + pad, -pad + box.z1 * 256,

			box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad,
			-pad + box.x1 * 256, box.y2 * 256 + pad, box.z2 * 256 + pad,

			// vertical
			-pad + box.x1 * 256, -pad + box.y1 * 256, -pad + box.z1 * 256,
			-pad + box.x1 * 256, box.y2 * 256 + pad, -pad + box.z1 * 256,

			box.x2 * 256 + pad, -pad + box.y1 * 256, -pad + box.z1 * 256,
			box.x2 * 256 + pad, box.y2 * 256 + pad, -pad + box.z1 * 256,

			-pad + box.x1 * 256, -pad + box.y1 * 256, box.z2 * 256 + pad,
			-pad + box.x1 * 256, box.y2 * 256 + pad, box.z2 * 256 + pad,

			box.x2 * 256 + pad, -pad + box.y1 * 256, box.z2 * 256 + pad,
			box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad},
		(uint8_t[]) {0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153,
					 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153,
					 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153,
					 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153,
					 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153,
					 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153, 0, 0, 0, 153});

	gfx_texture(true);
	gfx_lighting(true);
}
