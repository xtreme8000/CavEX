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

#include "../platform/thread.h"
#include "server_interface.h"

#define RPC_INBOX_SIZE 16
static struct server_rpc rpc_msg[RPC_INBOX_SIZE];
static struct thread_channel svin_inbox;
static struct thread_channel svin_empty_msg;

void svin_init() {
	tchannel_init(&svin_inbox, RPC_INBOX_SIZE);
	tchannel_init(&svin_empty_msg, RPC_INBOX_SIZE);

	for(int k = 0; k < RPC_INBOX_SIZE; k++)
		tchannel_send(&svin_empty_msg, rpc_msg + k, true);
}

void svin_process_messages(void (*process)(struct server_rpc*, void*),
						   void* user, bool block) {
	assert(process);

	void* call;
	while(tchannel_receive(&svin_inbox, &call, block)) {
		process(call, user);
		tchannel_send(&svin_empty_msg, call, true);
	}
}

void svin_rpc_send(struct server_rpc* call) {
	assert(call);

	struct server_rpc* empty;
	tchannel_receive(&svin_empty_msg, (void**)&empty, true);
	*empty = *call;
	tchannel_send(&svin_inbox, empty, true);
}
