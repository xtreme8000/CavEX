#include <assert.h>
#include <fat.h>
#include <gccore.h>
#include <m-lib/m-dict.h>
#include <m-lib/m-i-list.h>
#include <malloc.h>
#include <math.h>
#include <ogc/lwp_watchdog.h>
#include <ogc/tpl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiiuse/wpad.h>

#include "textures.h"
#include "textures_tpl.h"

#include "block/blocks.h"
#include "chunk.h"
#include "displaylist.h"
#include "stack.h"

#include "cglm/cglm.h"

ILIST_DEF(ilist_chunks, struct chunk, M_POD_OPLIST)
DICT_DEF2(dict_chunks, uint64_t, M_BASIC_OPLIST, struct chunk*, M_PTR_OPLIST)

GXRModeObj* screenMode;
#define FIFO_SIZE (256 * 1024)

u8 colors[256 * 3] ATTRIBUTE_ALIGN(32);

char* chunk_files[] = {
	"x0 z-10.vc",	"x-1 z-2.vc",  "x-3 z-5.vc",  "x-5 z-8.vc",	 "x-8 z-11.vc",
	"x0 z-11.vc",	"x-1 z-3.vc",  "x-3 z-6.vc",  "x-5 z-9.vc",	 "x-8 z-1.vc",
	"x0 z-1.vc",	"x-1 z-4.vc",  "x-3 z-7.vc",  "x-6 z-10.vc", "x-8 z-2.vc",
	"x0 z-2.vc",	"x-1 z-5.vc",  "x-3 z-8.vc",  "x-6 z-11.vc", "x-8 z-3.vc",
	"x0 z-3.vc",	"x-1 z-6.vc",  "x-3 z-9.vc",  "x-6 z-1.vc",	 "x-8 z-4.vc",
	"x0 z-4.vc",	"x-1 z-7.vc",  "x-4 z-10.vc", "x-6 z-2.vc",	 "x-8 z-5.vc",
	"x0 z-5.vc",	"x-1 z-8.vc",  "x-4 z-11.vc", "x-6 z-3.vc",	 "x-8 z-6.vc",
	"x0 z-6.vc",	"x-1 z-9.vc",  "x-4 z-1.vc",  "x-6 z-4.vc",	 "x-8 z-7.vc",
	"x0 z-7.vc",	"x-2 z-10.vc", "x-4 z-2.vc",  "x-6 z-5.vc",	 "x-8 z-8.vc",
	"x0 z-8.vc",	"x-2 z-11.vc", "x-4 z-3.vc",  "x-6 z-6.vc",	 "x-8 z-9.vc",
	"x0 z-9.vc",	"x-2 z-1.vc",  "x-4 z-4.vc",  "x-6 z-7.vc",	 "x-9 z-10.vc",
	"x-10 z-10.vc", "x-2 z-2.vc",  "x-4 z-5.vc",  "x-6 z-8.vc",	 "x-9 z-11.vc",
	"x-10 z-11.vc", "x-2 z-3.vc",  "x-4 z-6.vc",  "x-6 z-9.vc",	 "x-9 z-1.vc",
	"x-10 z-1.vc",	"x-2 z-4.vc",  "x-4 z-7.vc",  "x-7 z-10.vc", "x-9 z-2.vc",
	"x-10 z-2.vc",	"x-2 z-5.vc",  "x-4 z-8.vc",  "x-7 z-11.vc", "x-9 z-3.vc",
	"x-10 z-3.vc",	"x-2 z-6.vc",  "x-4 z-9.vc",  "x-7 z-1.vc",	 "x-9 z-4.vc",
	"x-10 z-4.vc",	"x-2 z-7.vc",  "x-5 z-10.vc", "x-7 z-2.vc",	 "x-9 z-5.vc",
	"x-10 z-5.vc",	"x-2 z-8.vc",  "x-5 z-11.vc", "x-7 z-3.vc",	 "x-9 z-6.vc",
	"x-10 z-6.vc",	"x-2 z-9.vc",  "x-5 z-1.vc",  "x-7 z-4.vc",	 "x-9 z-7.vc",
	"x-10 z-7.vc",	"x-3 z-10.vc", "x-5 z-2.vc",  "x-7 z-5.vc",	 "x-9 z-8.vc",
	"x-10 z-8.vc",	"x-3 z-11.vc", "x-5 z-3.vc",  "x-7 z-6.vc",	 "x-9 z-9.vc",
	"x-10 z-9.vc",	"x-3 z-1.vc",  "x-5 z-4.vc",  "x-7 z-7.vc",	 "x-1 z-10.vc",
	"x-3 z-2.vc",	"x-5 z-5.vc",  "x-7 z-8.vc",  "x-1 z-11.vc", "x-3 z-3.vc",
	"x-5 z-6.vc",	"x-7 z-9.vc",  "x-1 z-1.vc",  "x-3 z-4.vc",	 "x-5 z-7.vc",
	"x-8 z-10.vc",
};

dict_chunks_t chunks;
struct chunk chunks_base[sizeof(chunk_files) / sizeof(*chunk_files) * 8];
struct chunk* chunks_end = chunks_base;
#define CHUNK_TO_ID(x, y, z)                                                   \
	((((int64_t)(z)&0x3FFFFFFF) << 34) | (((int64_t)(x)&0x3FFFFFFF) << 4)      \
	 | ((int64_t)(y)&0xF))

