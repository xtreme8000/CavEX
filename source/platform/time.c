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

#include "time.h"

#ifdef PLATFORM_PC

#include <time.h>

void time_reset() { }

ptime_t time_get() {
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC_RAW, &t);
	return t;
}

int32_t time_diff_ms(ptime_t f, ptime_t s) {
	return (s.tv_sec - f.tv_sec) * 1000 + (s.tv_nsec - f.tv_nsec) / 1000000;
}

float time_diff_s(ptime_t f, ptime_t s) {
	return (float)(s.tv_sec - f.tv_sec)
		+ (float)(s.tv_nsec - f.tv_nsec) / 1000000000.0F;
}

#endif

#ifdef PLATFORM_WII

#include <ogc/lwp_watchdog.h>

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

#endif
