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

#include "face_occlusion.h"

struct face_occlusion face_occlusion_s[] = {
	{
		.mask = {0, 0, 0, 0, 0, 0, 0, 0},
	},
	{
		.mask = {0, 0, 0, 0, 0, 0, 0, 0xFFFF},
	},
	{
		.mask = {0, 0, 0, 0, 0, 0, 0, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0, 0, 0, 0, 0, 0xFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0, 0, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0, 0, 0, 0, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0, 0, 0, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask
		= {0, 0, 0, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask
		= {0, 0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
				 0xFFFFFFFF},
	},
	{
		.mask = {0, 0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
				 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
				 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
				 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0xFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
				 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
	},
	{
		.mask = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
				 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF},
	},
};

struct face_occlusion* face_occlusion_full() {
	return face_occlusion_rect(16);
}

struct face_occlusion* face_occlusion_empty() {
	return face_occlusion_rect(0);
}

struct face_occlusion* face_occlusion_rect(int size) {
	return face_occlusion_s + size;
}

bool face_occlusion_test(struct face_occlusion* a, struct face_occlusion* b) {
	bool is_empty_a = true;

	if(a == face_occlusion_empty())
		return true;

	if(a == face_occlusion_full() && b == face_occlusion_full())
		return false;

	for(size_t k = 0; k < FACE_OCCLUSION_ARR_LENGTH; k++) {
		if(a->mask[k]) {
			is_empty_a = false;
			break;
		}
	}

	if(is_empty_a)
		return true;

	for(size_t k = 0; k < FACE_OCCLUSION_ARR_LENGTH; k++) {
		if(a->mask[k] != b->mask[k] && (a->mask[k] | b->mask[k]) == a->mask[k])
			return true;
	}

	return false;
}
