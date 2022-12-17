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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stack.h"

void stack_create(struct stack* stk, size_t inital_size, size_t element_size) {
	assert(stk != NULL && inital_size > 0 && element_size > 0);

	stk->element_size = element_size;
	stk->index = 0;
	stk->length = inital_size;
	stk->data = malloc(stk->length * stk->element_size);
	assert(stk->data);
}

void stack_push(struct stack* stk, void* obj) {
	assert(stk != NULL && obj != NULL);

	if(stk->index >= stk->length) {
		stk->length *= 2;
		stk->data = realloc(stk->data, stk->length * stk->element_size);
		assert(stk->data);
	}

	memcpy((uint8_t*)stk->data + (stk->index++) * stk->element_size, obj,
		   stk->element_size);
}

bool stack_empty(struct stack* stk) {
	assert(stk != NULL);

	return stk->index == 0;
}

size_t stack_size(struct stack* stk) {
	assert(stk != NULL);

	return stk->index;
}

void stack_at(struct stack* stk, void* obj, size_t index) {
	assert(stk != NULL && obj != NULL);

	if(index < stk->index)
		memcpy(obj, (uint8_t*)stk->data + index * stk->element_size,
			   stk->element_size);
}

bool stack_pop(struct stack* stk, void* obj) {
	assert(stk != NULL && obj != NULL);

	if(stack_empty(stk))
		return false;

	memcpy(obj, (uint8_t*)stk->data + (--stk->index) * stk->element_size,
		   stk->element_size);

	/*if(stk->index * 4 <= stk->length) {
		stk->length /= 2;
		stk->data = realloc(stk->data, stk->length * stk->element_size);
		assert(stk->data);
	}*/

	return true;
}

void stack_clear(struct stack* stk) {
	assert(stk != NULL);

	stk->index = 0;
}

void stack_destroy(struct stack* stk) {
	assert(stk != NULL);

	if(stk->data)
		free(stk->data);
}
