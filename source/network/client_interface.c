#include <gccore.h>

#include "../game/game_state.h"
#include "client_interface.h"
#include "server_interface.h"

#define RPC_INBOX_SIZE 8
static struct client_rpc rpc_msg[RPC_INBOX_SIZE];
mqbox_t clin_inbox;
mqbox_t clin_empty_msg;

void clin_chunk(w_coord_t x, w_coord_t y, w_coord_t z, w_coord_t sx,
				w_coord_t sy, w_coord_t sz, uint8_t* ids, uint8_t* metadata,
				uint8_t* lighting) {
	assert(sx > 0 && sz > 0 && y >= 0 && y + sy <= WORLD_HEIGHT);
	assert(ids && metadata && lighting);

	// TODO: metadata

	uint8_t* ids_t = ids;
	uint8_t* metadata_t = metadata;
	uint8_t* lighting_t = lighting;
	bool flip = true;

	for(w_coord_t ox = x; ox < x + sx; ox++) {
		for(w_coord_t oz = z; oz < z + sz; oz++) {
			for(w_coord_t oy = y; oy < y + sy; oy++) {
				uint8_t md = flip ? (*metadata_t) & 0xF : (*metadata_t) >> 4;
				world_set_block(&gstate.world, ox, oy, oz,
								(struct block_data) {
									.type = *ids_t,
									.metadata = md,
									.sky_light = (*lighting_t) & 0xF,
									.torch_light = (*lighting_t) >> 4,
								});
				ids_t++;
				lighting_t++;

				flip = !flip;
				if(flip)
					metadata_t++;
			}
		}
	}

	free(ids);
	free(metadata);
	free(lighting);
}

void clin_unload_chunk(w_coord_t x, w_coord_t z) {
	for(w_coord_t k = 0; k < WORLD_HEIGHT; k += CHUNK_SIZE) {
		struct chunk* c = world_find_chunk(&gstate.world, x * CHUNK_SIZE, k,
										   z * CHUNK_SIZE);

		if(c)
			world_unload_chunk(&gstate.world, c);
	}
}

void clin_process(struct client_rpc* call) {
	assert(call);

	switch(call->type) {
		case CRPC_CHUNK:
			clin_chunk(call->payload.chunk.x, call->payload.chunk.y,
					   call->payload.chunk.z, call->payload.chunk.sx,
					   call->payload.chunk.sy, call->payload.chunk.sz,
					   call->payload.chunk.ids, call->payload.chunk.metadata,
					   call->payload.chunk.lighting);
			break;
		case CRPC_UNLOAD_CHUNK:
			clin_unload_chunk(call->payload.unload_chunk.x,
							  call->payload.unload_chunk.z);
			break;
	}
}

void clin_init() {
	MQ_Init(&clin_inbox, RPC_INBOX_SIZE);
	MQ_Init(&clin_empty_msg, RPC_INBOX_SIZE);

	for(int k = 0; k < RPC_INBOX_SIZE; k++)
		MQ_Send(clin_empty_msg, rpc_msg + k, MQ_MSG_BLOCK);
}

void clin_update() {
	mqmsg_t call;
	while(MQ_Receive(clin_inbox, &call, MQ_MSG_NOBLOCK)) {
		clin_process(call);
		MQ_Send(clin_empty_msg, call, MQ_MSG_BLOCK);
	}

	svin_rpc_send(&(struct server_rpc) {
		.type = SRPC_PLAYER_POS,
		.payload.player_pos.x = gstate.camera.x,
		.payload.player_pos.y = gstate.camera.y,
		.payload.player_pos.z = gstate.camera.z,
	});
}

void clin_rpc_send(struct client_rpc* call) {
	struct client_rpc* empty;
	MQ_Receive(clin_empty_msg, (mqmsg_t*)&empty, MQ_MSG_BLOCK);
	*empty = *call;
	MQ_Send(clin_inbox, empty, MQ_MSG_BLOCK);
}
