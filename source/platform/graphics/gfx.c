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
#include <gccore.h>
#include <malloc.h>
#include <math.h>
#include <ogc/tpl.h>
#include <string.h>

#include "../../util.h"
#include "gfx.h"

#include "textures.h"
#include "textures_tpl.h"

#define FIFO_SIZE (256 * 1024)

static GXRModeObj* screenMode; // TODO: rename
static void* frameBuffer[3];
static mqmsg_t frame = NULL;
static mqbox_t frame_draw;
static mqbox_t frame_empty;
static void* fifoBuffer = NULL;
static uint8_t colors[256 * 3] ATTRIBUTE_ALIGN(32);

static bool gfx_matrix_texture_prev = false;
static bool gfx_fog_prev = false;

static const float light_lookup[16] = {
	0.05F,	0.067F, 0.085F, 0.106F, 0.129F, 0.156F, 0.186F, 0.221F,
	0.261F, 0.309F, 0.367F, 0.437F, 0.525F, 0.638F, 0.789F, 1.0F,
};

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

static void gfx_load_textures() {
	GXTexObj terrain, font, anim, gui, gui2, items, fog;
	TPLFile spriteTPL;
	TPL_OpenTPLFromMemory(&spriteTPL, (void*)textures_tpl, textures_tpl_size);
	TPL_GetTexture(&spriteTPL, texture_terrain, &terrain);
	GX_InitTexObjFilterMode(&terrain, GX_NEAR, GX_NEAR);
	GX_InitTexObjMaxAniso(&terrain, GX_ANISO_1);
	GX_LoadTexObj(&terrain, GX_TEXMAP0);

	TPL_GetTexture(&spriteTPL, texture_font, &font);
	GX_InitTexObjFilterMode(&font, GX_NEAR, GX_NEAR);
	GX_InitTexObjMaxAniso(&font, GX_ANISO_1);
	GX_LoadTexObj(&font, GX_TEXMAP1);

	TPL_GetTexture(&spriteTPL, texture_anim, &anim);
	GX_InitTexObjFilterMode(&anim, GX_NEAR, GX_NEAR);
	GX_InitTexObjMaxAniso(&anim, GX_ANISO_1);
	GX_LoadTexObj(&anim, GX_TEXMAP2);

	TPL_GetTexture(&spriteTPL, texture_gui, &gui);
	GX_InitTexObjFilterMode(&gui, GX_NEAR, GX_NEAR);
	GX_InitTexObjMaxAniso(&gui, GX_ANISO_1);
	GX_LoadTexObj(&gui, GX_TEXMAP3);

	TPL_GetTexture(&spriteTPL, texture_gui2, &gui2);
	GX_InitTexObjFilterMode(&gui2, GX_NEAR, GX_NEAR);
	GX_InitTexObjMaxAniso(&gui2, GX_ANISO_1);
	GX_LoadTexObj(&gui2, GX_TEXMAP4);

	TPL_GetTexture(&spriteTPL, texture_items, &items);
	GX_InitTexObjFilterMode(&items, GX_NEAR, GX_NEAR);
	GX_InitTexObjMaxAniso(&items, GX_ANISO_1);
	GX_LoadTexObj(&items, GX_TEXMAP5);

	TPL_GetTexture(&spriteTPL, texture_fog, &fog);
	GX_InitTexObjFilterMode(&fog, GX_LINEAR, GX_LINEAR);
	GX_InitTexObjMaxAniso(&fog, GX_ANISO_1);
	GX_InitTexObjWrapMode(&fog, GX_CLAMP, GX_CLAMP);
	GX_LoadTexObj(&fog, GX_TEXMAP7);

	TPL_CloseTPLFile(&spriteTPL);
}

int gfx_width() {
	return 802;
}

int gfx_height() {
	return 480;
}

