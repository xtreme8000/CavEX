#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include "../item/items.h"
#include "../world.h"

#include "../cglm/cglm.h"

enum client_rpc_type {
	CRPC_CHUNK,
	CRPC_UNLOAD_CHUNK,
	CRPC_INVENTORY_SLOT,
	CRPC_PLAYER_POS,
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
		struct {
			uint8_t slot;
			struct item_data item;
		} inventory_slot;
		struct {
			vec3 position;
			vec2 rotation;
		} player_pos;
	} payload;
};

void clin_init(void);
void clin_update(void);
void clin_rpc_send(struct client_rpc* call);

#endif
