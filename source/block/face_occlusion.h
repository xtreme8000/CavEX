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

#ifndef FACE_OCCLUSION_H
#define FACE_OCCLUSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define FACE_OCCLUSION_SIZE (16 * 16)
#define FACE_OCCLUSION_ARR_LENGTH                                              \
	((FACE_OCCLUSION_SIZE + (sizeof(uint32_t) * 8) - 1)                        \
	 / (sizeof(uint32_t) * 8))

struct face_occlusion {
	uint32_t mask[FACE_OCCLUSION_ARR_LENGTH];
};

struct face_occlusion* face_occlusion_full();
struct face_occlusion* face_occlusion_empty();
struct face_occlusion* face_occlusion_rect(int size);

bool face_occlusion_test(struct face_occlusion* a, struct face_occlusion* b);

#endif
