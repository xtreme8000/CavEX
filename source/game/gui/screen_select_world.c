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

#include "../../graphics/gui_util.h"
#include "../../network/level_archive.h"
#include "../../network/server_interface.h"
#include "../../platform/gfx.h"
#include "../../platform/input.h"
#include "../../stack.h"
#include "../../util.h"
#include "../game_state.h"
#include "screen.h"

#include <assert.h>
#include <dirent.h>
#include <m-lib/m-string.h>
#include <string.h>
#include <time.h>

static struct stack* worlds = NULL;

static size_t gui_selection;
static int scroll_offset;

static int top_visible;
static int bottom_visible;
static int height_visible;
static int entry_height = 72;
static int side_padding = 4;

struct world_option {
	string_t name;
	string_t directory;
	string_t path;
	int64_t last_access;
	int64_t byte_size;
};

static void screen_sworld_reset(struct screen* s, int width, int height) {
	input_pointer_enable(true);

	if(worlds) {
		while(!stack_empty(worlds)) {
			struct world_option opt;
			stack_pop(worlds, &opt);
			string_clear(opt.name);
			string_clear(opt.directory);
			string_clear(opt.path);
		}

		stack_destroy(worlds);
		free(worlds);
		worlds = NULL;
	}

	worlds = malloc(sizeof(struct stack));
	stack_create(worlds, 8, sizeof(struct world_option));

	const char* saves_path
		= config_read_string(&gstate.config_user, "paths.worlds", "saves");

	DIR* d = opendir(saves_path);

	if(d) {
		struct dirent* dir;
		while((dir = readdir(d))) {
			if(dir->d_type & DT_DIR && *dir->d_name != '.') {
				struct world_option opt;
				string_init_printf(opt.path, "%s/%s", saves_path, dir->d_name);

				struct level_archive la;
				if(level_archive_create(&la, opt.path)) {
					char name[64];

					if(!level_archive_read(&la, LEVEL_NAME, name, sizeof(name)))
						strcpy(name, "Missing name");

					string_init_set_str(opt.name, name);
					string_init_set_str(opt.directory, dir->d_name);

					if(!level_archive_read(&la, LEVEL_DISK_SIZE, &opt.byte_size,
										   0))
						opt.byte_size = 0;

					if(!level_archive_read(&la, LEVEL_LAST_PLAYED,
										   &opt.last_access, 0))
						opt.last_access = 0;

					opt.last_access /= 1000;

					level_archive_destroy(&la);
					stack_push(worlds, &opt);
				} else {
					string_clear(opt.path);
				}
			}
		}

		closedir(d);
	}

	gui_selection = 0;
	scroll_offset = side_padding;
	top_visible = height * 0.133F;
	bottom_visible = height - 64;
	height_visible = bottom_visible - height * 0.133F;
}

static void screen_sworld_update(struct screen* s, float dt) {
	if(input_pressed(IB_GUI_UP) && gui_selection > 0)
		gui_selection--;

	if(input_pressed(IB_GUI_DOWN) && gui_selection < stack_size(worlds) - 1)
		gui_selection++;

	if(scroll_offset + (int)gui_selection * entry_height < 4)
		scroll_offset = side_padding - (int)gui_selection * entry_height;

	if(scroll_offset + (int)(gui_selection + 1) * entry_height
	   >= height_visible - side_padding)
		scroll_offset = height_visible - side_padding
			- (int)(gui_selection + 1) * entry_height;

	if(stack_size(worlds) > 0 && input_pressed(IB_GUI_CLICK)) {
		struct world_option opt;
		stack_at(worlds, &opt, gui_selection);

		struct server_rpc rpc;
		rpc.type = SRPC_LOAD_WORLD;
		string_init_set(rpc.payload.load_world.name, opt.path);
		svin_rpc_send(&rpc);

		screen_set(&screen_load_world);
	}

	if(input_pressed(IB_HOME))
		gstate.quit = true;
}

static void screen_sworld_render2D(struct screen* s, int width, int height) {
	gutil_bg();

	gutil_text((width - gutil_font_width("Select World", 16)) / 2,
			   top_visible - 16 * 1.5F, "Select World", 16, true);

	gfx_texture(false);
	gutil_texquad_col(0, top_visible, 0, 0, 0, 0, width, height_visible, 0, 0,
					  0, 128);
	gfx_texture(true);

	gfx_scissor(true, 0, top_visible, width, height_visible);

	int offset = scroll_offset;

	for(size_t idx = 0; idx < stack_size(worlds); idx++) {
		struct world_option opt;
		stack_at(worlds, &opt, idx);

		if(gui_selection == idx) {
			gfx_texture(false);
			gutil_texquad_col((width - 440) / 2.0F, top_visible + offset, 0, 0,
							  0, 0, 440, 72, 128, 128, 128, 255);
			gutil_texquad_col((width - 436) / 2.0F, top_visible + 2 + offset, 0,
							  0, 0, 0, 436, 68, 0, 0, 0, 255);
			gfx_texture(true);
		}

		gutil_text((width - 436) / 2.0F + 6, top_visible + 6 + offset,
				   (char*)string_get_cstr(opt.name), 16, true);

		char tmp_time[32];
		strftime(tmp_time, sizeof(tmp_time), "%D %I:%M %p",
				 gmtime(&opt.last_access));

		char tmp[128];
		snprintf(tmp, sizeof(tmp), "\2477%s (%s, %0.2fMB)",
				 string_get_cstr(opt.directory), tmp_time,
				 opt.byte_size / 1000.0F / 1000.0F);

		gutil_text((width - 436) / 2.0F + 6, top_visible + 28 + offset, tmp, 16,
				   true);
		offset += entry_height;
	}

	gfx_scissor(false, 0, 0, 0, 0);

	int icon_offset = 32;
	icon_offset
		+= gutil_control_icon(icon_offset, IB_GUI_UP, "Change selection");
	icon_offset += gutil_control_icon(icon_offset, IB_GUI_CLICK, "Play world");
	icon_offset += gutil_control_icon(icon_offset, IB_HOME, "Quit");
}

struct screen screen_select_world = {
	.reset = screen_sworld_reset,
	.update = screen_sworld_update,
	.render2D = screen_sworld_render2D,
	.render3D = NULL,
	.render_world = false,
};
