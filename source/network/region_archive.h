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

#ifndef REGION_ARCHIVE_H
#define REGION_ARCHIVE_H

#include <m-lib/m-i-list.h>
#include <m-lib/m-string.h>
#include <stdbool.h>
#include <stdint.h>

#include "../world.h"

struct server_chunk;

struct region_archive {
	w_coord_t x, z;
	uint32_t* offsets;
	uint32_t* occupied_sorted;
	size_t occupied_index;
	string_t file_name;
	ILIST_INTERFACE(ilist_regions, struct region_archive);
};

ILIST_DEF(ilist_regions, struct region_archive, M_POD_OPLIST)

#define REGION_SIZE 32
#define REGION_SIZE_BITS 5
#define REGION_SECTOR_SIZE 4096

#define CHUNK_REGION_COORD(x) ((w_coord_t)floor(x / (float)REGION_SIZE))

bool region_archive_create_new(struct region_archive* ra, string_t world_name,
							   w_coord_t x, w_coord_t z,
							   enum world_dim dimension);
bool region_archive_create(struct region_archive* ra, string_t world_name,
						   w_coord_t x, w_coord_t z, enum world_dim dimension);
void region_archive_destroy(struct region_archive* ra);
bool region_archive_contains(struct region_archive* ra, w_coord_t x,
							 w_coord_t z, bool* chunk_exists);
bool region_archive_get_blocks(struct region_archive* ra, w_coord_t x,
							   w_coord_t z, struct server_chunk* sc);
bool region_archive_set_blocks(struct region_archive* ra, w_coord_t x,
							   w_coord_t z, struct server_chunk* sc);

#endif
