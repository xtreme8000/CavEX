#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <stdbool.h>
#include <stdint.h>

void gutil_texquad_col(int x, int y, int tx, int ty, int sx, int sy, int width,
					   int height, uint8_t r, uint8_t g, uint8_t b);
void gutil_texquad(int x, int y, int tx, int ty, int sx, int sy, int width,
				   int height);
void gutil_texquad_rt(int x, int y, int tx, int ty, int sx, int sy, int width,
					  int height);
void gutil_bg(void);
int gutil_font_width(char* str, int scale);
void gutil_text(int x, int y, char* str, int scale);

#endif
