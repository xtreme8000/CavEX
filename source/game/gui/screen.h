#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

#include "../../cglm/cglm.h"

struct screen {
	void (*reset)(struct screen* s);
	void (*update)(struct screen* s, float dt);
	void (*render2D)(struct screen* s, int width, int height);
	void (*render3D)(struct screen* s, mat4 view);
	bool render_world;
	void* user;
};

extern struct screen screen_ingame;

void screen_set(struct screen* s);

#endif
