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

#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum tex_format {
	TEX_FMT_RGBA16,
	TEX_FMT_RGBA32,
	TEX_FMT_I8,
	TEX_FMT_IA4,
};

uint8_t* tex_read(const char* filename, size_t* width, size_t* height);

void tex_gfx_load(void* img, size_t width, size_t height, enum tex_format type,
				  int slot, bool linear);
void tex_gfx_load_file(const char* filename, enum tex_format type, int slot,
					   bool linear);

#endif
