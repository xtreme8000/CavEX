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
	IB_JUMP,
	IB_INVENTORY,
	IB_HOME,
	IB_SCROLL_LEFT,
	IB_SCROLL_RIGHT,
};

void input_init(void);
void input_poll(void);

bool input_pressed(enum input_button b);
bool input_released(enum input_button b);
bool input_held(enum input_button b);
bool input_joystick(float dt, float* x, float* y);

#endif
