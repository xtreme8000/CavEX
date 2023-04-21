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

#define rgba16(r, g, b, a)                                                     \
	(((int)(a) << 12) | ((int)(r) << 8) | ((int)(g) << 4) | (int)(b))
#define rgb16(r, g, b) (0x8000 | ((int)(r) << 10) | ((int)(g) << 5) | (int)(b))
#define xy16(x, y) (((int)(x) << 8) | (int)(y))
#define ia8(i, a) (((int)(a) << 4) | (int)(i))

static void* tex_conv_rgb32(uint8_t* image, size_t width, size_t height) {
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
					int alpha = col[3] >> 5;

					if(alpha < 7) {
						output[output_idx++] = rgba16(col[0] >> 4, col[1] >> 4,
													  col[2] >> 4, alpha);
					} else {
						output[output_idx++]
							= rgb16(col[0] >> 3, col[1] >> 3, col[2] >> 3);
					}
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
						(((int)col[0] + (int)col[1] + (int)col[2]) / 3) >> 4,
						col[3] >> 4);
				}
			}
		}
	}

	DCFlushRange(output, width * height * sizeof(uint8_t));

	return output;
}

void tex_gfx_load(void* img, size_t width, size_t height, enum tex_format type,
				  int slot, bool linear) {
	assert(img && width > 0 && height > 0);

	void* output = NULL;
	uint8_t fmt;

	switch(type) {
		case TEX_FMT_RGBA32:
			output = tex_conv_rgb32(img, width, height);
			fmt = GX_TF_RGBA8;
			break;
		case TEX_FMT_RGBA16:
			output = tex_conv_rgb16(img, width, height);
			fmt = GX_TF_RGB5A3;
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
		GXTexObj obj;
		GX_InitTexObj(&obj, output, width, height, fmt, GX_CLAMP, GX_CLAMP,
					  GX_FALSE);
		GX_InitTexObjMaxAniso(&obj, GX_ANISO_1);
		GX_InitTexObjFilterMode(&obj, linear ? GX_LINEAR : GX_NEAR,
								linear ? GX_LINEAR : GX_NEAR);
		GX_LoadTexObj(&obj, slot);

		// TODO: data needed by gpu, must not free
		// free(output);
	}
}
