/*
	Copyright (c) 2023 ByteBit/xtreme8000

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

#include "../../graphics/gfx_util.h"
#include "../../graphics/gui_util.h"
#include "../../graphics/render_model.h"
#include "../../network/server_interface.h"
#include "../../platform/gfx.h"
#include "../../platform/input.h"
#include "../../platform/time.h"
#include "../game_state.h"
#include "screen.h"

#define GUI_WIDTH 176
#define GUI_HEIGHT 167

struct inv_slot {
	int x, y;
	size_t slot;
};

static bool pointer_has_item;
static bool pointer_available;
static float pointer_x, pointer_y, pointer_angle;
static struct inv_slot slots[INVENTORY_SIZE];
static size_t slots_index;

static size_t selected_slot;

static void screen_inventory_reset(struct screen* s, int width, int height) {
	input_pointer_enable(true);

	if(gstate.local_player)
		gstate.local_player->data.local_player.capture_input = false;

	s->render3D = screen_ingame.render3D;

	pointer_available = false;
	pointer_has_item = false;

	slots_index = 0;

	for(int k = 0; k < INVENTORY_SIZE_MAIN; k++) {
		slots[slots_index++] = (struct inv_slot) {
			.x = (8 + (k % INVENTORY_SIZE_HOTBAR) * 18) * 2,
			.y = (84 + (k / INVENTORY_SIZE_HOTBAR) * 18) * 2,
			.slot = k + INVENTORY_SLOT_MAIN,
		};
	}

	for(int k = 0; k < INVENTORY_SIZE_HOTBAR; k++) {
		if(k
		   == (int)inventory_get_hotbar(
			   windowc_get_latest(gstate.windows[WINDOWC_INVENTORY])))
			selected_slot = slots_index;

		slots[slots_index++] = (struct inv_slot) {
			.x = (8 + k * 18) * 2,
			.y = (84 + 3 * 18 + 4) * 2,
			.slot = k + INVENTORY_SLOT_HOTBAR,
		};
	}

	for(int k = 0; k < INVENTORY_SIZE_ARMOR; k++) {
		slots[slots_index++] = (struct inv_slot) {
			.x = 8 * 2,
			.y = (8 + k * 18) * 2,
			.slot = k + INVENTORY_SLOT_ARMOR,
		};
	}

	for(int k = 0; k < 4; k++) {
		slots[slots_index++] = (struct inv_slot) {
			.x = (88 + (k % 2) * 18) * 2,
			.y = (26 + (k / 2) * 18) * 2,
			.slot = k + INVENTORY_SLOT_CRAFTING,
		};
	}

	slots[slots_index++] = (struct inv_slot) {
		.x = 144 * 2,
		.y = 36 * 2,
		.slot = INVENTORY_SLOT_OUTPUT,
	};
}

static void screen_inventory_update(struct screen* s, float dt) {
	struct inventory* inv
		= windowc_get_latest(gstate.windows[WINDOWC_INVENTORY]);

	if(input_pressed(IB_INVENTORY)) {
		svin_rpc_send(&(struct server_rpc) {
			.type = SRPC_WINDOW_CLOSE,
			.payload.window_close.window = WINDOWC_INVENTORY,
		});

		screen_set(&screen_ingame);
	}

	if(input_pressed(IB_GUI_CLICK)) {
		uint16_t action_id;
		if(windowc_new_action(gstate.windows[WINDOWC_INVENTORY], &action_id,
							  false, slots[selected_slot].slot)) {
			svin_rpc_send(&(struct server_rpc) {
				.type = SRPC_WINDOW_CLICK,
				.payload.window_click.window = WINDOWC_INVENTORY,
				.payload.window_click.action_id = action_id,
				.payload.window_click.right_click = false,
				.payload.window_click.slot = slots[selected_slot].slot,
			});
		}
	} else if(input_pressed(IB_GUI_CLICK_ALT)) {
		uint16_t action_id;
		if(windowc_new_action(gstate.windows[WINDOWC_INVENTORY], &action_id,
							  true, slots[selected_slot].slot)) {
			svin_rpc_send(&(struct server_rpc) {
				.type = SRPC_WINDOW_CLICK,
				.payload.window_click.window = WINDOWC_INVENTORY,
				.payload.window_click.action_id = action_id,
				.payload.window_click.right_click = true,
				.payload.window_click.slot = slots[selected_slot].slot,
			});
		}
	}

	pointer_available = input_pointer(&pointer_x, &pointer_y, &pointer_angle);

	size_t slot_nearest[4]
		= {selected_slot, selected_slot, selected_slot, selected_slot};
	int slot_dist[4] = {INT_MAX, INT_MAX, INT_MAX, INT_MAX};
	int pointer_slot = -1;

	int off_x = (gfx_width() - GUI_WIDTH * 2) / 2;
	int off_y = (gfx_height() - GUI_HEIGHT * 2) / 2;

	for(size_t k = 0; k < slots_index; k++) {
		int dx = slots[k].x - slots[selected_slot].x;
		int dy = slots[k].y - slots[selected_slot].y;

		if(pointer_x >= off_x + slots[k].x
		   && pointer_x < off_x + slots[k].x + 16 * 2
		   && pointer_y >= off_y + slots[k].y
		   && pointer_y < off_y + slots[k].y + 16 * 2)
			pointer_slot = k;

		int distx = dx * dx + dy * dy * 8;
		int disty = dx * dx * 8 + dy * dy;

		if(dx < 0 && distx < slot_dist[0]) {
			slot_nearest[0] = k;
			slot_dist[0] = distx;
		}

		if(dx > 0 && distx < slot_dist[1]) {
			slot_nearest[1] = k;
			slot_dist[1] = distx;
		}

		if(dy < 0 && disty < slot_dist[2]) {
			slot_nearest[2] = k;
			slot_dist[2] = disty;
		}

		if(dy > 0 && disty < slot_dist[3]) {
			slot_nearest[3] = k;
			slot_dist[3] = disty;
		}
	}

	if(pointer_available && pointer_slot >= 0) {
		selected_slot = pointer_slot;
		pointer_has_item = true;
	} else {
		if(input_pressed(IB_GUI_LEFT)) {
			selected_slot = slot_nearest[0];
			pointer_has_item = false;
		}

		if(input_pressed(IB_GUI_RIGHT)) {
			selected_slot = slot_nearest[1];
			pointer_has_item = false;
		}

		if(input_pressed(IB_GUI_UP)) {
			selected_slot = slot_nearest[2];
			pointer_has_item = false;
		}

		if(input_pressed(IB_GUI_DOWN)) {
			selected_slot = slot_nearest[3];
			pointer_has_item = false;
		}
	}
}

static void screen_inventory_render2D(struct screen* s, int width, int height) {
	struct inventory* inv
		= windowc_get_latest(gstate.windows[WINDOWC_INVENTORY]);

	// darken background
	gfx_texture(false);
	gutil_texquad_col(0, 0, 0, 0, 0, 0, width, height, 0, 0, 0, 180);
	gfx_texture(true);

	int off_x = (width - GUI_WIDTH * 2) / 2;
	int off_y = (height - GUI_HEIGHT * 2) / 2;

	// draw inventory
	gfx_bind_texture(&texture_gui_inventory);
	gutil_texquad(off_x, off_y, 0, 0, GUI_WIDTH, GUI_HEIGHT, GUI_WIDTH * 2,
				  GUI_HEIGHT * 2);
	gutil_text(off_x + 86 * 2, off_y + 16 * 2, "\2478Crafting", 16, false);

	struct inv_slot* selection = slots + selected_slot;

	float angle_x
		= atan2f((pointer_has_item ? pointer_x : off_x + selection->x + 8 * 2)
					 - (off_x + 52 * 2),
				 192.0F);
	float angle_y
		= atan2f((pointer_has_item ? pointer_y : off_y + selection->y + 8 * 2)
					 - (off_y + 19 * 2),
				 192.0F);

	mat4 view;
	glm_mat4_identity(view);
	glm_translate(view, (vec3) {off_x + 52 * 2, off_y + 39 * 2, 0.0F});
	glm_scale(view, (vec3) {3.75F, -3.75F, 1.0F});
	glm_rotate_x(view, angle_y * 0.66F * 0.5F, view);
	glm_rotate_y(view, angle_x * 0.5F, view);
	glm_translate(view, (vec3) {0.0F, 10.0F, 0.0F});

	gfx_write_buffers(true, true, true);
	struct item_data held_item, helmet, chestplate, leggings, boots;
	render_model_player(
		view, glm_deg(angle_y * 0.66F * 0.5F), glm_deg(angle_x * 0.5F), 0.0F,
		0.0F, inventory_get_hotbar_item(inv, &held_item) ? &held_item : NULL,
		inventory_get_slot(inv, INVENTORY_SLOT_ARMOR + 0, &helmet) ? &helmet :
																	 NULL,
		inventory_get_slot(inv, INVENTORY_SLOT_ARMOR + 1, &chestplate) ?
			&chestplate :
			NULL,
		inventory_get_slot(inv, INVENTORY_SLOT_ARMOR + 2, &leggings) ?
			&leggings :
			NULL,
		inventory_get_slot(inv, INVENTORY_SLOT_ARMOR + 3, &boots) ? &boots :
																	NULL);
	gfx_write_buffers(true, false, false);

	// draw items
	for(size_t k = 0; k < slots_index; k++) {
		struct item_data item;
		if((selected_slot != k || !inventory_get_picked_item(inv, NULL)
			|| (pointer_available && pointer_has_item))
		   && inventory_get_slot(inv, slots[k].slot, &item))
			gutil_draw_item(&item, off_x + slots[k].x, off_y + slots[k].y, 1);
	}

	gfx_bind_texture(&texture_gui2);

	gutil_texquad(off_x + selection->x - 8, off_y + selection->y - 8, 208, 0,
				  24, 24, 24 * 2, 24 * 2);

	int icon_offset = 32;
	icon_offset += gutil_control_icon(icon_offset, IB_GUI_UP, "Move");
	if(inventory_get_picked_item(inv, NULL)) {
		icon_offset
			+= gutil_control_icon(icon_offset, IB_GUI_CLICK, "Swap item");
	} else if(inventory_get_slot(inv, selection->slot, NULL)) {
		icon_offset
			+= gutil_control_icon(icon_offset, IB_GUI_CLICK, "Pickup item");
		icon_offset
			+= gutil_control_icon(icon_offset, IB_GUI_CLICK_ALT, "Split stack");
	}

	icon_offset += gutil_control_icon(icon_offset, IB_INVENTORY, "Leave");

	struct item_data item;
	if(inventory_get_picked_item(inv, &item)) {
		if(pointer_available && pointer_has_item) {
			gutil_draw_item(&item, pointer_x - 8 * 2, pointer_y - 8 * 2, 0);
		} else {
			gutil_draw_item(&item, off_x + selection->x, off_y + selection->y,
							0);
		}
	} else if(inventory_get_slot(inv, selection->slot, &item)) {
		char* tmp = item_get(&item) ? item_get(&item)->name : "Unknown";
		gfx_blending(MODE_BLEND);
		gfx_texture(false);
		gutil_texquad_col(off_x + selection->x - 4 + 16
							  - gutil_font_width(tmp, 16) / 2,
						  off_y + selection->y - 4 + 46, 0, 0, 0, 0,
						  gutil_font_width(tmp, 16) + 7, 16 + 8, 0, 0, 0, 180);
		gfx_texture(true);
		gfx_blending(MODE_OFF);

		gutil_text(off_x + selection->x + 16 - gutil_font_width(tmp, 16) / 2,
				   off_y + selection->y + 46, tmp, 16, false);
	}

	if(pointer_available) {
		gfx_bind_texture(&texture_pointer);
		gutil_texquad_rt_any(pointer_x, pointer_y, glm_rad(pointer_angle), 0, 0,
							 256, 256, 96, 96);
	}
}

struct screen screen_inventory = {
	.reset = screen_inventory_reset,
	.update = screen_inventory_update,
	.render2D = screen_inventory_render2D,
	.render3D = NULL,
	.render_world = true,
};
