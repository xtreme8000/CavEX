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

#ifndef AABB_H
#define AABB_H

#include <stdbool.h>

struct AABB {
	float x1, y1, z1;
	float x2, y2, z2;
};

struct ray {
	float x, y, z;
	float dx, dy, dz;
};

#include "blocks_data.h"

void aabb_setsize(struct AABB* a, float sx, float sy, float sz);
void aabb_setsize_centered(struct AABB* a, float sx, float sy, float sz);
void aabb_translate(struct AABB* a, float x, float y, float z);
bool aabb_intersection_ray(struct AABB* a, struct ray* r, enum side* s);
bool aabb_intersection(struct AABB* a, struct AABB* b);

#endif