void gfx_setup() {
	VIDEO_Init();
	screenMode = VIDEO_GetPreferredMode(NULL);
	frameBuffer[0] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));
	frameBuffer[1] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));
	frameBuffer[2] = MEM_K0_TO_K1(SYS_AllocateFramebuffer(screenMode));

	MQ_Init(&frame_draw, 3);
	MQ_Init(&frame_empty, 3);

	MQ_Send(frame_empty, frameBuffer[0], MQ_MSG_BLOCK);
	MQ_Send(frame_empty, frameBuffer[1], MQ_MSG_BLOCK);
	frame = frameBuffer[2];

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

	fifoBuffer = MEM_K0_TO_K1(memalign(32, FIFO_SIZE));
	memset(fifoBuffer, 0, FIFO_SIZE);

	GX_Init(fifoBuffer, FIFO_SIZE);
	gfx_clear_buffers(255, 255, 255);
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
	GX_SetVtxAttrFmt(GX_VTXFMT2, GX_VA_TEX0, GX_TEX_ST, GX_U16, 8);

	// blocks etc with direct color
	GX_SetVtxAttrFmt(GX_VTXFMT3, GX_VA_POS, GX_POS_XYZ, GX_S16, 8);
	GX_SetVtxAttrFmt(GX_VTXFMT3, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
	GX_SetVtxAttrFmt(GX_VTXFMT3, GX_VA_TEX0, GX_TEX_ST, GX_U8, 8);

	GX_SetArray(GX_VA_CLR0, colors, 3 * sizeof(uint8_t));
	GX_SetNumChans(1);
	GX_SetNumTexGens(1);
	GX_SetNumTevStages(1);
	gfx_bind_texture(TEXTURE_TERRAIN);
	gfx_texture(true);
	gfx_alpha_test(true);

	gfx_load_textures();

	GX_SetTexCoordGen(GX_TEXCOORD1, GX_TG_MTX2x4, GX_TG_POS, GX_TEXMTX1);

	gfx_update_light(1.0F);

	GX_DrawDone();
}

void gfx_update_light(float daytime) {
	assert(daytime > -GLM_FLT_EPSILON && daytime < 1.0F + GLM_FLT_EPSILON);

	for(int sky = 0; sky < 16; sky++) {
		for(int torch = 0; torch < 16; torch++) {
			uint8_t gray
				= roundf(fmaxf(light_lookup[torch], light_lookup[sky] * daytime)
						 * 255.0F);
			colors[(torch * 16 + sky) * 3 + 0] = gray;
			colors[(torch * 16 + sky) * 3 + 1] = gray;
			colors[(torch * 16 + sky) * 3 + 2] = gray;
		}
	}

	DCStoreRange(colors, sizeof(colors));
	GX_InvVtxCache();
}

void gfx_clear_buffers(uint8_t r, uint8_t g, uint8_t b) {
	GX_SetCopyClear((GXColor) {r, g, b, 255}, 0x00FFFFFF);
}

void gfx_finish(bool vsync) {
	assert(frame);

	GX_SetZMode(GX_TRUE, GX_LEQUAL, GX_TRUE);
	GX_SetColorUpdate(GX_TRUE);
	GX_CopyDisp(frame, GX_TRUE);
	GX_SetDrawDone();

	if(!vsync) {
		GX_WaitDrawDone();
		VIDEO_SetNextFramebuffer(frame);
		VIDEO_Flush();
	}
}

void gfx_flip_buffers(float* gpu_wait, float* vsync_wait) {
	assert(frame);

	ptime_t gpu_wait_start = time_get();
	GX_WaitDrawDone();
	ptime_t gpu_wait_end = time_get();

	MQ_Send(frame_draw, frame, MQ_MSG_BLOCK);
	MQ_Receive(frame_empty, &frame, MQ_MSG_BLOCK);

	if(vsync_wait)
		*vsync_wait = time_diff_s(gpu_wait_end, time_get());
	if(gpu_wait)
		*gpu_wait = time_diff_s(gpu_wait_start, gpu_wait_end);
}

void gfx_bind_texture(enum gfx_texture tex) {
	switch(tex) {
		case TEXTURE_TERRAIN:
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
			break;
		case TEXTURE_FONT:
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP1, GX_COLOR0A0);
			break;
		case TEXTURE_ANIM:
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP2, GX_COLOR0A0);
			break;
		case TEXTURE_GUI:
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP3, GX_COLOR0A0);
			break;
		case TEXTURE_GUI2:
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP4, GX_COLOR0A0);
			break;
		case TEXTURE_ITEMS:
			GX_SetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP5, GX_COLOR0A0);
			break;
	}
}

void gfx_mode_world() {
	gfx_write_buffers(true, true, true);
}

void gfx_mode_gui() {
	gfx_fog(false);

	Mtx44 projection;

	guOrtho(projection, 0, gfx_height(), 0, gfx_width(), 0, 256);
	GX_LoadProjectionMtx(projection, GX_ORTHOGRAPHIC);
	gfx_matrix_modelview(GLM_MAT4_IDENTITY);

	gfx_lighting(false);
	gfx_blending(MODE_BLEND);
	gfx_alpha_test(true);
	gfx_write_buffers(true, false, false);
}

