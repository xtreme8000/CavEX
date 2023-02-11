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

#ifndef HASH_H
#define HASH_H

#include <stddef.h>
#include <stdint.h>

typedef uint64_t ptime_t;

void time_reset(void);
ptime_t time_get(void);
int32_t time_diff_ms(ptime_t f, ptime_t s);
float time_diff_s(ptime_t f, ptime_t s);

uint32_t hash_u32(uint32_t x);

void hsv2rgb(float* h, float* s, float* v);

uint8_t nibble_read(uint8_t* base, size_t idx);
void nibble_write(uint8_t* base, size_t idx, uint8_t data);

#endif
