#include "util.h"

uint32_t hash_u32(uint32_t x) {
	x = ((x >> 16) ^ x) * 0x45D9F3B;
	x = ((x >> 16) ^ x) * 0x45D9F3B;
	x = (x >> 16) ^ x;
	return x;
}
