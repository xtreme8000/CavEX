#ifndef GFX_UTIL_H
#define GFX_UTIL_H

#include "../../cglm/cglm.h"

#include "../../world.h"

void gutil_sky_box(mat4 view_matrix, float celestial_angle, vec3 color_top,
				   vec3 color_bottom);
void gutil_block_selection(mat4 view_matrix, struct block_info* this);

#endif
