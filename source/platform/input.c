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
#include "gfx.h"
#include "input.h"

#ifdef PLATFORM_PC

#include <GLFW/glfw3.h>

extern GLFWwindow* window;

static bool input_pointer_enabled;
static double input_old_pointer_x, input_old_pointer_y;
static bool input_key_held[1024];
static int joystick = -1;

void input_init() {
	for(int k = 0; k < 1024; k++)
		input_key_held[k] = false;

	input_pointer_enabled = false;
	input_old_pointer_x = 0;
	input_old_pointer_y = 0;

	// Find joystick
	glfwInit();
	for(int joy = GLFW_JOYSTICK_1; joy <= GLFW_JOYSTICK_LAST; joy++) {
		int num_axes = 0;
		const float *axes = glfwGetJoystickAxes(joy, &num_axes);
		if(num_axes < 2) continue;

		/* Workaround some buggy motherboard which reports the LED controller
		 * as a joystick; in such cases the controller is reported as having a
		 * ridiculously high number of axes. See for example:
		 * https://bbs.archlinux.org/viewtopic.php?id=261161
		 */
		if (num_axes >= 10) {
			fprintf(stderr, "Skipping controller '%s' with %d axes\n",
					glfwGetJoystickName(joy), num_axes);
			continue;
		}
		joystick = joy;
		break;
	}
}

void input_poll() { }

void input_native_key_status(int key, bool* pressed, bool* released,
							 bool* held) {
	if(key >= 1024) {
		*pressed = false;
		*released = false;
		*held = false;
	}

	int state = key < 1000 ? glfwGetKey(window, key) :
							 glfwGetMouseButton(window, key - 1000);

	*pressed = (state == GLFW_PRESS) && !input_key_held[key];
	*released = (state == GLFW_RELEASE) && input_key_held[key];
	*held = !(*released) && input_key_held[key];

	if(state == GLFW_PRESS)
		input_key_held[key] = true;

	if(state == GLFW_RELEASE)
		input_key_held[key] = false;
}

bool input_native_key_symbol(int key, int* symbol, int* symbol_help,
							 enum input_category* category, int* priority) {
	*category = INPUT_CAT_NONE;
	*symbol = 7;
	*symbol_help = 7;
	*priority = 1;
	return true;
}

bool input_native_key_any(int* key) {
	return false;
}

void input_pointer_enable(bool enable) {
	glfwSetInputMode(window, GLFW_CURSOR,
					 enable ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

	if(!input_pointer_enabled && enable)
		glfwSetCursorPos(window, gfx_width() / 2, gfx_height() / 2);

	if(input_pointer_enabled && !enable)
		glfwGetCursorPos(window, &input_old_pointer_x, &input_old_pointer_y);

	input_pointer_enabled = enable;
}

bool input_pointer(float* x, float* y, float* angle) {
	double x2, y2;
	glfwGetCursorPos(window, &x2, &y2);
	*x = x2;
	*y = y2;
	*angle = 0.0F;
	return input_pointer_enabled && x2 >= 0 && y2 >= 0 && x2 < gfx_width()
		&& y2 < gfx_height();
}

void input_native_joystick(float dt, float* dx, float* dy) {
	*dx = 0.0F;
	*dy = 0.0F;
	if(!input_pointer_enabled) {
		double x2, y2;
		glfwGetCursorPos(window, &x2, &y2);
		*dx = (x2 - input_old_pointer_x) * 0.001F;
		*dy = -(y2 - input_old_pointer_y) * 0.001F;
		input_old_pointer_x = x2;
		input_old_pointer_y = y2;
	} else if(joystick >= 0) {
		int num_axes = 0;
		const float *axes = glfwGetJoystickAxes(joystick, &num_axes);
		if(axes && num_axes >= 2) {
			*dx = axes[0] * dt;
			*dy = -axes[1] * dt;
		}
	}
}

#endif

#ifdef PLATFORM_WII

#include <wiiuse/wpad.h>

void input_init() {
	WPAD_Init();
	WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, gfx_width(), gfx_height());
}

void input_poll() {
	WPAD_ScanPads();
}

