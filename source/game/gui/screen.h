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

#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>

#include "../../cglm/cglm.h"

struct screen {
	void (*reset)(struct screen* s, int width, int height);
	void (*update)(struct screen* s, float dt);
	void (*render2D)(struct screen* s, int width, int height);
	void (*render3D)(struct screen* s, mat4 view);
	bool render_world;
};

extern struct screen screen_ingame;
extern struct screen screen_load_world;
extern struct screen screen_select_world;
extern struct screen screen_inventory;
extern struct screen screen_crafting;
extern struct screen screen_furnace;

void screen_set(struct screen* s);

void screen_crafting_set_windowc(uint8_t container);
void screen_furnace_set_windowc(uint8_t container);

#endif
