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
#include <gccore.h>
#include <malloc.h>
#include <string.h>

#define rgba16(r, g, b, a)                                                     \
	(((int)(a) << 12) | ((int)(r) << 8) | ((int)(g) << 4) | (int)(b))
#define rgb16_2(r, g, b)                                                       \
	(0x8000 | ((int)(r) << 10) | ((int)(g) << 5) | (int)(b))
#define rgb16(r, g, b) (((int)(r) << 11) | ((int)(g) << 5) | (int)(b))
#define xy16(x, y) (((int)(x) << 8) | (int)(y))
#define ia8(i, a) (((int)(a) << 4) | (int)(i))

static uint8_t lut_3[256];
static uint8_t lut_4[256];
static uint8_t lut_5[256];
static uint8_t lut_6[256];

static uint8_t conv_color(uint8_t col, size_t depth) {
	return ((col << (8 - depth)) | (col >> (2 * depth - 8)));
}

static void lut_gen(int depth, uint8_t* lut) {
	assert(depth >= 1 && depth <= 8 && lut);

	for(int k = 0; k < 256; k++) {
		int best = 0;
		int best_error = INT_MAX;

		for(int i = 0; i < (1 << depth); i++) {
			int col = conv_color(i, depth);
			int error = abs(col - k);

			if(error < best_error) {
				best_error = error;
				best = i;
			}
		}

		lut[k] = best;
	}
}

void tex_init_pre() {
	lut_gen(3, lut_3);
	lut_gen(4, lut_4);
	lut_gen(5, lut_5);
	lut_gen(6, lut_6);
}

static void* tex_conv_rgba32(uint8_t* image, size_t width, size_t height) {
	assert(image && (width % 4) == 0 && (height % 4) == 0);
	uint16_t* output = memalign(32, width * height * sizeof(uint16_t) * 2);

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 4) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 4; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					output[output_idx++] = xy16(col[3], col[0]);
				}
			}

			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 4; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					output[output_idx++] = xy16(col[1], col[2]);
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint16_t) * 2);

	return output;
}

static void* tex_conv_rgba16(uint8_t* image, size_t width, size_t height) {
	assert(image && (width % 4) == 0 && (height % 4) == 0);
	uint16_t* output = memalign(32, width * height * sizeof(uint16_t));

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 4) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 4; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					int alpha = lut_3[col[3]];

					if(alpha < 7) {
						output[output_idx++] = rgba16(
							lut_4[col[0]], lut_4[col[1]], lut_4[col[2]], alpha);
					} else {
						output[output_idx++] = rgb16_2(
							lut_5[col[0]], lut_5[col[1]], lut_5[col[2]]);
					}
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint16_t));

	return output;
}

static void* tex_conv_rgb16(uint8_t* image, size_t width, size_t height) {
	assert(image && (width % 4) == 0 && (height % 4) == 0);
	uint16_t* output = memalign(32, width * height * sizeof(uint16_t));

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 4) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 4; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					output[output_idx++]
						= rgb16(lut_5[col[0]], lut_6[col[1]], lut_5[col[2]]);
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint16_t));

	return output;
}

static void* tex_conv_i8(uint8_t* image, size_t width, size_t height) {
	assert(image && (width % 8) == 0 && (height % 4) == 0);
	uint8_t* output = memalign(32, width * height * sizeof(uint8_t));

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 8) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 8; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					output[output_idx++]
						= ((int)col[0] + (int)col[1] + (int)col[2]) / 3;
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint8_t));

	return output;
}

static void* tex_conv_ia4(uint8_t* image, size_t width, size_t height) {
	assert(image && (width % 8) == 0 && (height % 4) == 0);
	uint8_t* output = memalign(32, width * height * sizeof(uint8_t));

	if(!output)
		return NULL;

	size_t output_idx = 0;

	for(size_t y = 0; y < height; y += 4) {
		for(size_t x = 0; x < width; x += 8) {
			for(size_t by = 0; by < 4; by++) {
				for(size_t bx = 0; bx < 8; bx++) {
					uint8_t* col = image + (x + bx + (y + by) * width) * 4;
					output[output_idx++] = ia8(
						lut_4[((int)col[0] + (int)col[1] + (int)col[2]) / 3],
						lut_4[col[3]]);
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint8_t));

	return output;
}

void tex_gfx_load(struct tex_gfx* tex, void* img, size_t width, size_t height,
				  enum tex_format type, bool linear) {
	assert(tex && img && width > 0 && height > 0);

	void* output = NULL;
	uint8_t fmt;

	switch(type) {
		case TEX_FMT_RGBA32:
			output = tex_conv_rgba32(img, width, height);
			fmt = GX_TF_RGBA8;
			break;
		case TEX_FMT_RGBA16:
			output = tex_conv_rgba16(img, width, height);
			fmt = GX_TF_RGB5A3;
			break;
		case TEX_FMT_RGB16:
			output = tex_conv_rgb16(img, width, height);
			fmt = GX_TF_RGB565;
			break;
		case TEX_FMT_I8:
			output = tex_conv_i8(img, width, height);
			fmt = GX_TF_I8;
			break;
		case TEX_FMT_IA4:
			output = tex_conv_ia4(img, width, height);
			fmt = GX_TF_IA4;
			break;
		default: return;
	}

	if(output) {
		tex->fmt = type;
		tex->data = output;
		tex->width = width;
		tex->height = height;

		GX_InitTexObj(&tex->obj, output, width, height, fmt, GX_CLAMP, GX_CLAMP,
					  GX_FALSE);
		GX_InitTexObjMaxAniso(&tex->obj, GX_ANISO_1);
		GX_InitTexObjFilterMode(&tex->obj, linear ? GX_LINEAR : GX_NEAR,
								linear ? GX_LINEAR : GX_NEAR);
		GX_InvalidateTexAll();
	}

	free(img);
}

void tex_gfx_wrap_mode(struct tex_gfx* tex, bool repeat) {
	assert(tex);
	GX_InitTexObjWrapMode(&tex->obj, repeat ? GX_REPEAT : GX_CLAMP,
						  repeat ? GX_REPEAT : GX_CLAMP);
}

void tex_gfx_bind(struct tex_gfx* tex, int slot) {
	assert(tex && slot >= GX_TEXMAP0 && slot < GX_MAX_TEXMAP);
	GX_LoadTexObj(&tex->obj, slot);
}

void tex_gfx_lookup(struct tex_gfx* tex, int x, int y, uint8_t* color) {
	assert(tex && color);

	x = ((unsigned int)x) % tex->width;
	y = ((unsigned int)y) % tex->height;

	switch(tex->fmt) {
		case TEX_FMT_IA4: {
			uint8_t* blk
				= tex->data + ((x / 8) + (y / 4) * (tex->width / 8)) * 32;
			uint8_t pixel = blk[(x % 8) + (y % 4) * 8];
			color[0] = color[1] = color[2] = conv_color(pixel & 0x0F, 4);
			color[3] = conv_color((pixel & 0xF0) >> 4, 4);
		} break;
		case TEX_FMT_I8: {
			uint8_t* blk
				= tex->data + ((x / 8) + (y / 4) * (tex->width / 8)) * 32;
			uint8_t pixel = blk[(x % 8) + (y % 4) * 8];
			color[0] = color[1] = color[2] = color[3] = pixel;
		} break;
		default: assert(false);
	}
}
