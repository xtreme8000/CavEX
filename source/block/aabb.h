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

#include "blocks.h"

void aabb_setsize(struct AABB* a, float sx, float sy, float sz);
void aabb_translate(struct AABB* a, float x, float y, float z);
bool aabb_intersection_ray(struct AABB* a, struct ray* r, enum side* s);

#endif
