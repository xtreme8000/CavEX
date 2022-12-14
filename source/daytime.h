#ifndef DAYTIME_H
#define DAYTIME_H

#include "cglm/cglm.h"

float daytime_brightness(float time);
float daytime_celestial_angle(float time);
void daytime_sky_colors(float time, vec3 top_plane, vec3 bottom_plane,
						vec3 atmosphere);

#endif
