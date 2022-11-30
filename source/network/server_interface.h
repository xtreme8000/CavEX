#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

#include <stdbool.h>

enum server_rpc_type {
	SRPC_PLAYER_POS,
};

struct server_rpc {
	enum server_rpc_type type;
	union {
		struct {
			double x, y, z;
		} player_pos;
	} payload;
};

void svin_init(void);
void svin_process_messages(void (*process)(struct server_rpc*, void*),
						   void* user, bool block);
void svin_rpc_send(struct server_rpc* call);

#endif
