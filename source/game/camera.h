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

#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

#include "../cglm/cglm.h"

struct camera {
	float x, y, z;
	float rx, ry;
	mat4 view, projection;
	vec4 frustum_planes[6];
	struct camera_controller {
		float vx, vy, vz;
	} controller;
};

#include "../entity/entity.h"
#include "../world.h"

struct camera_ray_result {
	bool hit;
	w_coord_t x, y, z;
	enum side side;
};

void camera_ray_pick(struct world* w, float gx0, float gy0, float gz0,
					 float gx1, float gy1, float gz1,
					 struct camera_ray_result* res);
void camera_physics(struct camera* c, float dt);
void camera_update(struct camera* c);
void camera_attach(struct camera* c, struct entity* e, float tick_delta,
				   float dt);

#endif