void gfx_matrix_projection(mat4 proj, bool is_perspective) {
	assert(proj);

	mat4 convert;
	glm_translate_make(convert, (vec3) {0, 0, -1});
	convert[3][3] = 2.0F;
	glm_mat4_mul(convert, proj, convert);
	glm_mat4_transpose(convert);

	GX_LoadProjectionMtx(convert,
						 is_perspective ? GX_PERSPECTIVE : GX_ORTHOGRAPHIC);
}

void gfx_matrix_modelview(mat4 mv) {
	assert(mv);

	mat4 convert;
	glm_mat4_transpose_to(mv, convert);
	GX_LoadPosMtxImm(convert, GX_PNMTX0);
}

void gfx_matrix_texture(bool enable, mat4 tex) {
	if(enable) {
		assert(tex);

		mat4 convert;
		glm_mat4_transpose_to(tex, convert);
		GX_LoadTexMtxImm(convert, GX_TEXMTX0, GX_MTX2x4);
	}

	if(enable != gfx_matrix_texture_prev)
		GX_SetTexCoordGen(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0,
						  enable ? GX_TEXMTX0 : GX_IDENTITY);
	gfx_matrix_texture_prev = enable;
}

void gfx_fog_color(uint8_t r, uint8_t g, uint8_t b) {
	GX_SetTevColor(GX_TEVREG0, (GXColor) {r, g, b, 255});
}

void gfx_fog_pos(float dx, float dz, float distance) {
	assert(distance > 0);

	if(gfx_fog_prev) {
		float dist = 1.0F / (distance * 2.0F);
		GX_LoadTexMtxImm((mat4) {{dist, 0, 0, dx * dist + 0.5F},
								 {0, 0, dist, dz * dist + 0.5F},
								 {0, 0, 0, 0},
								 {0, 0, 0, 1}},
						 GX_TEXMTX1, GX_MTX2x4);
	}
}

void gfx_fog(bool enable) {
	if(enable != gfx_fog_prev) {
		GX_SetNumTexGens(enable ? 2 : 1);
		GX_SetNumTevStages(enable ? 2 : 1);

		if(enable) {
			GX_SetTevOp(GX_TEVSTAGE1, GX_DECAL);
			GX_SetTevOrder(GX_TEVSTAGE1, GX_TEXCOORD1, GX_TEXMAP7, GX_COLOR0A0);
			GX_SetTevColorIn(GX_TEVSTAGE1, GX_CC_CPREV, GX_CC_C0, GX_CC_TEXA,
							 GX_CC_ZERO);
		}
	}

	gfx_fog_prev = enable;
}

void gfx_blending(enum gfx_blend mode) {
	switch(mode) {
		case MODE_BLEND:
			GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA,
							GX_LO_NOOP);
			break;
		case MODE_BLEND2:
			GX_SetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP);
			break;
		case MODE_INVERT:
			GX_SetBlendMode(GX_BM_LOGIC, GX_BL_ZERO, GX_BL_ZERO, GX_LO_INV);
			break;
		case MODE_OFF:
			GX_SetBlendMode(GX_BM_NONE, GX_BL_ONE, GX_BL_ZERO, GX_LO_NOOP);
			break;
	}
}

void gfx_alpha_test(bool enable) {
	if(enable) {
		GX_SetAlphaCompare(GX_GEQUAL, 16, GX_AOP_AND, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_FALSE);
	} else {
		GX_SetAlphaCompare(GX_ALWAYS, 0, GX_AOP_OR, GX_ALWAYS, 0);
		GX_SetZCompLoc(GX_TRUE);
	}
}

void gfx_write_buffers(bool color, bool depth, bool depth_test) {
	GX_SetColorUpdate(color ? GX_TRUE : GX_FALSE);
	GX_SetZMode(depth_test ? GX_TRUE : GX_FALSE, GX_LEQUAL,
				depth ? GX_TRUE : GX_FALSE);
}

void gfx_texture(bool enable) {
	GX_SetTevOp(GX_TEVSTAGE0, enable ? GX_MODULATE : GX_PASSCLR);
}

void gfx_lighting(bool enable) {
	GX_SetVtxDesc(GX_VA_CLR0, enable ? GX_INDEX8 : GX_DIRECT);
}

void gfx_culling(bool enable) {
	GX_SetCullMode(enable ? GX_CULL_BACK : GX_CULL_NONE);
}

void gfx_scissor(bool enable, uint32_t x, uint32_t y, uint32_t width,
				 uint32_t height) {
	if(enable) {
		GX_SetScissor(x, y, width, height);
	} else {
		GX_SetScissor(0, 0, 0x3FF, 0x3FF);
	}
}
