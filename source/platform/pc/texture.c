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

#include <GL/glew.h>
#include <string.h>

#include "../texture.h"

void tex_init_pre() { }

void tex_gfx_load(struct tex_gfx* tex, void* img, size_t width, size_t height,
				  enum tex_format type, bool linear) {
	assert(tex && img && width > 0 && height > 0);

	tex->fmt = type;
	tex->data = img;
	tex->width = width;
	tex->height = height;

	glGenTextures(1, &tex->id);

	glBindTexture(GL_TEXTURE_2D, tex->id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
				 GL_UNSIGNED_BYTE, img);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
					linear ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void tex_gfx_bind(struct tex_gfx* tex, int slot) {
	assert(tex);
	glBindTexture(GL_TEXTURE_2D, tex->id);
}

void tex_gfx_lookup(struct tex_gfx* tex, int x, int y, uint8_t* color) {
	assert(tex && color);

	memcpy(color,
		   tex->data
			   + (((unsigned int)x % tex->width)
				  + ((unsigned int)y % tex->height) * tex->width)
				   * 4,
		   4);
}