uint32_t input_wpad_translate(int key) {
	switch(key) {
		case 0: return WPAD_BUTTON_UP;
		case 1: return WPAD_BUTTON_DOWN;
		case 2: return WPAD_BUTTON_LEFT;
		case 3: return WPAD_BUTTON_RIGHT;
		case 4: return WPAD_BUTTON_A;
		case 5: return WPAD_BUTTON_B;
		case 6: return WPAD_BUTTON_1;
		case 7: return WPAD_BUTTON_2;
		case 8: return WPAD_BUTTON_PLUS;
		case 9: return WPAD_BUTTON_MINUS;
		case 10: return WPAD_BUTTON_HOME;
		default: break;
	}

	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);

	if(e.type == WPAD_EXP_NUNCHUK) {
		switch(key) {
			case 100: return WPAD_NUNCHUK_BUTTON_Z;
			case 101: return WPAD_NUNCHUK_BUTTON_C;
			default: break;
		}
	} else if(e.type == WPAD_EXP_CLASSIC) {
		switch(key) {
			case 200: return WPAD_CLASSIC_BUTTON_UP;
			case 201: return WPAD_CLASSIC_BUTTON_DOWN;
			case 202: return WPAD_CLASSIC_BUTTON_LEFT;
			case 203: return WPAD_CLASSIC_BUTTON_RIGHT;
			case 204: return WPAD_CLASSIC_BUTTON_A;
			case 205: return WPAD_CLASSIC_BUTTON_B;
			case 206: return WPAD_CLASSIC_BUTTON_X;
			case 207: return WPAD_CLASSIC_BUTTON_Y;
			case 208: return WPAD_CLASSIC_BUTTON_ZL;
			case 209: return WPAD_CLASSIC_BUTTON_ZR;
			case 210: return WPAD_CLASSIC_BUTTON_FULL_L;
			case 211: return WPAD_CLASSIC_BUTTON_FULL_R;
			case 212: return WPAD_CLASSIC_BUTTON_PLUS;
			case 213: return WPAD_CLASSIC_BUTTON_MINUS;
			case 214: return WPAD_CLASSIC_BUTTON_HOME;
			default: break;
		}
	} else if(e.type == WPAD_EXP_GUITARHERO3) {
		switch(key) {
			case 300: return WPAD_GUITAR_HERO_3_BUTTON_YELLOW;
			case 301: return WPAD_GUITAR_HERO_3_BUTTON_GREEN;
			case 302: return WPAD_GUITAR_HERO_3_BUTTON_BLUE;
			case 303: return WPAD_GUITAR_HERO_3_BUTTON_RED;
			case 304: return WPAD_GUITAR_HERO_3_BUTTON_ORANGE;
			case 305: return WPAD_GUITAR_HERO_3_BUTTON_PLUS;
			case 306: return WPAD_GUITAR_HERO_3_BUTTON_MINUS;
			case 307: return WPAD_GUITAR_HERO_3_BUTTON_STRUM_UP;
			case 308: return WPAD_GUITAR_HERO_3_BUTTON_STRUM_DOWN;
			default: break;
		}
	}

	return 0;
}

void input_native_key_status(int key, bool* pressed, bool* released,
							 bool* held) {
	*pressed = WPAD_ButtonsDown(WPAD_CHAN_0) & input_wpad_translate(key);
	*released = WPAD_ButtonsUp(WPAD_CHAN_0) & input_wpad_translate(key);
	*held = !(*pressed) && !(*released)
		&& WPAD_ButtonsHeld(WPAD_CHAN_0) & input_wpad_translate(key);
}

bool input_native_key_symbol(int key, int* symbol, int* symbol_help,
							 enum input_category* category, int* priority) {
	if(key < 0 || key > 308)
		return false;

	int symbols[] = {
		[0] = 25,	[1] = 26,	[2] = 27,	[3] = 28,	[4] = 0,	[5] = 1,
		[6] = 2,	[7] = 3,	[8] = 5,	[9] = 6,	[10] = 4,	[100] = 8,
		[101] = 9,	[200] = 25, [201] = 26, [202] = 27, [203] = 28, [204] = 10,
		[205] = 11, [206] = 12, [207] = 13, [208] = 14, [209] = 15, [210] = 22,
		[211] = 23, [212] = 5,	[213] = 6,	[214] = 4,	[300] = 7,	[301] = 7,
		[302] = 7,	[303] = 7,	[304] = 7,	[305] = 5,	[306] = 6,	[307] = 7,
		[308] = 7,
	};

	*category = INPUT_CAT_NONE;

	if(key >= 0 && key <= 10)
		*category = INPUT_CAT_WIIMOTE;

	if(key >= 100 && key <= 101)
		*category = INPUT_CAT_NUNCHUK;

	if(key >= 200 && key <= 214)
		*category = INPUT_CAT_CLASSIC_CONTROLLER;

	*symbol = symbols[key];
	*symbol_help = symbols[key];

	if(*symbol_help >= 25 && *symbol_help <= 28)
		*symbol_help = 24;

	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);

	if((*category == INPUT_CAT_NUNCHUK && e.type == WPAD_EXP_NUNCHUK)
	   || (*category == INPUT_CAT_CLASSIC_CONTROLLER
		   && e.type == WPAD_EXP_CLASSIC)) {
		*priority = 2;
	} else {
		*priority = 1;
	}

	return true;
}

bool input_native_key_any(int* key) {
	return false;
}

void input_pointer_enable(bool enable) { }

bool input_pointer(float* x, float* y, float* angle) {
	struct ir_t ir;
	WPAD_IR(WPAD_CHAN_0, &ir);
	*x = ir.x;
	*y = ir.y;
	*angle = ir.angle;
	return ir.valid;
}

