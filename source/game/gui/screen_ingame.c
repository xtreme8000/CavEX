#include <stddef.h>

#include "../../block/blocks.h"
#include "../../platform/graphics/gfx.h"
#include "../../platform/graphics/gui_util.h"
#include "../../platform/input.h"
#include "../game_state.h"

#include <malloc.h>

static void screen_ingame_render3D(struct screen* s, mat4 view) {
	if(gstate.world_loaded && gstate.camera_hit.hit) {
		struct block_data blk
			= world_get_block(&gstate.world, gstate.camera_hit.x,
							  gstate.camera_hit.y, gstate.camera_hit.z);
		render_block_selection(view,
							   &(struct block_info) {
								   .block = &blk,
								   .x = gstate.camera_hit.x,
								   .y = gstate.camera_hit.y,
								   .z = gstate.camera_hit.z,
							   });
	}
}

static void screen_ingame_update(struct screen* s, float dt) {
	if(gstate.camera_hit.hit) {
		if(input_pressed(IB_ACTION1)) {
			world_set_block(&gstate.world, gstate.camera_hit.x,
							gstate.camera_hit.y, gstate.camera_hit.z,
							(struct block_data) {
								.type = BLOCK_AIR,
								.metadata = 0,
								.sky_light = 15,
								.torch_light = 0,
							});
		} else if(input_pressed(IB_ACTION2)) {
			int x, y, z;
			blocks_side_offset(gstate.camera_hit.side, &x, &y, &z);
			world_set_block(&gstate.world, gstate.camera_hit.x + x,
							gstate.camera_hit.y + y, gstate.camera_hit.z + z,
							(struct block_data) {
								.type = BLOCK_DIRT,
								.metadata = 0,
								.sky_light = 0,
								.torch_light = 0,
							});
		}
	}
}

static void screen_ingame_render2D(struct screen* s, int width, int height) {
	char str[64];
	sprintf(str, GAME_NAME " Alpha %i.%i.%i (impl. MC B1.7.3)", VERSION_MAJOR,
			VERSION_MINOR, VERSION_PATCH);
	gutil_text(4, 4 + 17 * 0, str, 16);

	sprintf(str, "%0.1f fps, wait: gpu %0.1fms, vsync %0.1fms",
			gstate.stats.fps, gstate.stats.dt_gpu * 1000.0F,
			gstate.stats.dt_vsync * 1000.0F);
	gutil_text(4, 4 + 17 * 1, str, 16);

	sprintf(str, "%i chunks", gstate.stats.chunks_rendered);
	gutil_text(4, 4 + 17 * 2, str, 16);

	sprintf(str, "(%0.1f, %0.1f, %0.1f)", gstate.camera.x, gstate.camera.y,
			gstate.camera.z);
	gutil_text(4, 4 + 17 * 3, str, 16);

	// sprintf(str, "daytime: %0.2f", daytime);
	// gutil_text(4, 4 + 17 * 4, str, 16);

	if(gstate.camera_hit.hit) {
		sprintf(str, "side: %s, (%i, %i, %i)",
				block_side_name(gstate.camera_hit.side), gstate.camera_hit.x,
				gstate.camera_hit.y, gstate.camera_hit.z);
		gutil_text(4, 4 + 17 * 5, str, 16);
	}

	sprintf(str, "mem used %iKiB", mallinfo().uordblks / 1024);
	gutil_text(4, 4 + 17 * 6, str, 16);

	int icon_offset = 32;
	icon_offset += gutil_control_icon(icon_offset, CONTROL_A, "Inventory");
	icon_offset += gutil_control_icon(icon_offset, CONTROL_B, "Jump");
	if(gstate.camera_hit.hit) {
		icon_offset += gutil_control_icon(icon_offset, CONTROL_C, "Place");
		icon_offset += gutil_control_icon(icon_offset, CONTROL_Z, "Mine");
	}

	gfx_bind_texture(TEXTURE_GUI);

	// draw inventory
	/*gutil_texquad((782 - 176 * 2) / 2, (480 - 167 * 2) / 2, 176 / 2, 135,
				  176 / 2, 79, 176 * 2, 79 * 2);
	gutil_texquad_rt((782 - 176 * 2) / 2, (480 - 167 * 2) / 2 + 79 * 2,
					 352 / 2, 0, 88 / 2, 176, 176 * 2, 88 * 2);*/

	gfx_bind_texture(TEXTURE_GUI2);
	// draw hotbar
	/*gutil_texquad((782 - 182 * 2) / 2, 480 - 22 * 2, 0, 0, 182, 22,
					   182 * 2, 22 * 2);
	gutil_texquad((782 - 182 * 2) / 2 - 2, 480 - 23 * 2, 208, 0, 24,
					   24, 24 * 2, 24 * 2);*/

	gfx_blending(MODE_INVERT);
	gutil_texquad((width - 16 * 2) / 2, (height - 16 * 2) / 2, 0, 229, 16, 16,
				  16 * 2, 16 * 2);

	gfx_blending(MODE_OFF);
}

struct screen screen_ingame = {
	.reset = NULL,
	.update = screen_ingame_update,
	.render2D = screen_ingame_render2D,
	.render3D = screen_ingame_render3D,
	.render_world = true,
};
