#ifndef HASH_H
#define HASH_H

#include <stdint.h>

typedef uint64_t ptime_t;

void time_reset(void);
ptime_t time_get(void);
int32_t time_diff_ms(ptime_t f, ptime_t s);
float time_diff_s(ptime_t f, ptime_t s);

uint32_t hash_u32(uint32_t x);

#endif
