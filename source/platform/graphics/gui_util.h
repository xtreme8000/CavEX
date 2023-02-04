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
	CONTROL_HOME = 9,
	CONTROL_PLUS = 10,
	CONTROL_MINUS = 11,
};

int gutil_control_icon(int x, enum gutil_control_icon icon, char* str);
void gutil_texquad_col(int x, int y, int tx, int ty, int sx, int sy, int width,
					   int height, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void gutil_texquad(int x, int y, int tx, int ty, int sx, int sy, int width,
				   int height);
void gutil_texquad_rt(int x, int y, int tx, int ty, int sx, int sy, int width,
					  int height);
void gutil_bg(void);
int gutil_font_width(char* str, int scale);
void gutil_text(int x, int y, char* str, int scale);

#endif
