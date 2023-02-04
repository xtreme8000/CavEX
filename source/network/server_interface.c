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
#include <gccore.h>

#include "server_interface.h"

#define RPC_INBOX_SIZE 16
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
