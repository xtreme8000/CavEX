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

#ifndef SERVER_LOCAL
#define SERVER_LOCAL

#include <m-lib/m-string.h>
#include <stdbool.h>
#include <stddef.h>

#include "../item/inventory.h"
#include "../world.h"
#include "level_archive.h"
#include "server_world.h"

#define MAX_REGIONS 4
#define MAX_VIEW_DISTANCE 5 // in chunks
#define MAX_CHUNKS ((MAX_VIEW_DISTANCE * 2 + 2) * (MAX_VIEW_DISTANCE * 2 + 2))

struct server_local {
	struct {
		double x, y, z;
		float rx, ry;
		enum world_dim dimension;
		bool has_pos;
		bool finished_loading;
		struct inventory inventory;
	} player;
	struct server_world world;
	uint64_t world_time;
	ptime_t world_time_start;
	string_t level_name;
	struct level_archive level;
};

void server_local_create(struct server_local* s);

#endif