void load_chunk(struct chunk** c, char* file) {
	uint8_t* chunk_data = malloc(16 * 16 * 128 * 3);

	int32_t chunk_x, chunk_z;

	FILE* f = fopen(file, "rb");
	assert(f);
	fread((uint8_t*)&chunk_x + 3, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_x + 2, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_x + 1, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_x + 0, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_z + 3, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_z + 2, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_z + 1, sizeof(uint8_t), 1, f);
	fread((uint8_t*)&chunk_z + 0, sizeof(uint8_t), 1, f);
	fread(chunk_data, 1, 16 * 16 * 128 * 3, f);
	fclose(f);

	struct world w;

	for(int k = 0; k < 8; k++) {
		chunk_init(*c, &w, chunk_x * 16, k * 16, chunk_z * 16);
		dict_chunks_set_at(chunks, CHUNK_TO_ID(chunk_x, k, chunk_z), *c);

		for(int y = 0; y < CHUNK_SIZE; y++) {
			for(int z = 0; z < CHUNK_SIZE; z++) {
				for(int x = 0; x < CHUNK_SIZE; x++) {
					uint8_t blockid
						= chunk_data[(x + ((y + k * 16) * 16 + z) * 16) * 3
									 + 0];
					uint8_t metadata
						= chunk_data[(x + ((y + k * 16) * 16 + z) * 16) * 3
									 + 1];
					uint8_t light
						= chunk_data[(x + ((y + k * 16) * 16 + z) * 16) * 3
									 + 2];

					chunk_set_block(*c, x, y, z,
									(struct block_data) {
										.type = blockid,
										.metadata = metadata,
										.sky_light = light & 0xF,
										.torch_light = light >> 4,
									});
				}
			}
		}

		(*c)++;
	}

	free(chunk_data);
}

struct chunk* world_find_chunk_neighbour(struct world* w, struct chunk* c,
										 enum side s) {
	assert(w && c);

	int x, y, z;
	blocks_side_offset(s, &x, &y, &z);

	if(y + c->y / CHUNK_SIZE < 0
	   || y + c->y / CHUNK_SIZE >= WORLD_HEIGHT / CHUNK_SIZE)
		return NULL;

	struct chunk** res = dict_chunks_get(chunks,
										 CHUNK_TO_ID(x + c->x / CHUNK_SIZE,
													 y + c->y / CHUNK_SIZE,
													 z + c->z / CHUNK_SIZE));

	return res ? *res : NULL;
}

struct chunk* world_chunk_cache = NULL;

struct chunk* world_find_chunk(struct world* w, w_coord_t x, w_coord_t y,
							   w_coord_t z) {
	assert(w);

	if(y < 0 || y >= WORLD_HEIGHT)
		return NULL;

	int cx = x < 0 ? ((x + 1) / CHUNK_SIZE - 1) : x / CHUNK_SIZE;
	int cy = y / CHUNK_SIZE;
	int cz = z < 0 ? ((z + 1) / CHUNK_SIZE - 1) : z / CHUNK_SIZE;

	if(world_chunk_cache
	   && CHUNK_TO_ID(cx, cy, cz)
		   == CHUNK_TO_ID(world_chunk_cache->x / CHUNK_SIZE,
						  world_chunk_cache->y / CHUNK_SIZE,
						  world_chunk_cache->z / CHUNK_SIZE))
		return world_chunk_cache;

	struct chunk** res = dict_chunks_get(chunks, CHUNK_TO_ID(cx, cy, cz));

	if(res)
		world_chunk_cache = *res;

	return res ? *res : NULL;
}

void* frameBuffer[3];

static void flip_buffers() {
	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(frameBuffer[0], GX_TRUE);
	VIDEO_SetNextFramebuffer(frameBuffer[0]);
	VIDEO_Flush();
}

void draw_dirt_bg() {
	GX_Begin(GX_QUADS, GX_VTXFMT2, 4 * 13 * 8);
	for(int y = 0; y < 8; y++) {
		for(int x = 0; x < 13; x++) {
			uint8_t scale = 16 * 4;

			GX_Position3s16(x * scale, y * scale, -9);
			GX_Color4u8(0x40, 0x40, 0x40, 0xFF);
			GX_TexCoord2u8(39, 3);
			GX_Position3s16(x * scale + scale, y * scale, -9);
			GX_Color4u8(0x40, 0x40, 0x40, 0xFF);
			GX_TexCoord2u8(55, 3);
			GX_Position3s16(x * scale + scale, y * scale + scale, -9);
			GX_Color4u8(0x40, 0x40, 0x40, 0xFF);
			GX_TexCoord2u8(55, 19);
			GX_Position3s16(x * scale, y * scale + scale, -9);
			GX_Color4u8(0x40, 0x40, 0x40, 0xFF);
			GX_TexCoord2u8(39, 19);
		}
	}
	GX_End();
}

void setup_drawing(bool bg) {
	Mtx44 projection;
	Mtx44 identity;
	guMtxIdentity(identity);

	guOrtho(projection, 0, 480, 0, 782, 0, 10);
	GX_LoadProjectionMtx(projection, GX_ORTHOGRAPHIC);
	GX_LoadPosMtxImm(identity, GX_PNMTX0);

	GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
	GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);

	if(bg) {
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		draw_dirt_bg();
	}

	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
}

