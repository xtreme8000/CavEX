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

#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <signal.h>
#include <stdbool.h>
#include <stddef.h>

#include "../config.h"
#include "../item/window_container.h"
#include "../platform/time.h"
#include "../world.h"
#include "camera.h"
#include "gui/screen.h"

#define GAME_NAME "CavEX"
#define VERSION_MAJOR 0
#define VERSION_MINOR 2
#define VERSION_PATCH 1

struct game_state {
	sig_atomic_t quit;
	struct config config_user;
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
	uint64_t world_time;
	ptime_t world_time_start;
	struct window_container* windows[256];
	struct digging {
		bool active;
		ptime_t start;
		ptime_t cooldown;
		w_coord_t x, y, z;
	} digging;
	struct held_anim {
		struct {
			ptime_t start;
			bool place;
		} punch;
		struct {
			ptime_t start;
			size_t old_slot;
		} switch_item;
	} held_item_animation;
	bool world_loaded;
};

extern struct game_state gstate;

#endif
