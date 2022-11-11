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
