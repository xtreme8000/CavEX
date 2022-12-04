#include <gccore.h>
#include <stdint.h>

#include "../../block/blocks.h"
#include "../../chunk.h"
#include "../../util.h"
#include "gfx.h"
#include "render_block.h"

#define BLK_LEN 256

#define TEX_OFFSET(x) ((x)*18 + 3)

static inline uint8_t MIN_U8(uint8_t a, uint8_t b) {
	return a < b ? a : b;
}

static inline uint8_t MAX_U8(uint8_t a, uint8_t b) {
	return a > b ? a : b;
}

static uint8_t level_table_0[16] = {
	0, 0, 0, 0, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12,
};

static uint8_t level_table_1[16] = {
	0, 0, 0, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13,
};

static uint8_t level_table_2[16] = {
	0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
};

static inline uint8_t DIM_LIGHT(uint8_t l, uint8_t* table, bool shade_sides,
								uint8_t luminance) {
	return shade_sides ?
		(table[MAX_U8(l >> 4, luminance)] << 4) | table[l & 0x0F] :
		(MAX_U8(l >> 4, luminance) << 4) | (l & 0x0F);
}

static inline void render_block_side_adv_v2(
	struct displaylist* d, int16_t x, int16_t y, int16_t z, uint16_t width,
	uint16_t height, int16_t inset_bottom, int16_t inset_top, uint8_t tex_x,
	uint8_t tex_y, bool tex_flip_h, int tex_rotate, bool shade_sides,
	enum side side, const uint8_t* vertex_light, uint8_t luminance) {
	const uint8_t tex_coords[2][4][2] = {
		{
			{tex_x, tex_y},
			{tex_x + MIN_U8(16, width / 16), tex_y},
			{tex_x + MIN_U8(16, width / 16), tex_y + MIN_U8(16, height / 16)},
			{tex_x, tex_y + MIN_U8(16, height / 16)},
		},
		{
			{tex_x + MIN_U8(16, width / 16), tex_y},
			{tex_x, tex_y},
			{tex_x, tex_y + MIN_U8(16, height / 16)},
			{tex_x + MIN_U8(16, width / 16), tex_y + MIN_U8(16, height / 16)},
		},
	};

	switch(side) {
		case SIDE_LEFT: { // x minus
			displaylist_pos(d, x + inset_bottom, y, z);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[8], level_table_1,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 3) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 3) % 4][1]);
			displaylist_pos(d, x + inset_top, y + height, z);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[9], level_table_1,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 0) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 0) % 4][1]);
			displaylist_pos(d, x + inset_top, y + height, z + width);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[10], level_table_1,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 1) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 1) % 4][1]);
			displaylist_pos(d, x + inset_bottom, y, z + width);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[11], level_table_1,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 2) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 2) % 4][1]);
		} break;
		case SIDE_RIGHT: { // x positive
			displaylist_pos(d, x - inset_bottom, y, z);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[12], level_table_1,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 2) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 2) % 4][1]);
			displaylist_pos(d, x - inset_bottom, y, z + width);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[15], level_table_1,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 3) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 3) % 4][1]);
			displaylist_pos(d, x - inset_top, y + height, z + width);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[14], level_table_1,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 0) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 0) % 4][1]);
			displaylist_pos(d, x - inset_top, y + height, z);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[13], level_table_1,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 1) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 1) % 4][1]);
		} break;
		case SIDE_TOP: { // y positive
			displaylist_pos(d, x, y, z);
			displaylist_color(
				d, DIM_LIGHT(vertex_light[4], NULL, false, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 0) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 0) % 4][1]);
			displaylist_pos(d, x + width, y, z);
			displaylist_color(
				d, DIM_LIGHT(vertex_light[5], NULL, false, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 1) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 1) % 4][1]);
			displaylist_pos(d, x + width, y, z + height);
			displaylist_color(
				d, DIM_LIGHT(vertex_light[6], NULL, false, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 2) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 2) % 4][1]);
			displaylist_pos(d, x, y, z + height);
			displaylist_color(
				d, DIM_LIGHT(vertex_light[7], NULL, false, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 3) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 3) % 4][1]);
		} break;
		case SIDE_BOTTOM: { // y negative
			displaylist_pos(d, x, y + inset_bottom, z);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[0], level_table_0,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 3) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 3) % 4][1]);
			displaylist_pos(d, x, y + inset_top, z + height);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[3], level_table_0,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 0) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 0) % 4][1]);
			displaylist_pos(d, x + width, y + inset_top, z + height);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[2], level_table_0,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 1) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 1) % 4][1]);
			displaylist_pos(d, x + width, y + inset_bottom, z);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[1], level_table_0,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 2) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 2) % 4][1]);
		} break;
		case SIDE_FRONT: { // z minus
			displaylist_pos(d, x, y, z + inset_bottom);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[16], level_table_2,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 2) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 2) % 4][1]);
			displaylist_pos(d, x + width, y, z + inset_bottom);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[17], level_table_2,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 3) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 3) % 4][1]);
			displaylist_pos(d, x + width, y + height, z + inset_top);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[18], level_table_2,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 0) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 0) % 4][1]);
			displaylist_pos(d, x, y + height, z + inset_top);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[19], level_table_2,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 1) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 1) % 4][1]);
		} break;
		case SIDE_BACK: { // z positive
			displaylist_pos(d, x, y, z - inset_bottom);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[20], level_table_2,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 3) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 3) % 4][1]);
			displaylist_pos(d, x, y + height, z - inset_top);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[23], level_table_2,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 0) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 0) % 4][1]);
			displaylist_pos(d, x + width, y + height, z - inset_top);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[22], level_table_2,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 1) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 1) % 4][1]);
			displaylist_pos(d, x + width, y, z - inset_bottom);
			displaylist_color(d,
							  DIM_LIGHT(vertex_light[21], level_table_2,
										shade_sides, luminance));
			displaylist_texcoord(
				d, tex_coords[tex_flip_h][(tex_rotate + 2) % 4][0],
				tex_coords[tex_flip_h][(tex_rotate + 2) % 4][1]);
		} break;
		default: break;
	}
}

