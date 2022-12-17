#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stdbool.h>
#include <stddef.h>

#include "../item/inventory.h"
#include "../world.h"
#include "camera.h"
#include "gui/screen.h"

#define GAME_NAME "CavEX"
#define VERSION_MAJOR 0
#define VERSION_MINOR 1
#define VERSION_PATCH 0

struct game_state {
	struct {
		float dt, fps;
		float dt_gpu, dt_vsync;
		size_t chunks_rendered;
	} stats;
	struct {
		float fov;
		float render_distance;
		float fog_distance;
	} config;
	struct screen* current_screen;
	struct camera camera;
	struct camera_ray_result camera_hit;
	struct world world;
	struct inventory inventory;
	bool world_loaded;
};

extern struct game_state gstate;

#endif
