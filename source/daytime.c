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

#include "daytime.h"
#include "util.h"

float daytime_brightness(float time) {
	return glm_clamp(cos(daytime_celestial_angle(time) * 2.0F * GLM_PI) * 2.0F
						 + 0.5F,
					 0.0F, 1.0F);
}

float daytime_celestial_angle(float time) {
	float X = time - 0.25F;

	if(X < 0)
		X += 1;

	return X + ((1.0F - (cos(X * GLM_PI) + 1.0F) / 2.0F) - X) / 3.0F;
}

void daytime_sky_colors(float time, vec3 top_plane, vec3 bottom_plane,
						vec3 atmosphere) {
	assert(top_plane && bottom_plane && atmosphere);

	float brightness_mul = daytime_brightness(time);

	vec3 world_sky_color = {
		0.6222222F - (0.7F / 3.0F) * 0.05F,
		0.5F + (0.7F / 3.0F) * 0.1F,
		1.0F,
	};

	hsv2rgb(world_sky_color + 0, world_sky_color + 1, world_sky_color + 2);
	glm_vec3_scale(world_sky_color, brightness_mul, world_sky_color);

	vec3 fog_color = {
		0.7529412F * brightness_mul * 0.94F + 0.06F,
		0.8470588F * brightness_mul * 0.94F + 0.06F,
		1.0F * brightness_mul * 0.91F + 0.09F,
	};

	vec3 atmosphere_color;
	glm_vec3_lerp(fog_color, world_sky_color, 0.29F, atmosphere_color);
	glm_vec3_scale(atmosphere_color,
				   powf(0.8F, (1.0F - brightness_mul) * 11.0F),
				   atmosphere_color);

	vec3 bottom_plane_color = {0.04F, 0.04F, 0.1F};
	glm_vec3_muladd(world_sky_color, (vec3) {0.2F, 0.2F, 0.6F},
					bottom_plane_color);

	glm_vec3_scale(atmosphere_color, 255.0F, atmosphere_color);
	glm_vec3_scale(world_sky_color, 255.0F, world_sky_color);
	glm_vec3_scale(bottom_plane_color, 255.0F, bottom_plane_color);

	glm_vec3_copy(world_sky_color, top_plane);
	glm_vec3_copy(bottom_plane_color, bottom_plane);
	glm_vec3_copy(atmosphere_color, atmosphere);
}
