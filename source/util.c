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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "util.h"

void rand_gen_seed(struct random_gen* g) {
	assert(g);
	g->seed = time(NULL);
}

uint32_t rand_gen(struct random_gen* g) {
	g->seed ^= g->seed << 13;
	g->seed ^= g->seed >> 17;
	g->seed ^= g->seed << 5;
	return g->seed;
}

int rand_gen_range(struct random_gen* g, int min, int max) {
	assert(g);
	return rand_gen(g) % (max - min) + min;
}

float rand_gen_flt(struct random_gen* g) {
	assert(g);
	return (float)rand_gen(g) / UINT32_MAX;
}

void* file_read(const char* name) {
	FILE* f = fopen(name, "rb");

	if(!f)
		return NULL;

	fseek(f, 0, SEEK_END);
	size_t length = ftell(f);
	fseek(f, 0, SEEK_SET);
	char* res = malloc(length + 1);
	res[length] = 0;
	(void)!fread(res, length, 1, f);
	fclose(f);

	return res;
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

inline uint8_t nibble_read(uint8_t* base, size_t idx) {
	return (base[idx / 2] >> ((idx % 2) * 4)) & 0xF;
}

inline void nibble_write(uint8_t* base, size_t idx, uint8_t data) {
	base[idx / 2] = (base[idx / 2] & ~(0x0F << ((idx % 2) * 4)))
		| (data << ((idx % 2) * 4));
}
