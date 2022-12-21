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

#include <stddef.h>

#include "../../block/blocks.h"
#include "../../platform/graphics/gfx.h"
#include "../../platform/graphics/gfx_util.h"
#include "../../platform/graphics/gui_util.h"
#include "../../platform/input.h"
#include "../game_state.h"

#include <malloc.h>

static void screen_ingame_render3D(struct screen* s, mat4 view) {
	gfx_blending(MODE_BLEND);

	if(gstate.world_loaded && gstate.camera_hit.hit) {
		struct block_data blk
			= world_get_block(&gstate.world, gstate.camera_hit.x,
							  gstate.camera_hit.y, gstate.camera_hit.z);
		gutil_block_selection(view,
							  &(struct block_info) {
								  .block = &blk,
								  .x = gstate.camera_hit.x,
								  .y = gstate.camera_hit.y,
								  .z = gstate.camera_hit.z,
							  });
	}

	gfx_blending(MODE_OFF);

	float dig_lerp = 0.0F;
	float place_lerp = 0.0F;

	if(!gstate.held_item_animation.finished) {
		dig_lerp
			= time_diff_s(gstate.held_item_animation.start, time_get()) / 0.4F;

		if(gstate.held_item_animation.type)
			place_lerp = 1.0F - glm_clamp(dig_lerp * 4.0F, 0, 1);

		if(dig_lerp > 1.0F)
			gstate.held_item_animation.finished = true;
	}

	float sinHalfCircle = sinf(dig_lerp * GLM_PI);
	float sqrtLerpPI = sqrtf(dig_lerp) * GLM_PI;
	float sinHalfCircleWeird = sinf(glm_pow2(dig_lerp) * GLM_PI);

	mat4 model;
	glm_translate_make(
		model,
		(vec3) {0.56F - sinf(sqrtLerpPI) * 0.4F,
				-0.52F - 0 * 0.6F + sinf(sqrtLerpPI * 2.0F) * 0.2F
					- 0.6F * place_lerp /*- 0.4F * sinf(swing_lerp * GLM_PI)*/,
				-0.72F - sinHalfCircle * 0.2F});
	glm_rotate_y(model, glm_rad(45.0F), model);
	glm_rotate_y(model, glm_rad(-sinHalfCircleWeird * 20.0F), model);
	glm_rotate_z(model, glm_rad(-sinf(sqrtLerpPI) * 20.0F), model);
	glm_rotate_x(model, glm_rad(-sinf(sqrtLerpPI) * 80.0F), model);

	glm_scale_uni(model, 0.4F);
	glm_translate(model, (vec3) {-0.5F, -0.5F, -0.5F});

	struct item_data item;
	if(inventory_get_slot(&gstate.inventory,
						  inventory_get_hotbar(&gstate.inventory), &item)
	   && item_get(&item))
		items[item.id]->renderItem(item_get(&item), &item, model, false);
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
			gstate.held_item_animation = (struct held_anim) {
				.start = time_get(),
				.type = false,
				.finished = false,
			};
		} else if(input_pressed(IB_ACTION2)) {
			struct item_data item;
			if(inventory_get_slot(&gstate.inventory,
								  inventory_get_hotbar(&gstate.inventory),
								  &item)
			   && item_is_block(&item)) {
				int x, y, z;
				blocks_side_offset(gstate.camera_hit.side, &x, &y, &z);
				world_set_block(
					&gstate.world, gstate.camera_hit.x + x,
					gstate.camera_hit.y + y, gstate.camera_hit.z + z,
					(struct block_data) {
						.type = item.id,
						.metadata = 0,
						.sky_light = blocks[item.id]->can_see_through ? 15 : 0,
						.torch_light = blocks[item.id]->luminance,
					});
			}
			gstate.held_item_animation = (struct held_anim) {
				.start = time_get(),
				.type = true,
				.finished = false,
			};
		}
	}

	size_t slot = inventory_get_hotbar(&gstate.inventory);

	if(input_pressed(IB_SCROLL_LEFT)) {
		inventory_set_hotbar(&gstate.inventory,
							 (slot == 0) ? INVENTORY_SIZE_HOTBAR - 1 :
										   slot - 1);
	}

	if(input_pressed(IB_SCROLL_RIGHT)) {
		inventory_set_hotbar(&gstate.inventory,
							 (slot == INVENTORY_SIZE_HOTBAR - 1) ? 0 :
																   slot + 1);
	}

	struct block_data in_block
		= world_get_block(&gstate.world, floorf(gstate.camera.x),
						  floorf(gstate.camera.y), floorf(gstate.camera.z));

	render_item_update_light((in_block.torch_light << 4) | in_block.sky_light);
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

	sprintf(str, "(%0.1f, %0.1f, %0.1f) (%0.1f, %0.1f)", gstate.camera.x,
			gstate.camera.y, gstate.camera.z, glm_deg(gstate.camera.rx),
			glm_deg(gstate.camera.ry));
	gutil_text(4, 4 + 17 * 3, str, 16);

	if(gstate.camera_hit.hit) {
		struct block* b
			= blocks[world_get_block(&gstate.world, gstate.camera_hit.x,
									 gstate.camera_hit.y, gstate.camera_hit.z)
						 .type];
		sprintf(str, "side: %s, (%i, %i, %i), %s",
				block_side_name(gstate.camera_hit.side), gstate.camera_hit.x,
				gstate.camera_hit.y, gstate.camera_hit.z, b ? b->name : NULL);
		gutil_text(4, 4 + 17 * 5, str, 16);
	}

	sprintf(str, "mem used %iKiB", mallinfo().uordblks / 1024);
	gutil_text(4, 4 + 17 * 6, str, 16);

	int icon_offset = 32;
	icon_offset += gutil_control_icon(icon_offset, CONTROL_A, "Inventory");
	icon_offset += gutil_control_icon(icon_offset, CONTROL_B, "Jump");
	if(gstate.camera_hit.hit) {
		struct item_data item;
		if(inventory_get_slot(&gstate.inventory,
							  inventory_get_hotbar(&gstate.inventory), &item)
		   && item_get(&item)) {
			icon_offset += gutil_control_icon(
				icon_offset, CONTROL_C, item_is_block(&item) ? "Place" : "Use");
		}
		icon_offset += gutil_control_icon(icon_offset, CONTROL_Z, "Mine");
	}

	gfx_bind_texture(TEXTURE_GUI);

	// draw inventory
	/* gutil_texquad((width - 176 * 2) / 2, (height - 167 * 2) / 2, 176 / 2,
	135, 176 / 2, 79, 176 * 2, 79 * 2); gutil_texquad_rt((width - 176 * 2) /
	2, (height - 167 * 2) / 2 + 79 * 2, 352 / 2, 0, 88 / 2, 176, 176 * 2, 88
	* 2); */

	gfx_bind_texture(TEXTURE_GUI2);

	// draw hotbar
	gutil_texquad((width - 182 * 2) / 2, height - 32 * 8 / 5 - 22 * 2, 0, 0,
				  182, 22, 182 * 2, 22 * 2);

	gfx_blending(MODE_INVERT);
	gutil_texquad((width - 16 * 2) / 2, (height - 16 * 2) / 2, 0, 229, 16, 16,
				  16 * 2, 16 * 2);

	gfx_blending(MODE_OFF);

	for(int k = 0; k < INVENTORY_SIZE_HOTBAR; k++) {
		struct item_data item;
		if(inventory_get_slot(&gstate.inventory, k, &item) && item_get(&item)) {
			mat4 model;
			glm_translate_make(
				model,
				(vec3) {(width - 182 * 2) / 2 + 3 * 2 + 20 * 2 * k,
						height - 32 * 8 / 5 - 19 * 2, 0});
			items[item.id]->renderItem(item_get(&item), &item, model, true);

			if(item.count > 1) {
				char count[4];
				snprintf(count, 4, "%u", item.count);
				gutil_text((width - 182 * 2) / 2 + 20 * 2
							   - gutil_font_width(count, 16) + 20 * 2 * k,
						   height - 32 * 8 / 5 - 10 * 2, count, 16);
			}
		}
	}

	gfx_blending(MODE_BLEND);
	gfx_bind_texture(TEXTURE_GUI2);
	// draw hotbar selection
	gutil_texquad((width - 182 * 2) / 2 - 2
					  + 20 * 2 * inventory_get_hotbar(&gstate.inventory),
				  height - 32 * 8 / 5 - 23 * 2, 208, 0, 24, 24, 24 * 2, 24 * 2);
	gfx_blending(MODE_OFF);
}

struct screen screen_ingame = {
	.reset = NULL,
	.update = screen_ingame_update,
	.render2D = screen_ingame_render2D,
	.render3D = screen_ingame_render3D,
	.render_world = true,
};
