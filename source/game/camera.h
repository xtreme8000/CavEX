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

#include "../world.h"

struct camera_ray_result {
	bool hit;
	w_coord_t x, y, z;
	enum side side;
};

void camera_ray_pick(struct world* w, float gx0, float gy0, float gz0,
					 float gx1, float gy1, float gz1,
					 struct camera_ray_result* res);
void camera_update(struct camera* c, float dt);

#endif
