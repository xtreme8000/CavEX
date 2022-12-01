#ifndef REGION_ARCHIVE_H
#define REGION_ARCHIVE_H

#include <stdbool.h>
#include <stdint.h>

#include "../world.h"

struct region_archive {
	w_coord_t x, z;
	uint32_t* offsets;
	char file_name[64];
};

#define REGION_SIZE 32
#define REGION_SIZE_BITS 5
#define REGION_SECTOR_SIZE 4096

#define CHUNK_REGION_COORD(x) ((w_coord_t)floor(x / (float)REGION_SIZE))

bool region_archive_create(struct region_archive* ra, char* world_name,
						   w_coord_t x, w_coord_t z);
void region_archive_destroy(struct region_archive* ra);
bool region_archive_contains(struct region_archive* ra, w_coord_t x,
							 w_coord_t z);
bool region_archive_get_blocks(struct region_archive* ra, w_coord_t x,
							   w_coord_t z, uint8_t** ids, uint8_t** metadata,
							   uint8_t** lighting);

#endif
