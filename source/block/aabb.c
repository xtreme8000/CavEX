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
#include <math.h>

#include "aabb.h"

void aabb_setsize(struct AABB* a, float sx, float sy, float sz) {
	assert(a);

	a->x1 = 0.5F - sx / 2.0F;
	a->y1 = 0;
	a->z1 = 0.5F - sz / 2.0F;

	a->x2 = 0.5F + sx / 2.0F;
	a->y2 = sy;
	a->z2 = 0.5F + sz / 2.0F;
}

void aabb_translate(struct AABB* a, float x, float y, float z) {
	assert(a);

	a->x1 += x;
	a->y1 += y;
	a->z1 += z;

	a->x2 += x;
	a->y2 += y;
	a->z2 += z;
}

// see: https://tavianator.com/2011/ray_box.html
bool aabb_intersection_ray(struct AABB* a, struct ray* r, enum side* s) {
	assert(a && r);

	float inv_x = 1.0F / r->dx;
	float tx1 = (a->x1 - r->x) * inv_x;
	float tx2 = (a->x2 - r->x) * inv_x;

	float tmin = fminf(tx1, tx2);
	float tmax = fmaxf(tx1, tx2);

	float inv_y = 1.0F / r->dy;
	float ty1 = (a->y1 - r->y) * inv_y;
	float ty2 = (a->y2 - r->y) * inv_y;

	tmin = fmaxf(tmin, fminf(fminf(ty1, ty2), tmax));
	tmax = fminf(tmax, fmaxf(fmaxf(ty1, ty2), tmin));

	float inv_z = 1.0F / r->dz;
	float tz1 = (a->z1 - r->z) * inv_z;
	float tz2 = (a->z2 - r->z) * inv_z;

	tmin = fmaxf(tmin, fminf(fminf(tz1, tz2), tmax));
	tmax = fminf(tmax, fmaxf(fmaxf(tz1, tz2), tmin));

	// is fine since fmaxf and fminf return the same value as one of the inputs
	if(s) {
		if(tmin == tx1)
			*s = SIDE_LEFT;
		else if(tmin == tx2)
			*s = SIDE_RIGHT;
		else if(tmin == ty1)
			*s = SIDE_BOTTOM;
		else if(tmin == ty2)
			*s = SIDE_TOP;
		else if(tmin == tz1)
			*s = SIDE_FRONT;
		else if(tmin == tz2)
			*s = SIDE_BACK;
	}

	return tmax > fmax(tmin, 0.0F);
}