static inline void
render_block_side_adv(struct displaylist* d, int16_t x, int16_t y, int16_t z,
					  uint16_t width, uint16_t height, uint8_t tex_x,
					  uint8_t tex_y, bool tex_flip_h, int tex_rotate,
					  bool shade_sides, enum side side,
					  const uint8_t* vertex_light, uint8_t luminance) {
	render_block_side_adv_v2(d, x, y, z, width, height, 0, 0, tex_x, tex_y,
							 tex_flip_h, tex_rotate, shade_sides, side,
							 vertex_light, luminance);
}

static inline void render_block_side(struct displaylist* d, int16_t x,
									 int16_t y, int16_t z, int16_t yoffset,
									 uint16_t height, uint8_t tex,
									 uint8_t luminance, bool shade_sides,
									 uint16_t inset, bool tex_flip_h,
									 int tex_rotate, enum side side,
									 const uint8_t* vertex_light) {
	uint8_t tex_x = TEX_OFFSET(TEXTURE_X(tex));
	uint8_t tex_y = TEX_OFFSET(TEXTURE_Y(tex));

	switch(side) {
		case SIDE_LEFT: // x minus
			render_block_side_adv(
				d, x * BLK_LEN + inset, y * BLK_LEN + yoffset, z * BLK_LEN,
				BLK_LEN, height, tex_x, tex_y + (16 - height / 16), tex_flip_h,
				tex_rotate, shade_sides, SIDE_LEFT, vertex_light, luminance);
			break;
		case SIDE_RIGHT: // x positive
			render_block_side_adv(d, x * BLK_LEN + BLK_LEN - inset,
								  y * BLK_LEN + yoffset, z * BLK_LEN, BLK_LEN,
								  height, tex_x, tex_y + (16 - height / 16),
								  tex_flip_h, tex_rotate, shade_sides,
								  SIDE_RIGHT, vertex_light, luminance);
			break;
		case SIDE_BOTTOM: // y minus
			render_block_side_adv(d, x * BLK_LEN, y * BLK_LEN + yoffset + inset,
								  z * BLK_LEN, BLK_LEN, BLK_LEN, tex_x, tex_y,
								  tex_flip_h, tex_rotate, shade_sides,
								  SIDE_BOTTOM, vertex_light, luminance);
			break;
		case SIDE_TOP: // y positive
			render_block_side_adv(
				d, x * BLK_LEN, y * BLK_LEN + yoffset + height - inset,
				z * BLK_LEN, BLK_LEN, BLK_LEN, tex_x, tex_y, tex_flip_h,
				tex_rotate, shade_sides, SIDE_TOP, vertex_light, luminance);
			break;
		case SIDE_FRONT: // z minus
			render_block_side_adv(
				d, x * BLK_LEN, y * BLK_LEN + yoffset, z * BLK_LEN + inset,
				BLK_LEN, height, tex_x, tex_y + (16 - height / 16), tex_flip_h,
				tex_rotate, shade_sides, SIDE_FRONT, vertex_light, luminance);
			break;
		case SIDE_BACK: // z positive
			render_block_side_adv(d, x * BLK_LEN, y * BLK_LEN + yoffset,
								  z * BLK_LEN + BLK_LEN - inset, BLK_LEN,
								  height, tex_x, tex_y + (16 - height / 16),
								  tex_flip_h, tex_rotate, shade_sides,
								  SIDE_BACK, vertex_light, luminance);
			break;
		default: break;
	}
}

size_t render_block_cross(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only) {
	if(side != SIDE_TOP)
		return 0;

	if(!count_only) {
		int16_t x = W2C_COORD(this->x) * BLK_LEN;
		int16_t y = W2C_COORD(this->y) * BLK_LEN;
		int16_t z = W2C_COORD(this->z) * BLK_LEN;

		if(blocks[this->block->type]
			   ->render_block_data.cross_random_displacement) {
			uint32_t seed
				= hash_u32(this->x) ^ hash_u32(this->y) ^ hash_u32(this->z);
			x += (seed & 0xFFFF) % 129 - 64;
			z += (seed >> 16) % 129 - 64;
		}

		uint8_t tex
			= blocks[this->block->type]->getTextureIndex(this, SIDE_TOP);
		uint8_t tex_x = TEX_OFFSET(TEXTURE_X(tex));
		uint8_t tex_y = TEX_OFFSET(TEXTURE_Y(tex));
		uint8_t light = (MAX_U8(this->block->torch_light,
								blocks[this->block->type]->luminance)
						 << 4)
			| this->block->sky_light;

		displaylist_pos(d, x, y, z);
		displaylist_color(d, light);
		displaylist_texcoord(d, tex_x, tex_y + 16);
		displaylist_pos(d, x, y + BLK_LEN, z);
		displaylist_color(d, light);
		displaylist_texcoord(d, tex_x, tex_y);
		displaylist_pos(d, x + BLK_LEN, y + BLK_LEN, z + BLK_LEN);
		displaylist_color(d, light);
		displaylist_texcoord(d, tex_x + 16, tex_y);
		displaylist_pos(d, x + BLK_LEN, y, z + BLK_LEN);
		displaylist_color(d, light);
		displaylist_texcoord(d, tex_x + 16, tex_y + 16);

		displaylist_pos(d, x + BLK_LEN, y, z);
		displaylist_color(d, light);
		displaylist_texcoord(d, tex_x, tex_y + 16);
		displaylist_pos(d, x + BLK_LEN, y + BLK_LEN, z);
		displaylist_color(d, light);
		displaylist_texcoord(d, tex_x, tex_y);
		displaylist_pos(d, x, y + BLK_LEN, z + BLK_LEN);
		displaylist_color(d, light);
		displaylist_texcoord(d, tex_x + 16, tex_y);
		displaylist_pos(d, x, y, z + BLK_LEN);
		displaylist_color(d, light);
		displaylist_texcoord(d, tex_x + 16, tex_y + 16);
	}

	return 2;
}

