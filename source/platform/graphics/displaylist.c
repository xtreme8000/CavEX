#include <assert.h>
#include <gccore.h>
#include <malloc.h>
#include <string.h>

#include "displaylist.h"

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
	l->data = memalign(DISPLAYLIST_CLL, l->length);
	l->index = 0;
	l->finished = false;
}

void displaylist_destroy(struct displaylist* l) {
	assert(l);

	free(l->data);
}

void displaylist_begin(struct displaylist* l, uint8_t primitve, uint8_t vtxfmt,
					   uint16_t vtxcnt) {
	assert(l && !l->finished);
	uint8_t reg = primitve | (vtxfmt & 7);
	MEM_U8(l->data, l->index++) = reg;
	MEM_U16(l->data, l->index) = vtxcnt;
	l->index += 2;
}

void displaylist_end(struct displaylist* l) {
	assert(l && !l->finished);

	memset((uint8_t*)l->data + l->index, 0, l->length - l->index);
	DCStoreRange(l->data, l->length);
	l->finished = true;
}

void displaylist_pos(struct displaylist* l, int16_t x, int16_t y, int16_t z) {
	assert(l && !l->finished);
	MEM_U16(l->data, l->index) = x;
	l->index += 2;
	MEM_U16(l->data, l->index) = y;
	l->index += 2;
	MEM_U16(l->data, l->index) = z;
	l->index += 2;
}

void displaylist_color(struct displaylist* l, uint8_t index) {
	assert(l && !l->finished);
	MEM_U8(l->data, l->index++) = index;
}

void displaylist_texcoord(struct displaylist* l, uint8_t s, uint8_t t) {
	assert(l && !l->finished);
	assert(!l->finished);
	MEM_U8(l->data, l->index++) = s;
	MEM_U8(l->data, l->index++) = t;
}

void displaylist_render(struct displaylist* l) {
	assert(l);

	if(l->finished)
		GX_CallDispList(l->data, l->length);
}
