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

#include <GL/glew.h>
#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "../displaylist.h"

#define MEM_U8(b, i) (*((uint8_t*)(b) + (i)))
#define MEM_U16(b, i) (*(uint16_t*)((uint8_t*)(b) + (i)))
#define MEM_I16(b, i) (*(int16_t*)((uint8_t*)(b) + (i)))
#define MEM_FLT(b, i) (*(float*)((uint8_t*)(b) + (i)))

#define VERTEX_SIZE(l) ((l)->direct_color ? 24 : 22)

void displaylist_init(struct displaylist* l, size_t vertices,
					  bool direct_color) {
	assert(l && vertices > 0);

	l->direct_color = direct_color;
	l->length = 4096;
	l->data = NULL;
	l->index = 0;
	l->finished = false;
}

void displaylist_destroy(struct displaylist* l) {
	assert(l);

	if(l->data)
		free(l->data);

	if(l->finished)
		glDeleteBuffers(1, &l->vbo);
}

void displaylist_reset(struct displaylist* l) {
	assert(l && !l->finished);
	l->index = 0;
}

void displaylist_finalize(struct displaylist* l, uint16_t vtxcnt) {
	assert(l && !l->finished && l->data);

	l->index = vtxcnt;
}

void displaylist_pos(struct displaylist* l, int16_t x, int16_t y, int16_t z) {
	assert(l && !l->finished);

	if(!l->data) {
		l->data = malloc(l->length);
		assert(l->data);
	}

	if(l->index + VERTEX_SIZE(l) > l->length) {
		l->length *= 2;
		l->data = realloc(l->data, l->length);
		assert(l->data);
	}

	MEM_FLT(l->data, l->index) = (float)x / 256.0F;
	l->index += 4;
	MEM_FLT(l->data, l->index) = (float)y / 256.0F;
	l->index += 4;
	MEM_FLT(l->data, l->index) = (float)z / 256.0F;
	l->index += 4;
}

void displaylist_color(struct displaylist* l, uint8_t index) {
	assert(l && !l->finished && l->data);

	MEM_U8(l->data, l->index++) = index % 16;
	MEM_U8(l->data, l->index++) = index / 16;
}

void displaylist_color_rgba(struct displaylist* l, uint8_t r, uint8_t g,
							uint8_t b, uint8_t a) {
	assert(l && !l->finished && l->data);

	MEM_U8(l->data, l->index++) = r;
	MEM_U8(l->data, l->index++) = g;
	MEM_U8(l->data, l->index++) = b;
	MEM_U8(l->data, l->index++) = a;
}

void displaylist_texcoord(struct displaylist* l, uint8_t s, uint8_t t) {
	assert(l && !l->finished && l->data);
	MEM_FLT(l->data, l->index) = (float)s / 256.0F;
	l->index += 4;
	MEM_FLT(l->data, l->index) = (float)t / 256.0F;
	l->index += 4;
}

void displaylist_render(struct displaylist* l) {
	assert(l);

	if(!l->finished) {
		l->finished = true;

		glGenBuffers(1, &l->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, l->vbo);
		glBufferData(GL_ARRAY_BUFFER, l->index * VERTEX_SIZE(l), l->data,
					 GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(l->direct_color ? 1 : 3);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, l->vbo);

	if(l->direct_color) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, NULL);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 24,
							  (uint8_t*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 24,
							  (uint8_t*)(4 + 3 * sizeof(float)));
	} else {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 22, NULL);
		glVertexAttribPointer(3, 2, GL_UNSIGNED_BYTE, GL_FALSE, 22,
							  (uint8_t*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 22,
							  (uint8_t*)(2 + 3 * sizeof(float)));
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDrawArrays(GL_QUADS, 0, l->index);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(l->direct_color ? 1 : 3);
	glDisableVertexAttribArray(2);
}

void displaylist_render_immediate(struct displaylist* l, uint16_t vtxcnt) {
	assert(l && l->data && !l->finished);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(l->direct_color ? 1 : 3);
	glEnableVertexAttribArray(2);

	if(l->direct_color) {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, l->data);
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 24,
							  (uint8_t*)l->data + 3 * sizeof(float));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 24,
							  (uint8_t*)l->data + 4 + 3 * sizeof(float));
	} else {
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 22, l->data);
		glVertexAttribPointer(3, 2, GL_UNSIGNED_BYTE, GL_FALSE, 22,
							  (uint8_t*)l->data + 3 * sizeof(float));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 22,
							  (uint8_t*)l->data + 2 + 3 * sizeof(float));
	}

	glDrawArrays(GL_QUADS, 0, vtxcnt);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(l->direct_color ? 1 : 3);
	glDisableVertexAttribArray(2);
}
