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

#ifdef PLATFORM_WII
#include <gccore.h>
#endif

#ifdef PLATFORM_PC
#include <GL/glew.h>
#endif

enum tex_format {
	TEX_FMT_RGB16,
	TEX_FMT_RGBA16,
	TEX_FMT_RGBA32,
	TEX_FMT_I8,
	TEX_FMT_IA4,
};

struct tex_gfx {
#ifdef PLATFORM_WII
	GXTexObj obj;
#endif
#ifdef PLATFORM_PC
	GLuint id;
#endif
	size_t width, height;
	enum tex_format fmt;
	uint8_t* data;
};

extern struct tex_gfx texture_fog;
extern struct tex_gfx texture_terrain;
extern struct tex_gfx texture_items;
extern struct tex_gfx texture_font;
extern struct tex_gfx texture_anim;
extern struct tex_gfx texture_gui_inventory;
extern struct tex_gfx texture_gui2;
extern struct tex_gfx texture_controls;
extern struct tex_gfx texture_pointer;
extern struct tex_gfx texture_clouds;
extern struct tex_gfx texture_sun;
extern struct tex_gfx texture_moon;

void tex_init(void);

uint8_t* tex_read(const char* filename, size_t* width, size_t* height);


void tex_gfx_load(struct tex_gfx* tex, void* img, size_t width, size_t height,
				  enum tex_format type, bool linear);
void tex_gfx_load_file(struct tex_gfx* tex, const char* filename,
					   enum tex_format type, bool linear);
void tex_gfx_bind(struct tex_gfx* tex, int slot);
void tex_gfx_lookup(struct tex_gfx* tex, int x, int y, uint8_t* color);

#endif
