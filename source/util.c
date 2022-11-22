#include <ogc/lwp_watchdog.h>

#include "util.h"

void time_reset() {
	settime(0);
}

ptime_t time_get() {
	return gettime();
}

int32_t time_diff_ms(ptime_t f, ptime_t s) {
	return (s - f) / TB_TIMER_CLOCK;
}

float time_diff_s(ptime_t f, ptime_t s) {
	return (float)(s - f) / (float)(1000UL * TB_TIMER_CLOCK);
}

uint32_t hash_u32(uint32_t x) {
	x = ((x >> 16) ^ x) * 0x45D9F3B;
	x = ((x >> 16) ^ x) * 0x45D9F3B;
	x = (x >> 16) ^ x;
	return x;
}
