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

#include <assert.h>
#include <wiiuse/wpad.h>

#include "../cglm/cglm.h"
#include "input.h"

static bool input_has_joystick;
static float input_dx, input_dy;

void input_init() {
	WPAD_Init();
	WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);
	input_has_joystick = false;
}

void input_poll() {
	WPAD_ScanPads();

	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);

	if(e.type == WPAD_EXP_NUNCHUK && e.nunchuk.js.mag > 0.1F) {
		input_dx = sin(glm_rad(e.nunchuk.js.ang)) * e.nunchuk.js.mag;
		input_dy = cos(glm_rad(e.nunchuk.js.ang)) * e.nunchuk.js.mag;
		input_has_joystick = true;
	} else {
		input_dx = input_dy = 0;
		input_has_joystick = false;
	}
}

static int input_btn_translate(enum input_button b) {
	switch(b) {
		case IB_FORWARD: return WPAD_BUTTON_UP;
		case IB_BACKWARD: return WPAD_BUTTON_DOWN;
		case IB_LEFT: return WPAD_BUTTON_LEFT;
		case IB_RIGHT: return WPAD_BUTTON_RIGHT;
		case IB_ACTION1: return WPAD_NUNCHUK_BUTTON_Z;
		case IB_ACTION2: return WPAD_NUNCHUK_BUTTON_C;
		case IB_JUMP: return WPAD_BUTTON_B;
		case IB_INVENTORY: return WPAD_BUTTON_A;
		case IB_HOME: return WPAD_BUTTON_HOME;
		case IB_SCROLL_LEFT: return WPAD_BUTTON_MINUS;
		case IB_SCROLL_RIGHT: return WPAD_BUTTON_PLUS;
		default: return 0;
	}
}

bool input_pressed(enum input_button b) {
	return WPAD_ButtonsDown(0) & input_btn_translate(b);
}

bool input_released(enum input_button b) {
	return WPAD_ButtonsUp(0) & input_btn_translate(b);
}

bool input_held(enum input_button b) {
	return WPAD_ButtonsHeld(0) & input_btn_translate(b);
}

bool input_joystick(float dt, float* x, float* y) {
	assert(x && y);
	*x = input_dx * dt;
	*y = input_dy * dt;
	return input_has_joystick;
}
