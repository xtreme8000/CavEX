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

#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct displaylist {
	void* data;
	size_t length;
	size_t index;
	bool finished;
};

void displaylist_init(struct displaylist* l, size_t vertices,
					  size_t vertex_size);
void displaylist_destroy(struct displaylist* l);
void displaylist_finalize(struct displaylist* l, uint8_t primitve,
						  uint8_t vtxfmt, uint16_t vtxcnt);
void displaylist_render(struct displaylist* l);
void displaylist_render_immediate(struct displaylist* l, uint8_t primitve,
								  uint8_t vtxfmt, uint16_t vtxcnt);

void displaylist_pos(struct displaylist* l, int16_t x, int16_t y, int16_t z);
void displaylist_color(struct displaylist* l, uint8_t index);
void displaylist_texcoord(struct displaylist* l, uint8_t s, uint8_t t);

#endif
