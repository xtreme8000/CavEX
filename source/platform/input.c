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

#include "../cglm/cglm.h"
#include "input.h"

#ifdef PLATFORM_PC

#include <GLFW/glfw3.h>

extern GLFWwindow* window;

static bool input_key_held[IB_MAX];
static bool input_key_pressed[IB_MAX];
static bool input_key_released[IB_MAX];
static float joystick_x = 0, joystick_y = 0;

void input_init() {
	for(size_t k = 0; k < IB_MAX; k++) {
		input_key_held[k] = false;
		input_key_pressed[k] = false;
		input_key_released[k] = false;
	}
}

void input_poll() {
	for(size_t k = 0; k < IB_MAX; k++) {
		input_key_pressed[k] = false;
		input_key_released[k] = false;
	}
}

void input_set_status(enum input_button b, bool pressed) {
	assert(b < IB_MAX);
	input_key_held[b] = pressed;
	input_key_pressed[b] = pressed;
	input_key_released[b] = !pressed;
}

void input_set_joystick(float x, float y) {
	joystick_x = x;
	joystick_y = y;
}

bool input_pressed(enum input_button b) {
	return input_key_pressed[b];
}

bool input_released(enum input_button b) {
	return input_key_released[b];
}

bool input_held(enum input_button b) {
	return input_key_held[b];
}

bool input_joystick(float dt, float* x, float* y) {
	*x = joystick_x;
	*y = joystick_y;
	return true;
}

#endif

#ifdef PLATFORM_WII

#include <wiiuse/wpad.h>

static bool input_has_wpad;
static bool input_has_pad;
static bool input_has_joystick;
static float input_dx, input_dy;

void input_init() {
	input_has_wpad = WPAD_Init() == WPAD_ERR_NONE;
	input_has_pad = true;
	PAD_Init(); // always returns 1?
	input_dx = input_dy = 0;
	input_has_joystick = false;
	WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);
}

void input_poll() {
	input_dx = input_dy = 0;
	input_has_joystick = false;

	if(input_has_wpad) {
		WPAD_ScanPads();

		expansion_t e;
		WPAD_Expansion(WPAD_CHAN_0, &e);

		if(e.type == WPAD_EXP_NUNCHUK && e.nunchuk.js.mag > 0.1F) {
			input_dx = sin(glm_rad(e.nunchuk.js.ang)) * e.nunchuk.js.mag;
			input_dy = cos(glm_rad(e.nunchuk.js.ang)) * e.nunchuk.js.mag;
			input_has_joystick = true;
		}
	}

	if(input_has_pad) {
		PAD_ScanPads();

		float dx = (float)PAD_SubStickX(PAD_CHAN0) / 128.0F;
		float dy = (float)PAD_SubStickY(PAD_CHAN0) / 128.0F;

		if(glm_vec2_distance((vec2) {dx, dy}, GLM_VEC3_ZERO) > glm_pow2(0.1F)) {
			input_dx = dx;
			input_dy = dy;
			input_has_joystick = true;
		}
	}
}

static int input_wpad_translate(enum input_button b) {
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

static int input_pad_translate(enum input_button b) {
	switch(b) {
		case IB_FORWARD: return PAD_BUTTON_UP;
		case IB_BACKWARD: return PAD_BUTTON_DOWN;
		case IB_LEFT: return PAD_BUTTON_LEFT;
		case IB_RIGHT: return PAD_BUTTON_RIGHT;
		case IB_ACTION1: return PAD_BUTTON_A;
		case IB_ACTION2: return PAD_BUTTON_B;
		case IB_JUMP: return PAD_TRIGGER_L;
		case IB_INVENTORY: return PAD_BUTTON_X;
		case IB_HOME: return PAD_BUTTON_START;
		case IB_SCROLL_LEFT: return PAD_TRIGGER_Z;
		case IB_SCROLL_RIGHT: return PAD_TRIGGER_R;
		default: return 0;
	}
}

bool input_pressed(enum input_button b) {
	return (input_has_wpad
			&& (WPAD_ButtonsDown(WPAD_CHAN_0) & input_wpad_translate(b)))
		|| (input_has_pad
			&& (PAD_ButtonsDown(PAD_CHAN0) & input_pad_translate(b)));
}

bool input_released(enum input_button b) {
	return (input_has_wpad
			&& (WPAD_ButtonsUp(WPAD_CHAN_0) & input_wpad_translate(b)))
		|| (input_has_pad
			&& (PAD_ButtonsUp(PAD_CHAN0) & input_pad_translate(b)));
}

bool input_held(enum input_button b) {
	return (input_has_wpad
			&& (WPAD_ButtonsHeld(WPAD_CHAN_0) & input_wpad_translate(b)))
		|| (input_has_pad
			&& (PAD_ButtonsHeld(PAD_CHAN0) & input_pad_translate(b)));
}

bool input_joystick(float dt, float* x, float* y) {
	assert(x && y);
	*x = input_dx * dt;
	*y = input_dy * dt;
	return input_has_joystick;
}

#endif