size_t render_block_torch(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only) {
	if(side == SIDE_BOTTOM)
		return 0;

	if(!count_only) {
		int16_t x = W2C_COORD(this->x);
		int16_t y = W2C_COORD(this->y);
		int16_t z = W2C_COORD(this->z);
		uint8_t tex = blocks[this->block->type]->getTextureIndex(this, side);
		uint8_t tex_x = TEX_OFFSET(TEXTURE_X(tex));
		uint8_t tex_y = TEX_OFFSET(TEXTURE_Y(tex));
		uint8_t light = (MAX_U8(this->block->torch_light,
								blocks[this->block->type]->luminance)
						 << 4)
			| this->block->sky_light;

		int s1x, s1y; // wall offset
		int s2x, s2y; // layer shift
		int s3 = 48;  // y offset

		switch(this->block->metadata) {
			case 1:
				s1x = -128;
				s1y = 0;
				s2x = 104;
				s2y = 0;
				break;
			case 2:
				s1x = 128;
				s1y = 0;
				s2x = -104;
				s2y = 0;
				break;
			case 3:
				s1x = 0;
				s1y = -128;
				s2x = 0;
				s2y = 104;
				break;
			case 4:
				s1x = 0;
				s1y = 128;
				s2x = 0;
				s2y = -104;
				break;
			default: s1x = s1y = s2x = s2y = s3 = 0; break;
		}

		switch(side) {
			case SIDE_LEFT:
				displaylist_pos(d, x * BLK_LEN + 112 + s1x, y * BLK_LEN + s3,
								z * BLK_LEN + s1y);
				displaylist_color(d, DIM_LIGHT(light, level_table_1, true, 0));
				displaylist_texcoord(d, tex_x, tex_y + 16);
				displaylist_pos(d, x * BLK_LEN + 112 + s1x + s2x,
								y * BLK_LEN + BLK_LEN + s3,
								z * BLK_LEN + s1y + s2y);
				displaylist_color(d, DIM_LIGHT(light, level_table_1, true, 0));
				displaylist_texcoord(d, tex_x, tex_y);
				displaylist_pos(d, x * BLK_LEN + 112 + s1x + s2x,
								y * BLK_LEN + BLK_LEN + s3,
								z * BLK_LEN + BLK_LEN + s1y + s2y);
				displaylist_color(d, DIM_LIGHT(light, level_table_1, true, 0));
				displaylist_texcoord(d, tex_x + 16, tex_y);
				displaylist_pos(d, x * BLK_LEN + 112 + s1x, y * BLK_LEN + s3,
								z * BLK_LEN + BLK_LEN + s1y);
				displaylist_color(d, DIM_LIGHT(light, level_table_1, true, 0));
				displaylist_texcoord(d, tex_x + 16, tex_y + 16);
				break;
			case SIDE_RIGHT:
				displaylist_pos(d, x * BLK_LEN + 144 + s1x, y * BLK_LEN + s3,
								z * BLK_LEN + s1y);
				displaylist_color(d, DIM_LIGHT(light, level_table_1, true, 0));
				displaylist_texcoord(d, tex_x + 16, tex_y + 16);
				displaylist_pos(d, x * BLK_LEN + 144 + s1x, y * BLK_LEN + s3,
								z * BLK_LEN + BLK_LEN + s1y);
				displaylist_color(d, DIM_LIGHT(light, level_table_1, true, 0));
				displaylist_texcoord(d, tex_x, tex_y + 16);
				displaylist_pos(d, x * BLK_LEN + 144 + s1x + s2x,
								y * BLK_LEN + BLK_LEN + s3,
								z * BLK_LEN + BLK_LEN + s1y + s2y);
				displaylist_color(d, DIM_LIGHT(light, level_table_1, true, 0));
				displaylist_texcoord(d, tex_x, tex_y);
				displaylist_pos(d, x * BLK_LEN + 144 + s1x + s2x,
								y * BLK_LEN + BLK_LEN + s3,
								z * BLK_LEN + s1y + s2y);
				displaylist_color(d, DIM_LIGHT(light, level_table_1, true, 0));
				displaylist_texcoord(d, tex_x + 16, tex_y);
				break;
			case SIDE_BACK:
				displaylist_pos(d, x * BLK_LEN + s1x, y * BLK_LEN + s3,
								z * BLK_LEN + 144 + s1y);
				displaylist_color(d, DIM_LIGHT(light, level_table_2, true, 0));
				displaylist_texcoord(d, tex_x, tex_y + 16);
				displaylist_pos(d, x * BLK_LEN + s1x + s2x,
								y * BLK_LEN + BLK_LEN + s3,
								z * BLK_LEN + 144 + s1y + s2y);
				displaylist_color(d, DIM_LIGHT(light, level_table_2, true, 0));
				displaylist_texcoord(d, tex_x, tex_y);
				displaylist_pos(d, x * BLK_LEN + BLK_LEN + s1x + s2x,
								y * BLK_LEN + BLK_LEN + s3,
								z * BLK_LEN + 144 + s1y + s2y);
				displaylist_color(d, DIM_LIGHT(light, level_table_2, true, 0));
				displaylist_texcoord(d, tex_x + 16, tex_y);
				displaylist_pos(d, x * BLK_LEN + BLK_LEN + s1x,
								y * BLK_LEN + s3, z * BLK_LEN + 144 + s1y);
				displaylist_color(d, DIM_LIGHT(light, level_table_2, true, 0));
				displaylist_texcoord(d, tex_x + 16, tex_y + 16);
				break;
			case SIDE_FRONT:
				displaylist_pos(d, x * BLK_LEN + s1x, y * BLK_LEN + s3,
								z * BLK_LEN + 112 + s1y);
				displaylist_color(d, DIM_LIGHT(light, level_table_2, true, 0));
				displaylist_texcoord(d, tex_x + 16, tex_y + 16);
				displaylist_pos(d, x * BLK_LEN + BLK_LEN + s1x,
								y * BLK_LEN + s3, z * BLK_LEN + 112 + s1y);
				displaylist_color(d, DIM_LIGHT(light, level_table_2, true, 0));
				displaylist_texcoord(d, tex_x, tex_y + 16);
				displaylist_pos(d, x * BLK_LEN + BLK_LEN + s1x + s2x,
								y * BLK_LEN + BLK_LEN + s3,
								z * BLK_LEN + 112 + s1y + s2y);
				displaylist_color(d, DIM_LIGHT(light, level_table_2, true, 0));
				displaylist_texcoord(d, tex_x, tex_y);
				displaylist_pos(d, x * BLK_LEN + s1x + s2x,
								y * BLK_LEN + BLK_LEN + s3,
								z * BLK_LEN + 112 + s1y + s2y);
				displaylist_color(d, DIM_LIGHT(light, level_table_2, true, 0));
				displaylist_texcoord(d, tex_x + 16, tex_y);
				break;
			case SIDE_TOP:
				displaylist_pos(d, x * BLK_LEN + 112 + s1x + s2x * 10 / 16,
								y * BLK_LEN + 160 + s3,
								z * BLK_LEN + 112 + s1y + s2y * 10 / 16);
				displaylist_color(d, light);
				displaylist_texcoord(d, tex_x + 7, tex_y + 6);
				displaylist_pos(d, x * BLK_LEN + 144 + s1x + s2x * 10 / 16,
								y * BLK_LEN + 160 + s3,
								z * BLK_LEN + 112 + s1y + s2y * 10 / 16);
				displaylist_color(d, light);
				displaylist_texcoord(d, tex_x + 9, tex_y + 6);
				displaylist_pos(d, x * BLK_LEN + 144 + s1x + s2x * 10 / 16,
								y * BLK_LEN + 160 + s3,
								z * BLK_LEN + 144 + s1y + s2y * 10 / 16);
				displaylist_color(d, light);
				displaylist_texcoord(d, tex_x + 9, tex_y + 8);
				displaylist_pos(d, x * BLK_LEN + 112 + s1x + s2x * 10 / 16,
								y * BLK_LEN + 160 + s3,
								z * BLK_LEN + 144 + s1y + s2y * 10 / 16);
				displaylist_color(d, light);
				displaylist_texcoord(d, tex_x + 7, tex_y + 8);
				break;
			default: break;
		}
	}

	return 1;
}

