#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <stdbool.h>
#include <stddef.h>

struct displaylist {
	void* data;
	size_t length;
	bool dirty;
};

void displaylist_init(struct displaylist* l, size_t vertices,
					  size_t vertex_size);
void displaylist_destroy(struct displaylist* l);
void displaylist_begin(struct displaylist* l);
void displaylist_end(struct displaylist* l);
void displaylist_render(struct displaylist* l);

#endif
