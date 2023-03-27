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
#include <m-lib/m-string.h>

#include "../cNBT/nbt.h"

#include "region_archive.h"
#include "server_world.h"

#define CHUNK_EXISTS(offset, sectors) ((offset) >= 2 && (sectors) >= 1)

static uint32_t conv_u32_native(uint8_t* data) {
	return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
}

static void conv_native_u32(uint32_t in, uint8_t* data) {
	data[0] = in >> 24;
	data[1] = (in >> 16) & 0xFF;
	data[2] = (in >> 8) & 0xFF;
	data[3] = in & 0xFF;
}

static size_t fread_u32(uint32_t* out, FILE* f) {
	size_t res = fread(out, sizeof(uint32_t), 1, f);
	*out = conv_u32_native((uint8_t*)out);
	return res;
}

static size_t fwrite_u32(uint32_t in, FILE* f) {
	uint8_t tmp[sizeof(uint32_t)];
	conv_native_u32(in, tmp);

	return fwrite(tmp, sizeof(uint32_t), 1, f);
}

static int sort_region_chunks(const void* a, const void* b) {
	uint32_t offset_a = (*(const uint32_t*)a) >> 8;
	uint32_t offset_b = (*(const uint32_t*)b) >> 8;
	return offset_a - offset_b;
}

static bool rebuild_occupied_list(struct region_archive* ra) {
	assert(ra);

	ra->occupied_index = 0;
	ra->occupied_sorted[ra->occupied_index++] = (0 << 8) | 2;

	for(size_t k = 0; k < REGION_SIZE * REGION_SIZE; k++) {
		uint32_t offset = ra->offsets[k] >> 8;
		uint32_t sectors = ra->offsets[k] & 0xFF;

		if(CHUNK_EXISTS(offset, sectors))
			ra->occupied_sorted[ra->occupied_index++] = ra->offsets[k];
	}

	qsort(ra->occupied_sorted, ra->occupied_index, sizeof(uint32_t),
		  sort_region_chunks);

	uint32_t prev = 1;
	for(size_t k = 1; k < ra->occupied_index; k++) {
		uint32_t offset = ra->occupied_sorted[k] >> 8;

		if(offset <= prev)
			return false;

		prev = offset;
	}

	return true;
}

bool region_archive_create_new(struct region_archive* ra, string_t world_name,
							   w_coord_t x, w_coord_t z,
							   enum world_dim dimension) {
	assert(ra && world_name);

	if(dimension == WORLD_DIM_OVERWORLD) {
		string_init_printf(ra->file_name, "saves/%s/region/r.%i.%i.mcr",
						   string_get_cstr(world_name), x, z);
	} else {
		string_init_printf(ra->file_name, "saves/%s/DIM-1/region/r.%i.%i.mcr",
						   string_get_cstr(world_name), x, z);
	}

	FILE* f = fopen(string_get_cstr(ra->file_name), "w");

	if(!f) {
		string_clear(ra->file_name);
		return false;
	}

	for(size_t k = 0; k < REGION_SIZE * REGION_SIZE * 2; k++)
		fwrite((uint32_t[]) {0}, sizeof(uint32_t), 1, f);

	fclose(f);
	string_clear(ra->file_name);

	return region_archive_create(ra, world_name, x, z, dimension);
}

bool region_archive_create(struct region_archive* ra, string_t world_name,
						   w_coord_t x, w_coord_t z, enum world_dim dimension) {
	assert(ra && world_name);

	ra->offsets = malloc(sizeof(uint32_t) * REGION_SIZE * REGION_SIZE);

	if(!ra->offsets)
		return false;

	ra->occupied_sorted
		= malloc(sizeof(uint32_t) * (REGION_SIZE * REGION_SIZE + 1));

	if(!ra->occupied_sorted) {
		free(ra->offsets);
		return false;
	}

	if(dimension == WORLD_DIM_OVERWORLD) {
		string_init_printf(ra->file_name, "saves/%s/region/r.%i.%i.mcr",
						   string_get_cstr(world_name), x, z);
	} else {
		string_init_printf(ra->file_name, "saves/%s/DIM-1/region/r.%i.%i.mcr",
						   string_get_cstr(world_name), x, z);
	}

	ra->x = x;
	ra->z = z;

	FILE* f = fopen(string_get_cstr(ra->file_name), "rb");

	if(!f) {
		free(ra->offsets);
		string_clear(ra->file_name);
		return false;
	}

	if(!fread(ra->offsets, sizeof(uint32_t) * REGION_SIZE * REGION_SIZE, 1,
			  f)) {
		free(ra->offsets);
		fclose(f);
		string_clear(ra->file_name);
		return false;
	}

	// convert from big endian to native endian
	for(size_t k = 0; k < REGION_SIZE * REGION_SIZE; k++)
		ra->offsets[k] = conv_u32_native((uint8_t*)(ra->offsets + k));

	fclose(f);

	ilist_regions_init_field(ra);

	if(!rebuild_occupied_list(ra)) {
		free(ra->offsets);
		free(ra->occupied_sorted);
		string_clear(ra->file_name);
		return false;
	}

	return true;
}

