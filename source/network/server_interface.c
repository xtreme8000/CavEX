#include <assert.h>
#include <gccore.h>

#include "server_interface.h"

#define RPC_INBOX_SIZE 8
static struct server_rpc rpc_msg[RPC_INBOX_SIZE];
mqbox_t svin_inbox;
mqbox_t svin_empty_msg;

void svin_init() {
	MQ_Init(&svin_inbox, RPC_INBOX_SIZE);
	MQ_Init(&svin_empty_msg, RPC_INBOX_SIZE);

	for(int k = 0; k < RPC_INBOX_SIZE; k++)
		MQ_Send(svin_empty_msg, rpc_msg + k, MQ_MSG_BLOCK);
}

void svin_process_messages(void (*process)(struct server_rpc*, void*),
						   void* user, bool block) {
	assert(process);

	mqmsg_t call;
	while(
		MQ_Receive(svin_inbox, &call, block ? MQ_MSG_BLOCK : MQ_MSG_NOBLOCK)) {
		process(call, user);
		MQ_Send(svin_empty_msg, call, MQ_MSG_BLOCK);
	}
}

void svin_rpc_send(struct server_rpc* call) {
	assert(call);

	struct server_rpc* empty;
	MQ_Receive(svin_empty_msg, (mqmsg_t*)&empty, MQ_MSG_BLOCK);
	*empty = *call;
	MQ_Send(svin_inbox, empty, MQ_MSG_BLOCK);
}