size_t render_block_cactus(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only) {
	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			BLK_LEN, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true,
			(side == SIDE_TOP || side == SIDE_BOTTOM) ? 0 : 16, false, 0, side,
			vertex_light);
	return 1;
}

size_t render_block_portal(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only) {
	// TODO: handle case with neighbour obsidian blocks correctly (low priority)
	if(it->block->type == BLOCK_OBSIDIAN || it->block->type == BLOCK_PORTAL
	   || side == SIDE_TOP || side == SIDE_BOTTOM)
		return 0;

	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			BLK_LEN, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true, 96, false, 0, side,
			vertex_light);
	return 1;
}

size_t render_block_fluid(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only) {
	if(!count_only) {
		uint16_t height = (this->block->metadata & 0x8) ?
			BLK_LEN :
			(8 - this->block->metadata) * 14 * 2;
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			height, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true, 0, false, 0, side,
			vertex_light);
	}

	return 1;
}

size_t render_block_rail(struct displaylist* dl, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only) {
	if(side != SIDE_TOP)
		return 0;

	if(!count_only) {
		int16_t x = W2C_COORD(this->x);
		int16_t y = W2C_COORD(this->y);
		int16_t z = W2C_COORD(this->z);
		uint8_t tex = blocks[this->block->type]->getTextureIndex(this, side);
		uint8_t luminance = blocks[this->block->type]->luminance;

		uint8_t tex_coords[4][2] = {
			{TEX_OFFSET(TEXTURE_X(tex)), TEX_OFFSET(TEXTURE_Y(tex))},
			{TEX_OFFSET(TEXTURE_X(tex)) + 16, TEX_OFFSET(TEXTURE_Y(tex))},
			{TEX_OFFSET(TEXTURE_X(tex)) + 16, TEX_OFFSET(TEXTURE_Y(tex)) + 16},
			{TEX_OFFSET(TEXTURE_X(tex)), TEX_OFFSET(TEXTURE_Y(tex)) + 16},
		};
		int tex_rotate = 0;

		uint16_t a = 16, b = 16, c = 16, d = 16;

		switch(this->block->metadata & 0x7) {
			case 1: tex_rotate = 1; break;
			case 2:
				b = 272;
				c = 272;
				tex_rotate = 1;
				break;
			case 3:
				a = 272;
				d = 272;
				tex_rotate = 1;
				break;
			case 4:
				a = 272;
				b = 272;
				break;
			case 5:
				c = 272;
				d = 272;
				break;
		}

		if(blocks[this->block->type]->render_block_data.rail_curved_possible) {
			switch(this->block->metadata) {
				case 6: tex_rotate = 0; break;
				case 7: tex_rotate = 3; break;
				case 8: tex_rotate = 2; break;
			}
		}

		displaylist_pos(dl, x * BLK_LEN, y * BLK_LEN + a, z * BLK_LEN);
		displaylist_color(dl,
						  DIM_LIGHT(vertex_light[4], NULL, false, luminance));
		displaylist_texcoord(dl, tex_coords[(tex_rotate + 0) % 4][0],
							 tex_coords[(tex_rotate + 0) % 4][1]);
		displaylist_pos(dl, x * BLK_LEN + BLK_LEN, y * BLK_LEN + b,
						z * BLK_LEN);
		displaylist_color(dl,
						  DIM_LIGHT(vertex_light[5], NULL, false, luminance));
		displaylist_texcoord(dl, tex_coords[(tex_rotate + 1) % 4][0],
							 tex_coords[(tex_rotate + 1) % 4][1]);
		displaylist_pos(dl, x * BLK_LEN + BLK_LEN, y * BLK_LEN + c,
						z * BLK_LEN + BLK_LEN);
		displaylist_color(dl,
						  DIM_LIGHT(vertex_light[6], NULL, false, luminance));
		displaylist_texcoord(dl, tex_coords[(tex_rotate + 2) % 4][0],
							 tex_coords[(tex_rotate + 2) % 4][1]);
		displaylist_pos(dl, x * BLK_LEN, y * BLK_LEN + d,
						z * BLK_LEN + BLK_LEN);
		displaylist_color(dl,
						  DIM_LIGHT(vertex_light[7], NULL, false, luminance));
		displaylist_texcoord(dl, tex_coords[(tex_rotate + 3) % 4][0],
							 tex_coords[(tex_rotate + 3) % 4][1]);
	}

	return 1;
}