uint8_t font_char_width[256] = {
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4, 1, 4, 5, 5, 5, 5, 2, 4, 4, 4, 5,
	1, 5, 1, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 1, 1, 4, 5, 4, 5, 6, 5,
	5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 3, 5, 3, 5, 5, 2, 5, 5, 5, 5, 5, 4, 5, 5, 1, 5, 4, 2, 5,
	5, 5, 5, 5, 5, 5, 4, 5, 5, 5, 5, 5, 5, 4, 1, 4, 6, 5,
};

void draw_text(int x, int y, char* str, int scale) {
	GX_Begin(GX_QUADS, GX_VTXFMT1, strlen(str) * 4 * 2);

	while(*str) {
		uint8_t tex_x = *str % 16 * 8;
		uint8_t tex_y = *str / 16 * 8;
		uint8_t width = (font_char_width[(int)*str] + 1) * scale / 8;

		GX_Position3s16(x + scale / 8, y + scale / 8, -2);
		GX_Color4u8(0x80, 0x80, 0x80, 0xFF);
		GX_TexCoord2u8(tex_x, tex_y);
		GX_Position3s16(x + scale + scale / 8, y + scale / 8, -2);
		GX_Color4u8(0x80, 0x80, 0x80, 0xFF);
		GX_TexCoord2u8(tex_x + 8, tex_y);
		GX_Position3s16(x + scale + scale / 8, y + scale + scale / 8, -2);
		GX_Color4u8(0x80, 0x80, 0x80, 0xFF);
		GX_TexCoord2u8(tex_x + 8, tex_y + 8);
		GX_Position3s16(x + scale / 8, y + scale + scale / 8, -2);
		GX_Color4u8(0x80, 0x80, 0x80, 0xFF);
		GX_TexCoord2u8(tex_x, tex_y + 8);

		GX_Position3s16(x, y, -1);
		GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GX_TexCoord2u8(tex_x, tex_y);
		GX_Position3s16(x + scale, y, -1);
		GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GX_TexCoord2u8(tex_x + 8, tex_y);
		GX_Position3s16(x + scale, y + scale, -1);
		GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GX_TexCoord2u8(tex_x + 8, tex_y + 8);
		GX_Position3s16(x, y + scale, -1);
		GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
		GX_TexCoord2u8(tex_x, tex_y + 8);

		x += width;
		str++;
	}

	GX_End();
}

void draw_textured_quad(int x, int y, int tx, int ty, int sx, int sy,
						int scale) {
	GX_Begin(GX_QUADS, GX_VTXFMT2, 4);
	GX_Position3s16(x, y, -2);
	GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
	GX_TexCoord2u8(tx, ty);
	GX_Position3s16(x + sx * scale, y, -2);
	GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
	GX_TexCoord2u8(tx + sx, ty);
	GX_Position3s16(x + sx * scale, y + sy * scale, -2);
	GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
	GX_TexCoord2u8(tx + sx, ty + sy);
	GX_Position3s16(x, y + sy * scale, -2);
	GX_Color4u8(0xFF, 0xFF, 0xFF, 0xFF);
	GX_TexCoord2u8(tx, ty + sy);
	GX_End();
}

static void bfs(struct world* w, ilist_chunks_t render, float x, float y,
				float z, vec4* planes) {
	for(struct chunk* c = chunks_base; c != chunks_end; c++) {
		c->tmp_data.visited = false;
		c->tmp_data.steps = 0;
	}

	enum side sides[6]
		= {SIDE_TOP, SIDE_LEFT, SIDE_BACK, SIDE_BOTTOM, SIDE_RIGHT, SIDE_FRONT};

	struct chunk* c_camera = world_find_chunk(w, x, y, z);

	ilist_chunks_t queue;
	ilist_chunks_init(queue);

	if(!c_camera)
		return;

	ilist_chunks_push_back(queue, c_camera);
	c_camera->tmp_data.from = SIDE_MAX;
	c_camera->tmp_data.used_exit_sides = 0;
	c_camera->tmp_data.steps = 0;
	c_camera->tmp_data.visited = true;

	while(!ilist_chunks_empty_p(queue)) {
		struct chunk* current = ilist_chunks_pop_front(queue);
		ilist_chunks_push_back(render, current);

		for(int s = 0; s < 6; s++) {
			struct chunk* neigh
				= world_find_chunk_neighbour(w, current, sides[s]);

			if(neigh && !neigh->tmp_data.visited && neigh->tmp_data.steps < 6
			   && !(current->tmp_data.used_exit_sides & (1 << sides[s]))
			   && (current->tmp_data.from == SIDE_MAX
				   || current->reachable[current->tmp_data.from]
					   & (1 << sides[s]))
			   && glm_aabb_frustum(
				   (vec3[2]) {{neigh->x, neigh->y, neigh->z},
							  {neigh->x + CHUNK_SIZE, neigh->y + CHUNK_SIZE,
							   neigh->z + CHUNK_SIZE}},
				   planes)) {
				ilist_chunks_push_back(queue, neigh);
				neigh->tmp_data.from = blocks_side_opposite(sides[s]);
				neigh->tmp_data.used_exit_sides
					= current->tmp_data.used_exit_sides
					| (1 << blocks_side_opposite(sides[s]));
				neigh->tmp_data.steps
					= current->tmp_data.steps + (neigh->y < 64) ? 1 : 0;
				neigh->tmp_data.visited = true;
			}
		}
	}
}

