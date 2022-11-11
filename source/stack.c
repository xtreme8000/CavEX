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
