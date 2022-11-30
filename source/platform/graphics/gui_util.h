#ifndef GUI_UTIL_H
#define GUI_UTIL_H

#include <stdbool.h>
#include <stdint.h>

enum gutil_control_icon {
	CONTROL_A = 0,
	CONTROL_B = 1,
	CONTROL_1 = 2,
	CONTROL_2 = 3,
	CONTROL_DPAD = 4,
	CONTROL_MOTION = 5,
	CONTROL_Z = 6,
	CONTROL_C = 7,
	CONTROL_JOYSTICK = 8,
	CONTROL_PLUS = 10,
	CONTROL_MINUS = 11,
};

int gutil_control_icon(int x, enum gutil_control_icon icon, char* str);
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