mqbox_t frame_draw;
mqbox_t frame_empty;

/*static void* thread_vsync(void* user) {
	void* current_frame = NULL;

	while(1) {
		mqmsg_t input_frame;
		MQ_Receive(frame_draw, &input_frame, MQ_MSG_BLOCK);

		VIDEO_SetNextFramebuffer(input_frame);
		VIDEO_Flush();

		if(current_frame)
			MQ_Send(frame_empty, current_frame, MQ_MSG_BLOCK);

		current_frame = input_frame;

		VIDEO_WaitVSync();
	}

	return NULL;
}*/

static volatile mqmsg_t current_frame = NULL;

static void copy_buffers(u32 cnt) {
	mqmsg_t input_frame;

	if(MQ_Receive(frame_draw, &input_frame, MQ_MSG_NOBLOCK)) {
		VIDEO_SetNextFramebuffer(input_frame);
		VIDEO_Flush();

		if(current_frame)
			MQ_Send(frame_empty, current_frame, MQ_MSG_BLOCK);

		current_frame = input_frame;
	}
}

bool is_block_air(struct ray* r, w_coord_t x, w_coord_t y, w_coord_t z,
				  enum side* s) {
	struct world w;
	struct block_data blk = world_get_block(&w, x, y, z);

	if(blocks[blk.type]) {
		struct AABB bbox;

		if(blocks[blk.type]->getBoundingBox(
			   &(struct block_info) {
				   .block = &blk, .world = &w, .x = x, .y = y, .z = z},
			   false, &bbox)) {
			aabb_translate(&bbox, x, y, z);
			return !aabb_intersection_ray(&bbox, r, s);
		} else {
			return true;
		}
	}

	return true;
}

// all vectors normalized
float plane_distance(vec3 n, vec3 p0, vec3 l0, vec3 l) {
	float d = glm_vec3_dot(n, l);

	if(d < GLM_FLT_EPSILON)
		return FLT_MAX;

	vec3 tmp;
	glm_vec3_sub(p0, l0, tmp);
	return glm_vec3_dot(tmp, n) / d;
}

bool camera_pick(float gx0, float gy0, float gz0, float gx1, float gy1,
				 float gz1, int* bx, int* by, int* bz, enum side* s) {
	int sx = gx1 > gx0 ? 1 : -1;
	int sy = gy1 > gy0 ? 1 : -1;
	int sz = gz1 > gz0 ? 1 : -1;

	int gx = floor(gx0);
	int gy = floor(gy0);
	int gz = floor(gz0);

	int x1 = floor(gx1);
	int y1 = floor(gy1);
	int z1 = floor(gz1);

	vec3 dir = {gx1 - gx0, gy1 - gy0, gz1 - gz0};
	glm_vec3_normalize(dir);

	while(1) {
		if(!is_block_air(&(struct ray) {.x = gx0,
										.y = gy0,
										.z = gz0,
										.dx = dir[0],
										.dy = dir[1],
										.dz = dir[2]},
						 gx, gy, gz, s)) {
			*bx = gx;
			*by = gy;
			*bz = gz;
			return true;
		}

		if(gx == x1 && gy == y1 && gz == z1)
			return false;

		float t1 = plane_distance((vec3) {sx, 0, 0},
								  (vec3) {gx + (sx + 1) / 2, 0, 0},
								  (vec3) {gx0, gy0, gz0}, dir);
		float t2 = plane_distance((vec3) {0, sy, 0},
								  (vec3) {0, gy + (sy + 1) / 2, 0},
								  (vec3) {gx0, gy0, gz0}, dir);
		float t3 = plane_distance((vec3) {0, 0, sz},
								  (vec3) {0, 0, gz + (sz + 1) / 2},
								  (vec3) {gx0, gy0, gz0}, dir);

		if(t1 <= t2 && t1 <= t3) {
			gx += sx;
		} else if(t2 <= t1 && t2 <= t3) {
			gy += sy;
		} else if(t3 <= t1 && t3 <= t2) {
			gz += sz;
		}
	}
}

