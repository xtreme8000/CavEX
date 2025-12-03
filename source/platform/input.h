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

#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

enum input_button {
	IB_FORWARD,
	IB_BACKWARD,
	IB_LEFT,
	IB_RIGHT,
	IB_ACTION1,
	IB_ACTION2,
	IB_LOCK_CAMERA,
	IB_JUMP,
	IB_SNEAK,
	IB_INVENTORY,
	IB_HOME,
	IB_SCROLL_LEFT,
	IB_SCROLL_RIGHT,
	IB_GUI_UP,
	IB_GUI_DOWN,
	IB_GUI_LEFT,
	IB_GUI_RIGHT,
	IB_GUI_CLICK,
	IB_GUI_CLICK_ALT,
	IB_SCREENSHOT,
};

enum input_category {
	INPUT_CAT_WIIMOTE,
	INPUT_CAT_NUNCHUK,
	INPUT_CAT_CLASSIC_CONTROLLER,
	INPUT_CAT_NONE,
};

void input_init(void);
void input_poll(void);

bool input_symbol(enum input_button b, int* symbol, int* symbol_help,
				  enum input_category* category);
bool input_pressed(enum input_button b);
bool input_released(enum input_button b);
bool input_held(enum input_button b);
bool input_joystick(float dt, float* x, float* y);
void input_pointer_enable(bool enable);
bool input_pointer(float* x, float* y, float* angle);

#endif
