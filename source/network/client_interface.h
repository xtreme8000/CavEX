#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include "../world.h"

enum client_rpc_type {
	CRPC_CHUNK,
	CRPC_UNLOAD_CHUNK,
};

struct client_rpc {
	enum client_rpc_type type;
	union {
		struct {
			w_coord_t x, y, z;
			w_coord_t sx, sy, sz;
			uint8_t* ids;
			uint8_t* metadata;
			uint8_t* lighting;
		} chunk;
		struct {
			w_coord_t x, z;
		} unload_chunk;
	} payload;
};

void clin_init(void);
void clin_update(void);
void clin_rpc_send(struct client_rpc* call);

#endif