void draw_selection_box(struct AABB* box) {
	assert(box);

	int pad = 1;

	GX_Begin(GX_LINES, GX_VTXFMT3, 24);

	// bottom
	GX_Position3s16(-pad + box->x1 * 256, -pad + box->y1 * 256,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box->x2 * 256 + pad, -pad + box->y1 * 256,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box->x1 * 256, -pad + box->y1 * 256,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box->x1 * 256, -pad + box->y1 * 256,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box->x2 * 256 + pad, -pad + box->y1 * 256,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box->x2 * 256 + pad, -pad + box->y1 * 256,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box->x2 * 256 + pad, -pad + box->y1 * 256,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box->x1 * 256, -pad + box->y1 * 256,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	// top
	GX_Position3s16(-pad + box->x1 * 256, box->y2 * 256 + pad,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box->x2 * 256 + pad, box->y2 * 256 + pad,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box->x1 * 256, box->y2 * 256 + pad,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box->x1 * 256, box->y2 * 256 + pad,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box->x2 * 256 + pad, box->y2 * 256 + pad,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box->x2 * 256 + pad, box->y2 * 256 + pad,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box->x2 * 256 + pad, box->y2 * 256 + pad,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box->x1 * 256, box->y2 * 256 + pad,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	// vertical
	GX_Position3s16(-pad + box->x1 * 256, -pad + box->y1 * 256,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box->x1 * 256, box->y2 * 256 + pad,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box->x2 * 256 + pad, -pad + box->y1 * 256,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box->x2 * 256 + pad, box->y2 * 256 + pad,
					-pad + box->z1 * 256);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(-pad + box->x1 * 256, -pad + box->y1 * 256,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(-pad + box->x1 * 256, box->y2 * 256 + pad,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_Position3s16(box->x2 * 256 + pad, -pad + box->y1 * 256,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);
	GX_Position3s16(box->x2 * 256 + pad, box->y2 * 256 + pad,
					box->z2 * 256 + pad);
	GX_Color4u8(0, 0, 0, 153);
	GX_TexCoord2u8(0, 0);

	GX_End();
}

int main(void) {
	settime((uint64_t)0);

	float light_lookup[16] = {
		0.05F,	0.067F, 0.085F, 0.106F, 0.129F, 0.156F, 0.186F, 0.221F,
		0.261F, 0.309F, 0.367F, 0.437F, 0.525F, 0.638F, 0.789F, 1.0F,
	};

	for(int sky = 0; sky < 16; sky++) {
		for(int torch = 0; torch < 16; torch++) {
			float gray = fmaxf(light_lookup[torch], light_lookup[sky]) * 255.0F;
			colors[(torch * 16 + sky) * 3 + 0] = gray;
			colors[(torch * 16 + sky) * 3 + 1] = gray;
			colors[(torch * 16 + sky) * 3 + 2] = gray;
		}
	}

	Mtx44 projection;
	GXColor backgroundColor = {0x79, 0xA6, 0xFF, 255};
	void* fifoBuffer = NULL;

	VIDEO_Init();
	WPAD_Init();
	WPAD_SetDataFormat(0, WPAD_FMT_BTNS_ACC_IR);

	screenMode = VIDEO_GetPreferredMode(NULL);
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));
	frameBuffer[2] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));

	MQ_Init(&frame_draw, 3);
	MQ_Init(&frame_empty, 3);

	MQ_Send(frame_empty, frameBuffer[0], MQ_MSG_BLOCK);
	MQ_Send(frame_empty, frameBuffer[1], MQ_MSG_BLOCK);
	MQ_Send(frame_empty, frameBuffer[2], MQ_MSG_BLOCK);

	if(CONF_GetAspectRatio() == CONF_ASPECT_16_9) {
		screenMode->viWidth = 678;
	} else {
		screenMode->viWidth = 672;
	}

	if(VIDEO_GetCurrentTvMode() == VI_PAL) {
		// screenMode->viHeight = VI_MAX_HEIGHT_PAL;
		screenMode->viXOrigin = (VI_MAX_WIDTH_PAL - screenMode->viWidth) / 2;
		screenMode->viYOrigin = (VI_MAX_HEIGHT_PAL - screenMode->viHeight) / 2;
	} else {
		// screenMode->viHeight = VI_MAX_HEIGHT_NTSC;
		screenMode->viXOrigin = (VI_MAX_WIDTH_NTSC - screenMode->viWidth) / 2;
		screenMode->viYOrigin = (VI_MAX_HEIGHT_NTSC - screenMode->viHeight) / 2;
	}

	s8 hoffset = 0;
	CONF_GetDisplayOffsetH(&hoffset);
	screenMode->viXOrigin += hoffset;

	VIDEO_Configure(screenMode);
	VIDEO_SetNextFramebuffer(frameBuffer[0]);
	VIDEO_SetPreRetraceCallback(copy_buffers);
	VIDEO_SetBlack(false);
	VIDEO_Flush();

	fatInitDefault();

	fifoBuffer = MEM_K0_TO_K1(memalign(32, FIFO_SIZE));
	memset(fifoBuffer, 0, FIFO_SIZE);

	GX_Init(fifoBuffer, FIFO_SIZE);
	GX_SetCopyClear(backgroundColor, 0x00FFFFFF);
	GX_SetViewport(0, 0, screenMode->fbWidth, screenMode->efbHeight, 0, 1);
	GX_SetDispCopyYScale(
		GX_GetYScaleFactor(screenMode->efbHeight, screenMode->xfbHeight));
	GX_SetScissor(0, 0, screenMode->fbWidth, screenMode->efbHeight);
	GX_SetDispCopySrc(0, 0, screenMode->fbWidth, screenMode->efbHeight);
	GX_SetDispCopyDst(screenMode->fbWidth, screenMode->xfbHeight);
	GX_SetCopyFilter(screenMode->aa, screenMode->sample_pattern, GX_TRUE,
					 screenMode->vfilter);
	GX_SetFieldMode(screenMode->field_rendering,
					((screenMode->viHeight == 2 * screenMode->xfbHeight) ?
						 GX_ENABLE :
						 GX_DISABLE));

	GX_SetCullMode(GX_CULL_BACK);
	GX_CopyDisp(frameBuffer[0], GX_TRUE);
	GX_SetDispCopyGamma(GX_GM_1_0);

	GX_InvalidateTexAll();
	GX_ClearVtxDesc();
	GX_SetVtxDesc(GX_VA_POS, GX_DIRECT);
	GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
	GX_SetVtxDesc(GX_VA_TEX0, GX_DIRECT);

	// blocks
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGB, GX_RGB8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U8, 8);

	// font drawing
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_U8, 7);

	// gui
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_U8, 8);

	// blocks etc with direct color
	GX_SetVtxAttrFmt(GX_VTXFMT3, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
	GX_SetVtxAttrFmt(GX_VTXFMT3, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT3, GX_VA_TEX0, GX_TEX_ST, GX_U8, 8);

	GX_SetArray(GX_VA_CLR0, colors, 3 * sizeof(u8));
	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
	GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);

	GXTexObj terrain, font, anim, gui2;
	TPLFile spriteTPL;
	TPL_OpenTPLFromMemory(&spriteTPL, (void*)textures_tpl, textures_tpl_size);
	TPL_GetTexture(&spriteTPL, texture_terrain, &terrain);
	GX_InitTexObjLOD(&terrain, GX_NEAR, GX_NEAR, 0, 0, 0, GX_DISABLE,
					 GX_DISABLE, GX_ANISO_1);
	GX_LoadTexObj(&terrain, GX_TEXMAP0);

	TPL_GetTexture(&spriteTPL, texture_font, &font);
	GX_InitTexObjLOD(&font, GX_NEAR, GX_NEAR, 0, 0, 0, GX_DISABLE, GX_DISABLE,
					 GX_ANISO_1);
	GX_LoadTexObj(&font, GX_TEXMAP1);

	TPL_GetTexture(&spriteTPL, texture_anim, &anim);
	GX_InitTexObjLOD(&anim, GX_NEAR, GX_NEAR, 0, 0, 0, GX_DISABLE, GX_DISABLE,
					 GX_ANISO_1);
	GX_LoadTexObj(&anim, GX_TEXMAP2);

	TPL_GetTexture(&spriteTPL, texture_gui2, &gui2);
	GX_InitTexObjLOD(&gui2, GX_NEAR, GX_NEAR, 0, 0, 0, GX_DISABLE, GX_DISABLE,
					 GX_ANISO_1);
	GX_LoadTexObj(&gui2, GX_TEXMAP4);

	GX_SetAlphaCompare(GX_GEQUAL, 16, GX_AOP_AND, GX_ALWAYS, 0);
	GX_SetZCompLoc(GX_FALSE);

	blocks_init();

	dict_chunks_init(chunks);

	for(size_t k = 0; k < sizeof(chunk_files) / sizeof(*chunk_files); k++)
		load_chunk(&chunks_end, chunk_files[k]);

	flip_buffers();

	float rotate_x = DegToRad(-26), rotate_y = DegToRad(91);
	float center_x = -66.5F, center_y = 72.5F, center_z = -83.5F;
	// float center_x = -34.5F, center_y = 77.5F, center_z = -171.5F;
	//  float center_x = 0, center_y = 72, center_z = 0;
	int center_cnt = 0;
	float avg_chunk_time = 0.0F;
	for(struct chunk* c = chunks_base; c != chunks_end; c++) {
		// center_x += c->x + CHUNK_SIZE / 2;
		// center_z += c->z + CHUNK_SIZE / 2;
		center_cnt++;
		uint64_t s = gettime();
		chunk_check_built(c);
		uint64_t e = gettime();

		avg_chunk_time += (float)(e - s) / (float)(1000UL * TB_TIMER_CLOCK);

		setup_drawing(true);

		char str[16];
		sprintf(str, "%i %0.1fms", center_cnt,
				avg_chunk_time * 1000.0F / center_cnt);
		draw_text(10, 10, str, 16);
		GX_DrawDone();
		flip_buffers();
	}

	// center_x /= center_cnt;
	// center_z /= center_cnt;

	uint64_t last_frame = gettime();

	ilist_chunks_t render;
	mqmsg_t frame = NULL;

	int frame_counter = 0;
	float daytime = 1.0F;

	float speed_x = 0, speed_y = 0, speed_z = 0;

	while(1) {
		uint64_t this_frame = gettime();
		float dt = (float)(this_frame - last_frame)
			/ (float)(1000UL * TB_TIMER_CLOCK);
		float fps = 1.0F / dt;
		last_frame = this_frame;

		WPAD_ScanPads();

		expansion_t e;
		WPAD_Expansion(WPAD_CHAN_0, &e);

		if(e.type == WPAD_EXP_NUNCHUK && e.nunchuk.js.mag > 0.1F) {
			rotate_x -= sin(DegToRad(e.nunchuk.js.ang)) * e.nunchuk.js.mag * dt
				* 2.0F;
			rotate_y -= cos(DegToRad(e.nunchuk.js.ang)) * e.nunchuk.js.mag * dt
				* 2.0F;
		}

		u32 held = WPAD_ButtonsHeld(0);

		float acc_x = 0, acc_y = 0, acc_z = 0;
		float speed_c = 40.0F;
		float air_friction = 0.05F;

		if(held & WPAD_BUTTON_LEFT) {
			acc_x += cos(rotate_x) * speed_c;
			acc_z -= sin(rotate_x) * speed_c;
		}

		if(held & WPAD_BUTTON_RIGHT) {
			acc_x -= cos(rotate_x) * speed_c;
			acc_z += sin(rotate_x) * speed_c;
		}

		if(held & WPAD_BUTTON_UP) {
			acc_x += sin(rotate_x) * sin(rotate_y) * speed_c;
			acc_y += cos(rotate_y) * speed_c;
			acc_z += cos(rotate_x) * sin(rotate_y) * speed_c;
		}

		if(held & WPAD_BUTTON_DOWN) {
			acc_x -= sin(rotate_x) * sin(rotate_y) * speed_c;
			acc_y -= cos(rotate_y) * speed_c;
			acc_z -= cos(rotate_x) * sin(rotate_y) * speed_c;
		}

		if(held & WPAD_BUTTON_B) {
			acc_y += speed_c;
		}

		if(held & WPAD_BUTTON_A) {
			acc_y -= speed_c;
		}

		speed_x += acc_x * dt;
		speed_y += acc_y * dt;
		speed_z += acc_z * dt;

		speed_x *= powf(air_friction, dt);
		speed_y *= powf(air_friction, dt);
		speed_z *= powf(air_friction, dt);

		center_x += speed_x * dt;
		center_y += speed_y * dt;
		center_z += speed_z * dt;

		if(held & WPAD_BUTTON_1) {
			daytime = fminf(daytime + 0.1F * dt, 1.0F);
		}

		if(held & WPAD_BUTTON_2) {
			daytime = fmaxf(daytime - 0.1F * dt, 0.0F);
		}

		rotate_y = fminf(M_PI - 0.05F, fmaxf(0.05F, rotate_y));

		guPerspective(projection, 75.0F, 1.628F, 0.1F, 192.0F);

		Mtx viewMatrix;
		guVector camera = {center_x, center_y, center_z};
		guVector up = {0.0F, 1.0F, 0.0F};
		guVector look = {center_x + sin(rotate_x) * sin(rotate_y),
						 center_y + cos(rotate_y),
						 center_z + cos(rotate_x) * sin(rotate_y)};
		guLookAt(viewMatrix, &camera, &up, &look);

		mat4 dest, test1, test2;
		vec4 planes[6];
		glm_perspective(DegToRad(75.0F), 1.628F, 0.1F, 192.0F, test1);
		glm_lookat((vec3) {center_x, center_y, center_z},
				   (vec3) {look.x, look.y, look.z}, (vec3) {0, 1, 0}, test2);
		glm_mat4_mul(test1, test2, dest);
		glm_frustum_planes(dest, planes);

		ilist_chunks_init(render);

		struct world w;
		bfs(&w, render, center_x, center_y, center_z, planes);

		for(int sky = 0; sky < 16; sky++) {
			for(int torch = 0; torch < 16; torch++) {
				uint8_t gray = roundf(
					fmaxf(light_lookup[torch], light_lookup[sky] * daytime)
					* 255.0F);
				colors[(torch * 16 + sky) * 3 + 0] = gray;
				colors[(torch * 16 + sky) * 3 + 1] = gray;
				colors[(torch * 16 + sky) * 3 + 2] = gray;
			}
		}

		DCStoreRange(colors, sizeof(colors));

		ilist_chunks_it_t it;
		ilist_chunks_it(it, render);

		while(!ilist_chunks_end_p(it)) {
			chunk_pre_render(ilist_chunks_ref(it), viewMatrix);
			ilist_chunks_next(it);
		}

		if(e.type == WPAD_EXP_NUNCHUK) {
			int bx, by, bz;
			enum side s;
			if(camera_pick(center_x, center_y, center_z,
						   center_x + sin(rotate_x) * sin(rotate_y) * 16.0F,
						   center_y + cos(rotate_y) * 16.0F,
						   center_z + cos(rotate_x) * sin(rotate_y) * 16.0F,
						   &bx, &by, &bz, &s)) {
				if(WPAD_ButtonsDown(0) & WPAD_NUNCHUK_BUTTON_C) {
					int x, y, z;
					blocks_side_offset(s, &x, &y, &z);

					struct world w;
					world_set_block(&w, bx + x, by + y, bz + z,
									(struct block_data) {
										.type = BLOCK_DIRT,
										.metadata = 0,
										.sky_light = 0,
										.torch_light = 0,
									});
				}

				if(WPAD_ButtonsDown(0) & WPAD_NUNCHUK_BUTTON_Z) {
					struct world w;
					world_set_block(&w, bx, by, bz,
									(struct block_data) {
										.type = BLOCK_AIR,
										.metadata = 0,
										.sky_light = 15,
										.torch_light = 0,
									});
				}
			}
		}

		uint64_t gpu_wait_start = gettime();
		GX_WaitDrawDone();
		uint64_t gpu_wait_end = gettime();

		if(frame)
			MQ_Send(frame_draw, frame, MQ_MSG_BLOCK);
		MQ_Receive(frame_empty, &frame, MQ_MSG_BLOCK);
		uint64_t vsync_wait = gettime();

		GX_InvVtxCache();
		backgroundColor.r = 0x79 * daytime;
		backgroundColor.g = 0xA6 * daytime;
		backgroundColor.b = 0xFF * daytime;
		GX_SetCopyClear(backgroundColor, 0x00FFFFFF);

		GX_LoadProjectionMtx(projection, GX_PERSPECTIVE);
		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
		GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);

		int chunks_inview = 0;

		ilist_chunks_it(it, render);

		while(!ilist_chunks_end_p(it)) {
			chunk_render(ilist_chunks_ref(it), false, center_x, center_y,
						 center_z);
			chunks_inview++;

			ilist_chunks_next(it);
		}

		enum side s;

		{
			GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
			GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
							GX_LO_NOOP);
			int bx, by, bz;

			if(camera_pick(center_x, center_y, center_z,
						   center_x + sin(rotate_x) * sin(rotate_y) * 16.0F,
						   center_y + cos(rotate_y) * 16.0F,
						   center_z + cos(rotate_x) * sin(rotate_y) * 16.0F,
						   &bx, &by, &bz, &s)) {
				Mtx model, test;
				guMtxTrans(model, bx, by, bz);
				guMtxConcat(viewMatrix, model, test);
				GX_LoadPosMtxImm(test, GX_PNMTX0);

				GX_SetTevOp(GX_TEVSTAGE0, GX_PASSCLR);

				struct world w;
				struct block_data blk = world_get_block(&w, bx, by, bz);

				if(blocks[blk.type]) {
					struct AABB bbox;

					if(blocks[blk.type]->getBoundingBox(
						   &(struct block_info) {.block = &blk,
												 .world = &w,
												 .x = bx,
												 .y = by,
												 .z = bz},
						   false, &bbox))
						draw_selection_box(&bbox);
				}

				GX_SetTevOp(GX_TEVSTAGE0, GX_MODULATE);
			}

			GX_SetVtxDesc(GX_VA_CLR0, GX_INDEX8);
		}

		GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_TRUE);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
						GX_LO_NOOP);
		GX_SetColorUpdate(GX_FALSE);

		Mtx anim_mtx;
		int anim = (frame_counter / 8) % 24;
		guMtxTrans(anim_mtx, (anim / 14) * 0.3515625F, (anim % 14) * 0.0703125F,
				   0.0F);

		GX_LoadTexMtxImm(anim_mtx, GX_TEXMTX0, GX_MTX2x4);
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX0);
		frame_counter++;

		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP2, GX_COLOR0A0);

		for(int pass = 0; pass < 2; pass++) {
			if(pass == 1) {
				GX_SetColorUpdate(GX_TRUE);
				GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_FALSE);
			}

			ilist_chunks_it(it, render);
			while(!ilist_chunks_end_p(it)) {
				chunk_render(ilist_chunks_ref(it), true, center_x, center_y,
							 center_z);
				ilist_chunks_next(it);
			}
		}

		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY);

		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetAlphaCompare(GX_GEQUAL, 16, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_FALSE);
		GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);

		GX_SetVtxDesc(GX_VA_CLR0, GX_DIRECT);
		GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
						GX_LO_NOOP);

		setup_drawing(false);

		char str[64];

		draw_text(4, 4 + 16 * 0, "CavEX Alpha (impl. MC B1.7.3)", 16);

		float gpu_dt = (float)(gpu_wait_end - gpu_wait_start)
			/ (float)(1000UL * TB_TIMER_CLOCK);

		float vsync_dt = (float)(vsync_wait - gpu_wait_end)
			/ (float)(1000UL * TB_TIMER_CLOCK);

		sprintf(str, "%0.1f fps, wait: gpu %0.1fms, vsync %0.1fms", fps,
				gpu_dt * 1000.0F, vsync_dt * 1000.0F);
		draw_text(4, 4 + 17 * 1, str, 16);

		sprintf(str, "%i chunks", chunks_inview);
		draw_text(4, 4 + 17 * 2, str, 16);

		sprintf(str, "(%0.1f, %0.1f, %0.1f)", center_x, center_y, center_z);
		draw_text(4, 4 + 17 * 3, str, 16);

		sprintf(str, "daytime: %0.2f", daytime);
		draw_text(4, 4 + 17 * 4, str, 16);

		sprintf(str, "side: %s", block_side_name(s));
		draw_text(4, 4 + 17 * 5, str, 16);

		GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP4, GX_COLOR0A0);
		/*draw_textured_quad((782 - 182 * 2) / 2, 480 - 22 * 2, 0, 0, 182, 22,
		2); draw_textured_quad((782 - 182 * 2) / 2 - 2, 480 - 23 * 2, 208, 0,
		24, 24, 2);*/

		GX_SetBlendMode(GX_BM_LOGIC, GX_BL_ZERO, GX_BL_ZERO, GX_LO_INV);
		draw_textured_quad((782 - 16 * 2) / 2, (480 - 16 * 2) / 2, 0, 229, 16,
						   16, 2);

		GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);

		GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
		GX_SetColorUpdate(GX_TRUE);
		GX_CopyDisp(frame, GX_TRUE);
		GX_SetDrawDone();

		if(WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
			exit(0);
	}
	return 0;
}
