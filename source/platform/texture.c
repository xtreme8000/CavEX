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
#include <stdlib.h>

#include "../lodepng/lodepng.h"
#include "texture.h"

uint8_t* tex_read(const char* filename, size_t* width, size_t* height) {
	assert(filename && width && height);

	uint8_t* img;
	unsigned w, h;
	if(lodepng_decode32_file(&img, &w, &h, filename))
		return NULL;

	*width = w;
	*height = h;

	return img;
}

void tex_gfx_load_file(const char* filename, enum tex_format type, int slot,
					   bool linear) {
	assert(filename);

	size_t width, height;
	void* img = tex_read(filename, &width, &height);

	if(!img)
		return;

	tex_gfx_load(img, width, height, type, slot, linear);

	free(img);
}

#ifdef PLATFORM_WII
#include "wii/texture.c"
#endif

#ifdef PLATFORM_PC
#include "pc/texture.c"
#endif
