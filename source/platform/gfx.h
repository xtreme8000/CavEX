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

#ifndef GFX_H
#define GFX_H

#include <stdbool.h>
#include <stdint.h>

#include "../cglm/cglm.h"
#include "texture.h"

enum gfx_blend {
	MODE_BLEND,
	MODE_BLEND2,
	MODE_BLEND3,
	MODE_INVERT,
	MODE_OFF,
};

enum depth_func {
	MODE_LEQUAL,
	MODE_EQUAL,
};

enum cull_func {
	MODE_NONE,
	MODE_BACK,
	MODE_FRONT,
};

void gfx_setup(void);
void gfx_update_light(float daytime, const float* light_lookup);
float gfx_lookup_light(uint8_t light);
void gfx_finish(bool vsync);
void gfx_flip_buffers(float* gpu_wait, float* vsync_wait);
void gfx_bind_texture(struct tex_gfx* tex);
void gfx_clear_buffers(uint8_t r, uint8_t g, uint8_t b);
int gfx_width(void);
int gfx_height(void);

void gfx_copy_framebuffer(uint8_t* dest, size_t* width, size_t* height);

void gfx_matrix_projection(mat4 proj, bool is_perspective);
void gfx_matrix_modelview(mat4 mv);
void gfx_matrix_texture(bool enable, mat4 tex);

void gfx_mode_world(void);
void gfx_mode_gui(void);
void gfx_fog_color(uint8_t r, uint8_t g, uint8_t b);
void gfx_fog_pos(float dx, float dz, float distance);
void gfx_fog(bool enable);
void gfx_blending(enum gfx_blend mode);
void gfx_alpha_test(bool enable);
void gfx_write_buffers(bool color, bool depth, bool depth_test);
void gfx_depth_range(float near, float far);
void gfx_depth_func(enum depth_func func);
void gfx_texture(bool enable);
void gfx_lighting(bool enable);
void gfx_cull_func(enum cull_func func);
void gfx_scissor(bool enable, uint32_t x, uint32_t y, uint32_t width,
				 uint32_t height);
void gfx_draw_lines(size_t vertex_count, const int16_t* vertices,
					const uint8_t* colors);
void gfx_draw_quads(size_t vertex_count, const int16_t* vertices,
					const uint8_t* colors, const uint16_t* texcoords);
void gfx_draw_quads_flt(size_t vertex_count, const float* vertices,
						const uint8_t* colors, const float* texcoords);

#endif