size_t render_block_ladder(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only) {
	if((this->block->metadata == 2 && side != SIDE_FRONT)
	   || (this->block->metadata == 3 && side != SIDE_BACK)
	   || (this->block->metadata == 4 && side != SIDE_LEFT)
	   || (this->block->metadata == 5 && side != SIDE_RIGHT))
		return 0;

	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			BLK_LEN, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true, 240, false, 0, side,
			vertex_light);
	return 1;
}

size_t render_block_crops(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only) {
	if(side == SIDE_TOP || side == SIDE_BOTTOM)
		return 0;

	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), -16,
			BLK_LEN, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, false, 64, false, 0, side,
			vertex_light);
	return 1;
}

size_t render_block_cake(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only) {
	if(!count_only) {
		uint8_t tex = blocks[this->block->type]->getTextureIndex(this, side);
		uint8_t luminance = blocks[this->block->type]->luminance;

		switch(side) {
			case SIDE_FRONT:
				render_block_side_adv(d,
									  W2C_COORD(this->x) * BLK_LEN
										  + this->block->metadata * 32 + 16,
									  W2C_COORD(this->y) * BLK_LEN,
									  W2C_COORD(this->z) * BLK_LEN + 16,
									  (7 - this->block->metadata) * 32, 128,
									  TEX_OFFSET(TEXTURE_X(tex)) + 1,
									  TEX_OFFSET(TEXTURE_Y(tex)) + 8, false, 0,
									  true, side, vertex_light, luminance);
				break;
			case SIDE_BACK:
				render_block_side_adv(d,
									  W2C_COORD(this->x) * BLK_LEN
										  + this->block->metadata * 32 + 16,
									  W2C_COORD(this->y) * BLK_LEN,
									  W2C_COORD(this->z) * BLK_LEN + 240,
									  (7 - this->block->metadata) * 32, 128,
									  TEX_OFFSET(TEXTURE_X(tex)) + 1
										  + this->block->metadata * 2,
									  TEX_OFFSET(TEXTURE_Y(tex)) + 8, false, 0,
									  true, side, vertex_light, luminance);
				break;
			case SIDE_TOP:
				render_block_side_adv(d,
									  W2C_COORD(this->x) * BLK_LEN
										  + this->block->metadata * 32 + 16,
									  W2C_COORD(this->y) * BLK_LEN + 128,
									  W2C_COORD(this->z) * BLK_LEN + 16,
									  (7 - this->block->metadata) * 32, 224,
									  TEX_OFFSET(TEXTURE_X(tex)) + 1
										  + this->block->metadata * 2,
									  TEX_OFFSET(TEXTURE_Y(tex)) + 1, false, 0,
									  true, side, vertex_light, luminance);
				break;
			case SIDE_BOTTOM:
				render_block_side_adv(d,
									  W2C_COORD(this->x) * BLK_LEN
										  + this->block->metadata * 32 + 16,
									  W2C_COORD(this->y) * BLK_LEN,
									  W2C_COORD(this->z) * BLK_LEN + 16,
									  (7 - this->block->metadata) * 32, 224,
									  TEX_OFFSET(TEXTURE_X(tex)) + 1
										  + this->block->metadata * 2,
									  TEX_OFFSET(TEXTURE_Y(tex)) + 1, false, 0,
									  true, side, vertex_light, luminance);
				break;
			default:
				render_block_side(
					d, W2C_COORD(this->x), W2C_COORD(this->y),
					W2C_COORD(this->z), 0, 128, tex, luminance, true,
					(side == SIDE_LEFT) ? (16 + this->block->metadata * 32) :
										  16,
					false, 0, side, vertex_light);
		}
	}

	return 1;
}

