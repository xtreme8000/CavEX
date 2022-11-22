#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include "../world.h"

void clin_chunk(w_coord_t x, w_coord_t y, w_coord_t z, c_coord_t sx,
				c_coord_t sy, c_coord_t sz, void* data);

void clin_unload_chunk(w_coord_t x, w_coord_t z);

#endif
