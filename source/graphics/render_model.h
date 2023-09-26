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

#ifndef RENDER_MODEL_H
#define RENDER_MODEL_H

#include "../item/items.h"
#include "../platform/gfx.h"

void render_model_box(mat4 view, vec3 position, vec3 pivot, vec3 rotation,
					  ivec2 origin, ivec3 box, float padding, bool mirror,
					  float brightness);
void render_model_player(mat4 mv, float head_pitch, float head_yaw,
						 float foot_angle, float arm_angle,
						 struct item_data* held_item, struct item_data* helmet,
						 struct item_data* chestplate,
						 struct item_data* leggings, struct item_data* boots);

#endif