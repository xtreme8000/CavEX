/*
	Copyright (c) 2023 ByteBit/xtreme8000

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

#ifndef PARTICLE_H
#define PARTICLE_H

#include "cglm/cglm.h"
#include "platform/time.h"
#include "world.h"

struct particle {
	vec3 pos;
	vec3 pos_old;
	vec3 vel;
	vec2 tex_uv;
	float size;
	int age;
};

void particle_init(void);
void particle_generate_block(struct block_info* info);
void particle_generate_side(struct block_info* info, enum side s);
void particle_update(void);
void particle_render(mat4 view, vec3 camera, float delta);

#endif
