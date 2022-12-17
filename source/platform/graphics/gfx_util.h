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

#ifndef GFX_UTIL_H
#define GFX_UTIL_H

#include "../../cglm/cglm.h"

#include "../../world.h"

void gutil_sky_box(mat4 view_matrix, float celestial_angle, vec3 color_top,
				   vec3 color_bottom);
void gutil_block_selection(mat4 view_matrix, struct block_info* this);

#endif