void region_archive_destroy(struct region_archive* ra) {
	assert(ra && ra->offsets && ra->occupied_sorted);

	free(ra->offsets);
	free(ra->occupied_sorted);
	string_clear(ra->file_name);
}

bool region_archive_contains(struct region_archive* ra, w_coord_t x,
							 w_coord_t z, bool* chunk_exists) {
	assert(ra && chunk_exists);

	if(CHUNK_REGION_COORD(x) != ra->x || CHUNK_REGION_COORD(z) != ra->z) {
		*chunk_exists = false;
		return false;
	}

	int rx = x & (REGION_SIZE - 1);
	int rz = z & (REGION_SIZE - 1);

	uint32_t offset = ra->offsets[rx + rz * REGION_SIZE] >> 8;
	uint32_t sectors = ra->offsets[rx + rz * REGION_SIZE] & 0xFF;

	*chunk_exists = CHUNK_EXISTS(offset, sectors);
	return true;
}

bool region_archive_get_blocks(struct region_archive* ra, w_coord_t x,
							   w_coord_t z, struct server_chunk* sc) {
	assert(ra && sc);
	bool chunk_exists;
	assert(region_archive_contains(ra, x, z, &chunk_exists) && chunk_exists);

	int rx = x & (REGION_SIZE - 1);
	int rz = z & (REGION_SIZE - 1);

	uint32_t offset = ra->offsets[rx + rz * REGION_SIZE] >> 8;
	uint32_t sectors = ra->offsets[rx + rz * REGION_SIZE] & 0xFF;

	// TODO: little endian

	FILE* f = fopen(string_get_cstr(ra->file_name), "rb");

	if(!f)
		return false;

	if(fseek(f, offset * REGION_SECTOR_SIZE, SEEK_SET) != 0) {
		fclose(f);
		return false;
	}

	uint32_t length;
	if(!fread_u32(&length, f)
	   || length + sizeof(uint32_t) > sectors * REGION_SECTOR_SIZE) {
		fclose(f);
		return false;
	}

	uint8_t type;
	if(!fread(&type, sizeof(uint8_t), 1, f) || type > 3) {
		fclose(f);
		return false;
	}

	void* nbt_compressed = malloc(length - 1);

	if(!nbt_compressed) {
		fclose(f);
		return false;
	}

	if(!fread(nbt_compressed, length - 1, 1, f)) {
		free(nbt_compressed);
		fclose(f);
		return false;
	}

	nbt_node* chunk = nbt_parse_compressed(nbt_compressed, length - 1);

	free(nbt_compressed);
	fclose(f);

	if(!chunk)
		return false;

	nbt_node* n_x = nbt_find_by_path(chunk, ".Level.xPos");
	nbt_node* n_z = nbt_find_by_path(chunk, ".Level.zPos");

	if(!n_x || !n_z || n_x->type != TAG_INT || n_z->type != TAG_INT
	   || n_x->payload.tag_int != x || n_z->payload.tag_int != z) {
		nbt_free(chunk);
		return false;
	}

	nbt_node* n_blocks = nbt_find_by_path(chunk, ".Level.Blocks");
	nbt_node* n_metadata = nbt_find_by_path(chunk, ".Level.Data");
	nbt_node* n_skyl = nbt_find_by_path(chunk, ".Level.SkyLight");
	nbt_node* n_torchl = nbt_find_by_path(chunk, ".Level.BlockLight");
	nbt_node* n_height = nbt_find_by_path(chunk, ".Level.HeightMap");

	if(!n_blocks || !n_metadata || !n_skyl || !n_torchl || !n_height
	   || n_blocks->type != TAG_BYTE_ARRAY || n_metadata->type != TAG_BYTE_ARRAY
	   || n_skyl->type != TAG_BYTE_ARRAY || n_torchl->type != TAG_BYTE_ARRAY
	   || n_height->type != TAG_BYTE_ARRAY
	   || n_blocks->payload.tag_byte_array.length
		   != CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT
	   || n_metadata->payload.tag_byte_array.length
		   != CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT / 2
	   || n_skyl->payload.tag_byte_array.length
		   != CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT / 2
	   || n_torchl->payload.tag_byte_array.length
		   != CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT / 2
	   || n_height->payload.tag_byte_array.length != CHUNK_SIZE * CHUNK_SIZE) {
		nbt_free(chunk);
		return false;
	}

	// borrow memory regions from nbt tree

	n_blocks->type = TAG_INVALID;
	n_metadata->type = TAG_INVALID;
	n_skyl->type = TAG_INVALID;
	n_torchl->type = TAG_INVALID;
	n_height->type = TAG_INVALID;

	sc->ids = n_blocks->payload.tag_byte_array.data;
	sc->metadata = n_metadata->payload.tag_byte_array.data;
	sc->lighting_sky = n_skyl->payload.tag_byte_array.data;
	sc->lighting_torch = n_torchl->payload.tag_byte_array.data;
	sc->heightmap = n_height->payload.tag_byte_array.data;

	nbt_free(chunk);

	return true;
}