size_t render_block_farmland(struct displaylist* d, struct block_info* this,
							 enum side side, struct block_info* it,
							 uint8_t* vertex_light, bool count_only) {
	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			240, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true, 0, false, 0, side,
			vertex_light);
	return 1;
}

size_t render_block_stairs_always(struct displaylist* d,
								  struct block_info* this, enum side side,
								  struct block_info* it, uint8_t* vertex_light,
								  bool count_only) {
	enum side facing = (enum side[4]) {SIDE_RIGHT, SIDE_LEFT, SIDE_BACK,
									   SIDE_FRONT}[this->block->metadata & 3];

	if(side != SIDE_TOP && side != blocks_side_opposite(facing))
		return 0;

	if(!count_only) {
		uint8_t tex = blocks[this->block->type]->getTextureIndex(this, side);
		uint8_t luminance = blocks[this->block->type]->luminance;

		if(side == SIDE_TOP) {
			render_block_side(d, W2C_COORD(this->x), W2C_COORD(this->y),
							  W2C_COORD(this->z), 0, BLK_LEN / 2, tex,
							  luminance, true, 0, false, 0, side, vertex_light);
		} else if(side == blocks_side_opposite(facing)) {
			render_block_side(d, W2C_COORD(this->x), W2C_COORD(this->y),
							  W2C_COORD(this->z), 0, BLK_LEN, tex, luminance,
							  true, BLK_LEN / 2, false, 0, side, vertex_light);
		}
	}

	return 1;
}

size_t render_block_stairs(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only) {
	size_t k = 0;
	enum side facing = (enum side[4]) {SIDE_RIGHT, SIDE_LEFT, SIDE_BACK,
									   SIDE_FRONT}[this->block->metadata & 3];
	uint8_t tex = blocks[this->block->type]->getTextureIndex(this, side);
	uint8_t luminance = blocks[this->block->type]->luminance;

	// render "slab"
	if(side == facing) {
		if(!count_only)
			render_block_side(d, W2C_COORD(this->x), W2C_COORD(this->y),
							  W2C_COORD(this->z), 0, BLK_LEN, tex, luminance,
							  true, 0, false, 0, side, vertex_light);
		k++;
	} else if(side != SIDE_TOP) {
		if(!count_only)
			render_block_side(d, W2C_COORD(this->x), W2C_COORD(this->y),
							  W2C_COORD(this->z), 0, BLK_LEN / 2, tex,
							  luminance, true, 0, false, 0, side, vertex_light);
		k++;
	}

	// render top part
	if(side == SIDE_TOP) {
		int tx = (facing == SIDE_RIGHT) ? BLK_LEN / 2 : 0;
		int ty = (facing == SIDE_BACK) ? BLK_LEN / 2 : 0;
		int sx = (facing == SIDE_LEFT || facing == SIDE_RIGHT) ? BLK_LEN / 2 :
																 BLK_LEN;
		int sy = (facing == SIDE_FRONT || facing == SIDE_BACK) ? BLK_LEN / 2 :
																 BLK_LEN;
		if(!count_only)
			render_block_side_adv(d, W2C_COORD(this->x) * BLK_LEN + tx,
								  W2C_COORD(this->y) * BLK_LEN + BLK_LEN,
								  W2C_COORD(this->z) * BLK_LEN + ty, sx, sy,
								  TEX_OFFSET(TEXTURE_X(tex)) + tx / 16,
								  TEX_OFFSET(TEXTURE_Y(tex)) + ty / 16, false,
								  false, true, SIDE_TOP, vertex_light,
								  luminance);
		k++;
	} else if(side != facing && side != blocks_side_opposite(facing)
			  && side != SIDE_BOTTOM) {
		int off[][2] = {
			[SIDE_LEFT] = {0, 0},
			[SIDE_RIGHT] = {BLK_LEN, 0},
			[SIDE_FRONT] = {0, 0},
			[SIDE_BACK] = {0, BLK_LEN},
		};

		if(facing == SIDE_BACK) {
			off[SIDE_LEFT][1] = BLK_LEN / 2;
			off[SIDE_RIGHT][1] = BLK_LEN / 2;
		} else if(facing == SIDE_RIGHT) {
			off[SIDE_FRONT][0] = BLK_LEN / 2;
			off[SIDE_BACK][0] = BLK_LEN / 2;
		}

		// TODO: fix texture offset

		if(!count_only)
			render_block_side_adv(
				d, W2C_COORD(this->x) * BLK_LEN + off[side][0],
				W2C_COORD(this->y) * BLK_LEN + BLK_LEN / 2,
				W2C_COORD(this->z) * BLK_LEN + off[side][1], BLK_LEN / 2,
				BLK_LEN / 2, TEX_OFFSET(TEXTURE_X(tex)),
				TEX_OFFSET(TEXTURE_Y(tex)), false, false, true, side,
				vertex_light, luminance);
		k++;
	}

	return k;
}

size_t render_block_bed(struct displaylist* d, struct block_info* this,
						enum side side, struct block_info* it,
						uint8_t* vertex_light, bool count_only) {
	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			144, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true,
			(side == SIDE_BOTTOM) ? 48 : 0,
			(side == SIDE_RIGHT && (this->block->metadata & 0x3) == 0)
				|| (side == SIDE_BACK && (this->block->metadata & 0x3) == 1)
				|| (side == SIDE_LEFT && (this->block->metadata & 0x3) == 2)
				|| (side == SIDE_FRONT && (this->block->metadata & 0x3) == 3),
			(side == SIDE_TOP || side == SIDE_BOTTOM) ?
				(3 - (this->block->metadata & 0x3)) :
				0,
			side, vertex_light);
	return 1;
}

