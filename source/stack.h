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

#ifndef STACK_H
#define STACK_H

#include <stdbool.h>
#include <stddef.h>

struct stack {
	size_t length;
	size_t index;
	size_t element_size;
	void* data;
};

void stack_create(struct stack* stk, size_t inital_size, size_t element_size);

void stack_push(struct stack* stk, void* obj);

bool stack_empty(struct stack* stk);

size_t stack_size(struct stack* stk);

void stack_at(struct stack* stk, void* obj, size_t index);

bool stack_pop(struct stack* stk, void* obj);

void stack_clear(struct stack* stk);

void stack_destroy(struct stack* stk);

#endif
