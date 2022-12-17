#include <assert.h>
#include <gccore.h>
#include <malloc.h>
#include <string.h>

#include "displaylist.h"

#define GX_NOP 0
#define DISPLAYLIST_CLL 32
#define DISPLAYLIST_CACHE_LINES(v, s)                                          \
	(((v) * (s) + 3 + DISPLAYLIST_CLL - 1) / DISPLAYLIST_CLL * DISPLAYLIST_CLL)

#define MEM_U8(b, i) (*((uint8_t*)(b) + (i)))
#define MEM_U16(b, i) (*(uint16_t*)((uint8_t*)(b) + (i)))
#define MEM_I16(b, i) (*(int16_t*)((uint8_t*)(b) + (i)))

void displaylist_init(struct displaylist* l, size_t vertices,
					  size_t vertex_size) {
	assert(l && vertices > 0 && vertex_size > 0);

	l->length = DISPLAYLIST_CACHE_LINES(vertices, vertex_size);
	l->data = NULL;
	/* has 32 byte padding of GX_NOP in front for possible misalignment caused
	 * by realloc */
	l->index = DISPLAYLIST_CLL + 3;
	l->finished = false;
}

void displaylist_destroy(struct displaylist* l) {
	assert(l);

	if(l->data)
		free(l->data);
}

void displaylist_finalize(struct displaylist* l, uint8_t primitve,
						  uint8_t vtxfmt, uint16_t vtxcnt) {
	assert(l && !l->finished && l->data);

	MEM_U8(l->data, DISPLAYLIST_CLL) = primitve | (vtxfmt & 7);
	MEM_U16(l->data, DISPLAYLIST_CLL + 1) = vtxcnt;

	memset(l->data, GX_NOP, DISPLAYLIST_CLL);
	memset((uint8_t*)l->data + l->index, GX_NOP,
		   l->length + DISPLAYLIST_CLL - l->index);
	DCStoreRange(l->data, l->length + DISPLAYLIST_CLL);
	l->finished = true;
}

void displaylist_pos(struct displaylist* l, int16_t x, int16_t y, int16_t z) {
	assert(l && !l->finished);

	if(!l->data) {
		l->data = malloc(l->length + DISPLAYLIST_CLL);
		assert(l->data);
	}

	if(l->index + 9 > l->length) {
		l->length *= 2;
		l->data = realloc(l->data, l->length + DISPLAYLIST_CLL);
		assert(l->data);
	}

	MEM_U16(l->data, l->index) = x;
	l->index += 2;
	MEM_U16(l->data, l->index) = y;
	l->index += 2;
	MEM_U16(l->data, l->index) = z;
	l->index += 2;
}

void displaylist_color(struct displaylist* l, uint8_t index) {
	assert(l && !l->finished && l->data);
	MEM_U8(l->data, l->index++) = index;
}

void displaylist_texcoord(struct displaylist* l, uint8_t s, uint8_t t) {
	assert(l && !l->finished && l->data);
	MEM_U8(l->data, l->index++) = s;
	MEM_U8(l->data, l->index++) = t;
}

void displaylist_render(struct displaylist* l) {
	assert(l);

	if(l->finished)
		GX_CallDispList(
			(uint8_t*)l->data
				+ (DISPLAYLIST_CLL - (uintptr_t)l->data % DISPLAYLIST_CLL),
			(l->index + (uintptr_t)l->data % DISPLAYLIST_CLL - 1)
				/ DISPLAYLIST_CLL * DISPLAYLIST_CLL);
}

void displaylist_render_immediate(struct displaylist* l, uint8_t primitve,
								  uint8_t vtxfmt, uint16_t vtxcnt) {
	assert(l && l->data);

	uint8_t* base = (uint8_t*)l->data + DISPLAYLIST_CLL + 3;

	GX_Begin(primitve, vtxfmt, vtxcnt);
	for(uint16_t k = 0; k < vtxcnt; k++) {
		GX_Position3s16(MEM_U16(base, 0), MEM_U16(base, 2), MEM_U16(base, 4));
		GX_Color1x8(MEM_U8(base, 6));
		GX_TexCoord2u8(MEM_U8(base, 7), MEM_U8(base, 8));
		base += 9;
	}
	GX_End();
}