size_t render_block_fire(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only) {
	if(side == SIDE_TOP)
		return 0;

	size_t k = 0;
	bool is_floating = !blocks[this->neighbours[SIDE_BOTTOM].type]
		|| (blocks[this->neighbours[SIDE_BOTTOM].type]->can_see_through
			&& !blocks[this->neighbours[SIDE_BOTTOM].type]->flammable);

	int16_t x = W2C_COORD(this->x);
	int16_t y = W2C_COORD(this->y);
	int16_t z = W2C_COORD(this->z);
	uint8_t tex = blocks[this->block->type]->getTextureIndex(this, side);
	uint8_t luminance = blocks[this->block->type]->luminance;
	uint8_t tex_x = TEX_OFFSET(TEXTURE_X(tex));
	uint8_t tex_y = TEX_OFFSET(TEXTURE_Y(tex));
	uint16_t height = 360;

	if(is_floating) {
		switch(side) {
			case SIDE_BOTTOM:
				if(blocks[this->neighbours[SIDE_TOP].type]
				   && blocks[this->neighbours[SIDE_TOP].type]->flammable) {
					if(!count_only) {
						render_block_side_adv_v2(
							d, x * BLK_LEN, y * BLK_LEN + BLK_LEN, z * BLK_LEN,
							BLK_LEN, BLK_LEN, 0, -3 * 16, tex_x, tex_y, false,
							0, false, SIDE_BOTTOM, vertex_light, luminance);
						render_block_side_adv_v2(
							d, x * BLK_LEN, y * BLK_LEN + BLK_LEN, z * BLK_LEN,
							BLK_LEN, BLK_LEN, -3 * 16, 0, tex_x, tex_y, false,
							2, false, SIDE_BOTTOM, vertex_light, luminance);
					}
					k += 2;
				}
				break;
			case SIDE_LEFT:
				if(blocks[this->neighbours[SIDE_RIGHT].type]
				   && blocks[this->neighbours[SIDE_RIGHT].type]->flammable) {
					if(!count_only)
						render_block_side_adv_v2(
							d, x * BLK_LEN + BLK_LEN, y * BLK_LEN + 16,
							z * BLK_LEN, BLK_LEN, height, 0, -48, tex_x, tex_y,
							false, false, false, SIDE_LEFT, vertex_light,
							luminance);
					k++;
				}
				break;
			case SIDE_RIGHT:
				if(blocks[this->neighbours[SIDE_LEFT].type]
				   && blocks[this->neighbours[SIDE_LEFT].type]->flammable) {
					if(!count_only)
						render_block_side_adv_v2(
							d, x * BLK_LEN, y * BLK_LEN + 16, z * BLK_LEN,
							BLK_LEN, height, 0, -48, tex_x, tex_y, false, false,
							false, SIDE_RIGHT, vertex_light, luminance);
					k++;
				}
				break;
			case SIDE_FRONT:
				if(blocks[this->neighbours[SIDE_BACK].type]
				   && blocks[this->neighbours[SIDE_BACK].type]->flammable) {
					if(!count_only)
						render_block_side_adv_v2(
							d, x * BLK_LEN, y * BLK_LEN + 16,
							z * BLK_LEN + BLK_LEN, BLK_LEN, height, 0, -48,
							tex_x, tex_y, false, false, false, SIDE_FRONT,
							vertex_light, luminance);
					k++;
				}
				break;
			case SIDE_BACK:
				if(blocks[this->neighbours[SIDE_FRONT].type]
				   && blocks[this->neighbours[SIDE_FRONT].type]->flammable) {
					if(!count_only)
						render_block_side_adv_v2(
							d, x * BLK_LEN, y * BLK_LEN + 16, z * BLK_LEN,
							BLK_LEN, height, 0, -48, tex_x, tex_y, false, false,
							false, SIDE_BACK, vertex_light, luminance);
					k++;
				}
				break;
			default: break;
		}
	} else {
		switch(side) {
			case SIDE_LEFT:
				if(!count_only) {
					render_block_side_adv_v2(
						d, x * BLK_LEN, y * BLK_LEN, z * BLK_LEN, BLK_LEN,
						height, 0, 24, tex_x, tex_y, false, false, false,
						SIDE_LEFT, vertex_light, luminance);

					render_block_side_adv_v2(
						d, x * BLK_LEN + 5 * 16, y * BLK_LEN, z * BLK_LEN,
						BLK_LEN, height, 0, BLK_LEN / 2, tex_x, tex_y, false,
						false, false, SIDE_LEFT, vertex_light, luminance);
				}
				k += 2;
				break;
			case SIDE_RIGHT:
				if(!count_only) {
					render_block_side_adv_v2(
						d, x * BLK_LEN + BLK_LEN, y * BLK_LEN, z * BLK_LEN,
						BLK_LEN, height, 0, 24, tex_x, tex_y, false, false,
						false, SIDE_RIGHT, vertex_light, luminance);

					render_block_side_adv_v2(
						d, x * BLK_LEN + BLK_LEN - 5 * 16, y * BLK_LEN,
						z * BLK_LEN, BLK_LEN, height, 0, BLK_LEN / 2, tex_x,
						tex_y, false, false, false, SIDE_RIGHT, vertex_light,
						luminance);
				}
				k += 2;
				break;
			case SIDE_FRONT:
				if(!count_only) {
					render_block_side_adv_v2(
						d, x * BLK_LEN, y * BLK_LEN, z * BLK_LEN, BLK_LEN,
						height, 0, 24, tex_x, tex_y, false, false, false,
						SIDE_FRONT, vertex_light, luminance);

					render_block_side_adv_v2(
						d, x * BLK_LEN, y * BLK_LEN, z * BLK_LEN + 5 * 16,
						BLK_LEN, height, 0, BLK_LEN / 2, tex_x, tex_y, false,
						false, false, SIDE_FRONT, vertex_light, luminance);
				}
				k += 2;
				break;
			case SIDE_BACK:
				if(!count_only) {
					render_block_side_adv_v2(
						d, x * BLK_LEN, y * BLK_LEN, z * BLK_LEN + BLK_LEN,
						BLK_LEN, height, 0, 24, tex_x, tex_y, false, false,
						false, SIDE_BACK, vertex_light, luminance);

					render_block_side_adv_v2(
						d, x * BLK_LEN, y * BLK_LEN,
						z * BLK_LEN + BLK_LEN - 5 * 16, BLK_LEN, height, 0,
						BLK_LEN / 2, tex_x, tex_y, false, false, false,
						SIDE_BACK, vertex_light, luminance);
				}
				k += 2;
				break;
			default: break;
		}
	}

	return k;
}

