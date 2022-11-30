#ifndef CHUNK_MESHER_H
#define CHUNK_MESHER_H

#include <stdbool.h>

#define CHUNK_MESHER_QLENGTH 8

struct chunk;

void chunk_mesher_init(void);
void chunk_mesher_receive(void);
bool chunk_mesher_send(struct chunk* c);

#endif