void input_native_joystick(float dt, float* dx, float* dy) {
	expansion_t e;
	WPAD_Expansion(WPAD_CHAN_0, &e);

	if(e.type == WPAD_EXP_NUNCHUK && e.nunchuk.js.mag > 0.1F) {
		*dx = sin(glm_rad(e.nunchuk.js.ang)) * e.nunchuk.js.mag * dt;
		*dy = cos(glm_rad(e.nunchuk.js.ang)) * e.nunchuk.js.mag * dt;
	} else if(e.type == WPAD_EXP_CLASSIC && e.classic.rjs.mag > 0.1F) {
		*dx = sin(glm_rad(e.classic.rjs.ang)) * e.classic.rjs.mag * dt;
		*dy = cos(glm_rad(e.classic.rjs.ang)) * e.classic.rjs.mag * dt;
	} else {
		*dx = 0.0F;
		*dy = 0.0F;
	}
}

#endif

#include "../game/game_state.h"

static const char* input_config_translate(enum input_button key) {
	switch(key) {
		case IB_ACTION1: return "input.item_action_left";
		case IB_ACTION2: return "input.item_action_right";
		case IB_FORWARD: return "input.player_forward";
		case IB_BACKWARD: return "input.player_backward";
		case IB_LEFT: return "input.player_left";
		case IB_RIGHT: return "input.player_right";
		case IB_JUMP: return "input.player_jump";
		case IB_SNEAK: return "input.player_sneak";
		case IB_INVENTORY: return "input.inventory";
		case IB_HOME: return "input.open_menu";
		case IB_SCROLL_LEFT: return "input.scroll_left";
		case IB_SCROLL_RIGHT: return "input.scroll_right";
		case IB_GUI_UP: return "input.gui_up";
		case IB_GUI_DOWN: return "input.gui_down";
		case IB_GUI_LEFT: return "input.gui_left";
		case IB_GUI_RIGHT: return "input.gui_right";
		case IB_GUI_CLICK: return "input.gui_click";
		case IB_GUI_CLICK_ALT: return "input.gui_click_alt";
		default: return NULL;
	}
}

bool input_symbol(enum input_button b, int* symbol, int* symbol_help,
				  enum input_category* category) {
	const char* key = input_config_translate(b);

	if(!key)
		return false;

	size_t length = 8;
	int mapping[length];

	if(!config_read_int_array(&gstate.config_user, input_config_translate(b),
							  mapping, &length))
		return false;

	int priority = 0;
	bool has_any = false;

	for(size_t k = 0; k < length; k++) {
		int symbol_tmp, symbol_help_tmp, priority_tmp;
		enum input_category category_tmp;
		if(input_native_key_symbol(mapping[k], &symbol_tmp, &symbol_help_tmp,
								   &category_tmp, &priority_tmp)
		   && priority_tmp > priority) {
			priority = priority_tmp;
			*symbol = symbol_tmp;
			*symbol_help = symbol_help_tmp;
			*category = category_tmp;
			has_any = true;
		}
	}

	return has_any;
}

bool input_pressed(enum input_button b) {
	const char* key = input_config_translate(b);

	if(!key)
		return false;

	size_t length = 8;
	int mapping[length];

	if(!config_read_int_array(&gstate.config_user, input_config_translate(b),
							  mapping, &length))
		return false;

	bool any_pressed = false;
	bool any_held = false;
	bool any_released = false;

	for(size_t k = 0; k < length; k++) {
		bool pressed, released, held;
		input_native_key_status(mapping[k], &pressed, &released, &held);
		if(pressed)
			any_pressed = true;
		if(released)
			any_released = true;
		if(held)
			any_held = true;
	}

	return any_pressed && !any_held && !any_released;
}

bool input_released(enum input_button b) {
	const char* key = input_config_translate(b);

	if(!key)
		return false;

	size_t length = 8;
	int mapping[length];

	if(!config_read_int_array(&gstate.config_user, input_config_translate(b),
							  mapping, &length))
		return false;

	bool any_pressed = false;
	bool any_held = false;
	bool any_released = false;

	for(size_t k = 0; k < length; k++) {
		bool pressed, released, held;
		input_native_key_status(mapping[k], &pressed, &released, &held);
		if(pressed)
			any_pressed = true;
		if(released)
			any_released = true;
		if(held)
			any_held = true;
	}

	return !any_pressed && !any_held && any_released;
}

bool input_held(enum input_button b) {
	const char* key = input_config_translate(b);

	if(!key)
		return false;

	size_t length = 8;
	int mapping[length];

	if(!config_read_int_array(&gstate.config_user, input_config_translate(b),
							  mapping, &length))
		return false;

	bool any_pressed = false;
	bool any_held = false;
	bool any_released = false;

	for(size_t k = 0; k < length; k++) {
		bool pressed, released, held;
		input_native_key_status(mapping[k], &pressed, &released, &held);
		if(pressed)
			any_pressed = true;
		if(released)
			any_released = true;
		if(held)
			any_held = true;
	}

	return any_pressed || any_held;
}

bool input_joystick(float dt, float* x, float* y) {
	input_native_joystick(dt, x, y);
	return true;
}