size_t render_block_pressure_plate(struct displaylist* d,
								   struct block_info* this, enum side side,
								   struct block_info* it, uint8_t* vertex_light,
								   bool count_only) {
	if(!count_only) {
		uint8_t tex = blocks[this->block->type]->getTextureIndex(this, side);
		uint8_t luminance = blocks[this->block->type]->luminance;
		uint8_t height = this->block->metadata ? 8 : 16;

		switch(side) {
			case SIDE_TOP:
				render_block_side_adv(
					d, W2C_COORD(this->x) * BLK_LEN + 16,
					W2C_COORD(this->y) * BLK_LEN + height,
					W2C_COORD(this->z) * BLK_LEN + 16, BLK_LEN - 32,
					BLK_LEN - 32, TEX_OFFSET(TEXTURE_X(tex)) + 1,
					TEX_OFFSET(TEXTURE_Y(tex)) + 1, false, false, true,
					SIDE_TOP, vertex_light, luminance);
				break;
			case SIDE_BOTTOM:
				render_block_side_adv(
					d, W2C_COORD(this->x) * BLK_LEN + 16,
					W2C_COORD(this->y) * BLK_LEN,
					W2C_COORD(this->z) * BLK_LEN + 16, BLK_LEN - 32,
					BLK_LEN - 32, TEX_OFFSET(TEXTURE_X(tex)) + 1,
					TEX_OFFSET(TEXTURE_Y(tex)) + 1, false, false, true,
					SIDE_BOTTOM, vertex_light, luminance);
				break;
			case SIDE_FRONT:
			case SIDE_LEFT:
				render_block_side_adv(
					d, W2C_COORD(this->x) * BLK_LEN + 16,
					W2C_COORD(this->y) * BLK_LEN,
					W2C_COORD(this->z) * BLK_LEN + 16, BLK_LEN - 32, height,
					TEX_OFFSET(TEXTURE_X(tex)) + 1,
					TEX_OFFSET(TEXTURE_Y(tex)) + 1, false, false, true, side,
					vertex_light, luminance);
				break;
			case SIDE_RIGHT:
				render_block_side_adv(
					d, W2C_COORD(this->x) * BLK_LEN + BLK_LEN - 16,
					W2C_COORD(this->y) * BLK_LEN,
					W2C_COORD(this->z) * BLK_LEN + 16, BLK_LEN - 32, height,
					TEX_OFFSET(TEXTURE_X(tex)) + 1,
					TEX_OFFSET(TEXTURE_Y(tex)) + 1, false, false, true,
					SIDE_RIGHT, vertex_light, luminance);
				break;
			case SIDE_BACK:
				render_block_side_adv(
					d, W2C_COORD(this->x) * BLK_LEN + 16,
					W2C_COORD(this->y) * BLK_LEN,
					W2C_COORD(this->z) * BLK_LEN + BLK_LEN - 16, BLK_LEN - 32,
					height, TEX_OFFSET(TEXTURE_X(tex)) + 1,
					TEX_OFFSET(TEXTURE_Y(tex)) + 1, false, false, true,
					SIDE_BACK, vertex_light, luminance);
				break;
			default: break;
		}
	}

	return 1;
}

size_t render_block_layer(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only) {
	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			(this->block->metadata + 1) * 32,
			blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true, 0, false, 0, side,
			vertex_light);
	return 1;
}

size_t render_block_slab(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only) {
	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			128, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true, 0, false, 0, side,
			vertex_light);
	return 1;
}

size_t render_block_full(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only) {
	if(!count_only)
		render_block_side(
			d, W2C_COORD(this->x), W2C_COORD(this->y), W2C_COORD(this->z), 0,
			BLK_LEN, blocks[this->block->type]->getTextureIndex(this, side),
			blocks[this->block->type]->luminance, true, 0, false, 0, side,
			vertex_light);
	return 1;
}

// TODO: find better solution for this
void render_block_selection(mat4 view_matrix, struct block_info* this) {
	assert(this);

	int pad = 1;
	struct AABB box;

	if(!blocks[this->block->type]
	   || !blocks[this->block->type]->getBoundingBox(this, false, &box))
		return;

	gfx_lighting(false);
	gfx_blending(MODE_BLEND);
	gfx_texture(false);

	mat4 model_view;
	glm_translate_to(view_matrix, (vec3) {this->x, this->y, this->z},
					 model_view);
	gfx_matrix_modelview(model_view);

	GX_Begin(GX_LINES, GX_VTXFMT3, 24);

	// bottom
	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	// top
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	// vertical
	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad,
					-pad + box.z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box.x1 * 256, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box.x1 * 256, box.y2 * 256 + pad,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box.x2 * 256 + pad, -pad + box.y1 * 256,
					box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box.x2 * 256 + pad, box.y2 * 256 + pad, box.z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_End();

	gfx_texture(true);
	gfx_lighting(true);
}
