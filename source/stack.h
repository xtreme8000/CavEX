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
