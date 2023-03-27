/*
	Copyright (c) 2023 ByteBit/xtreme8000

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

#ifndef THREAD_H
#define THREAD_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef PLATFORM_PC
#include <pthread.h>

struct thread {
	pthread_t native;
};

struct thread_channel {
	pthread_mutex_t lock;
	pthread_cond_t signal;
	void** data;
	size_t count;
	size_t length;
};

#endif

#ifdef PLATFORM_WII
#include <gccore.h>

struct thread {
	lwp_t native;
};

struct thread_channel {
	mqbox_t native;
};
#endif

void thread_create(struct thread* t, void* (*entry)(void* arg), void* arg,
				   uint8_t priority);
void thread_join(struct thread* t);
void thread_msleep(size_t ms);

void tchannel_init(struct thread_channel* c, size_t count);
void tchannel_close(struct thread_channel* c);
bool tchannel_receive(struct thread_channel* c, void** msg, bool block);
bool tchannel_send(struct thread_channel* c, void* msg, bool block);

#endif