static bool file_overwrite_index(FILE* f, size_t index, uint32_t data) {
	assert(f);

	if(fseek(f, index * sizeof(uint32_t), SEEK_SET)) {
		fclose(f);
		return false;
	}

	if(fwrite_u32(data, f) != 1) {
		fclose(f);
		return false;
	}

	return true;
}

static bool file_overwrite_chunk(FILE* f, size_t offset, void* data,
								 size_t length, bool pad) {
	assert(f && data && length > 0);

	if(fseek(f, offset, SEEK_SET)) {
		fclose(f);
		return false;
	}

	if(fwrite_u32(length + 1, f) != 1) {
		fclose(f);
		return false;
	}

	if(fwrite((uint8_t[]) {2}, sizeof(uint8_t), 1, f) != 1) {
		fclose(f);
		return false;
	}

	if(fwrite(data, length, 1, f) != 1) {
		fclose(f);
		return false;
	}

	size_t sectors = (length + REGION_SECTOR_SIZE - 1) / REGION_SECTOR_SIZE
		* REGION_SECTOR_SIZE;

	if(pad && sectors > length + 5) {
		for(size_t k = 0; k < sectors - length - 5; k++) {
			if(fwrite((uint8_t[]) {0}, sizeof(uint8_t), 1, f) != 1) {
				fclose(f);
				return false;
			}
		}
	}

	return true;
}

