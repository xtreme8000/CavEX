#include <assert.h>
#include <gccore.h>
#include <malloc.h>

#include "displaylist.h"

#define DISPLAYLIST_CLL 32
#define DISPLAYLIST_CACHE_LINES(v, s)                                          \
	(((v) * (s) + 3 + DISPLAYLIST_CLL - 1) / DISPLAYLIST_CLL * DISPLAYLIST_CLL)

void displaylist_init(struct displaylist* l, size_t vertices,
					  size_t vertex_size) {
	assert(l && vertices > 0 && vertex_size > 0);

	l->length = DISPLAYLIST_CACHE_LINES(vertices, vertex_size);
	l->data = memalign(DISPLAYLIST_CLL, l->length);
	DCInvalidateRange(l->data, l->length);
}

void displaylist_destroy(struct displaylist* l) {
	assert(l);

	free(l->data);
}

void displaylist_begin(struct displaylist* l) {
	assert(l);

	GX_BeginDispList(l->data, l->length);
}

void displaylist_end(struct displaylist* l) {
	assert(l);

	// has bug when exact size is given to GX_BeginDispList() (see docs), don't
	// check result
	GX_EndDispList();
}

void displaylist_render(struct displaylist* l) {
	assert(l);

	GX_CallDispList(l->data, l->length);
}
