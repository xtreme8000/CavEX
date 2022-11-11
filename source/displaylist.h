#ifndef DISPLAYLIST_H
#define DISPLAYLIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct displaylist {
	void* data;
	size_t length;
	size_t index;
	bool dirty;
};

void displaylist_init(struct displaylist* l, size_t vertices,
					  size_t vertex_size);
void displaylist_destroy(struct displaylist* l);
void displaylist_begin(struct displaylist* l, uint8_t primitve, uint8_t vtxfmt,
					   uint16_t vtxcnt);
void displaylist_end(struct displaylist* l);
void displaylist_render(struct displaylist* l);

void displaylist_pos(struct displaylist* l, int16_t x, int16_t y, int16_t z);
void displaylist_color(struct displaylist* l, uint8_t index);
void displaylist_texcoord(struct displaylist* l, uint8_t s, uint8_t t);

#endif