bool region_archive_set_blocks(struct region_archive* ra, w_coord_t x,
							   w_coord_t z, struct server_chunk* sc) {
	assert(ra && sc);
	assert(CHUNK_REGION_COORD(x) == ra->x && CHUNK_REGION_COORD(z) == ra->z);

	FILE* f = fopen(string_get_cstr(ra->file_name), "rb+");

	// early exit
	if(!f)
		return false;

	struct nbt_list root_list_sentinel;
	struct nbt_list level_list_sentinel;
	struct nbt_list empty_list_sentinel = (struct nbt_list) {
		.data = &(nbt_node) {.type = TAG_COMPOUND},
	};

	INIT_LIST_HEAD(&root_list_sentinel.entry);
	INIT_LIST_HEAD(&level_list_sentinel.entry);
	INIT_LIST_HEAD(&empty_list_sentinel.entry);

	nbt_node root = (nbt_node) {
		.type = TAG_COMPOUND,
		.name = "",
		.payload.tag_compound = &root_list_sentinel,
	};

	nbt_node level = (nbt_node) {
		.type = TAG_COMPOUND,
		.name = "Level",
		.payload.tag_compound = &level_list_sentinel,
	};

	struct nbt_list root_list = (struct nbt_list) {
		.data = &level,
	};

	list_add_head(&root_list.entry, &root_list_sentinel.entry);

	nbt_node level_list_nodes[] = {
		{
			.type = TAG_BYTE_ARRAY,
			.name = "Blocks",
			.payload.tag_byte_array.data = sc->ids,
			.payload.tag_byte_array.length
			= CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT,
		},
		{
			.type = TAG_BYTE_ARRAY,
			.name = "Data",
			.payload.tag_byte_array.data = sc->metadata,
			.payload.tag_byte_array.length
			= CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT / 2,
		},
		{
			.type = TAG_BYTE_ARRAY,
			.name = "SkyLight",
			.payload.tag_byte_array.data = sc->lighting_sky,
			.payload.tag_byte_array.length
			= CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT / 2,
		},
		{
			.type = TAG_BYTE_ARRAY,
			.name = "BlockLight",
			.payload.tag_byte_array.data = sc->lighting_torch,
			.payload.tag_byte_array.length
			= CHUNK_SIZE * CHUNK_SIZE * WORLD_HEIGHT / 2,
		},
		{
			.type = TAG_BYTE_ARRAY,
			.name = "HeightMap",
			.payload.tag_byte_array.data = sc->heightmap,
			.payload.tag_byte_array.length = CHUNK_SIZE * CHUNK_SIZE,
		},
		{
			.type = TAG_LIST,
			.name = "Entities",
			.payload.tag_list = &empty_list_sentinel,
		},
		{
			.type = TAG_LIST,
			.name = "TileEntities",
			.payload.tag_list = &empty_list_sentinel,
		},
		{.type = TAG_LONG, .name = "LastUpdate", .payload.tag_long = 0},
		{.type = TAG_INT, .name = "xPos", .payload.tag_int = x},
		{.type = TAG_INT, .name = "zPos", .payload.tag_int = z},
		{.type = TAG_BYTE, .name = "TerrainPopulated", .payload.tag_byte = 1},
	};

	struct nbt_list
		level_list[sizeof(level_list_nodes) / sizeof(*level_list_nodes)];

	for(size_t k = 0; k < sizeof(level_list) / sizeof(*level_list); k++) {
		level_list[k].data = level_list_nodes + k;
		list_add_tail(&level_list[k].entry, &level_list_sentinel.entry);
	}

	struct buffer res = nbt_dump_compressed(&root, STRAT_INFLATE);

	if(!res.data)
		return false;

	uint32_t new_data_sectors = (res.len + sizeof(uint32_t) + sizeof(uint8_t)
								 + REGION_SECTOR_SIZE - 1)
		/ REGION_SECTOR_SIZE;

	int rx = x & (REGION_SIZE - 1);
	int rz = z & (REGION_SIZE - 1);

	uint32_t offset = ra->offsets[rx + rz * REGION_SIZE] >> 8;
	uint32_t sectors = ra->offsets[rx + rz * REGION_SIZE] & 0xFF;

	bool success = true;

	if(CHUNK_EXISTS(offset, sectors) && new_data_sectors <= sectors) {
		// chunk already exists in file and existing space fits new data
		uint32_t data = (offset << 8) | new_data_sectors;
		ra->offsets[rx + rz * REGION_SIZE] = data;

		if(success && sectors != new_data_sectors
		   && !file_overwrite_index(f, rx + rz * REGION_SIZE, data))
			success = false;

		if(success
		   && !file_overwrite_chunk(f, offset * REGION_SECTOR_SIZE, res.data,
									res.len, false))
			success = false;

	} else {
		/* append new data at end or insert it in between existing chunks where
		 * there is enough space left */
		uint32_t new_offset = 0;
		bool pad = false;

		for(size_t k = 0; k < ra->occupied_index; k++) {
			uint32_t off1 = ra->occupied_sorted[k] >> 8;
			uint32_t sec1 = ra->occupied_sorted[k] & 0xFF;

			// append at end
			if(k + 1 >= ra->occupied_index) {
				new_offset = off1 + sec1;
				pad = true; // mc requires files to be multiples of 4KiB
				break;
			}

			uint32_t off2 = ra->occupied_sorted[k + 1] >> 8;

			// insert in between?
			if(off2 - (off1 + sec1) >= new_data_sectors) {
				new_offset = off1 + sec1;
				pad = false;
				break;
			}
		}

		// sanity check, don't overwrite lookup tables at start of file
		if(new_offset > 0) {
			uint32_t data = (new_offset << 8) | new_data_sectors;
			ra->offsets[rx + rz * REGION_SIZE] = data;

			if(success && !file_overwrite_index(f, rx + rz * REGION_SIZE, data))
				success = false;

			if(success
			   && !file_overwrite_chunk(f, new_offset * REGION_SECTOR_SIZE,
										res.data, res.len, pad))
				success = false;
		} else {
			success = false;
		}
	}

	// this could be done without resorting the entire list
	if(success)
		success = rebuild_occupied_list(ra);

	fclose(f);
	buffer_free(&res);
	return success;
}
