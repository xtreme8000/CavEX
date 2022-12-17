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
#include <math.h>
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

void hsv2rgb(float* h, float* s, float* v) {
	assert(h && s && v);

	int i = floor((*h) * 6.0F);
	float f = (*h) * 6.0F - i;
	float p = (*v) * (1.0F - *s);
	float q = (*v) * (1.0F - f * (*s));
	float t = (*v) * (1.0F - (1.0F - f) * (*s));

	float r, g, b;
	switch(i % 6) {
		case 0: r = *v, g = t, b = p; break;
		case 1: r = q, g = *v, b = p; break;
		case 2: r = p, g = *v, b = t; break;
		case 3: r = p, g = q, b = *v; break;
		case 4: r = t, g = p, b = *v; break;
		case 5: r = *v, g = p, b = q; break;
	}

	*h = r;
	*s = g;
	*v = b;
}
