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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef GEKKO
#include <fat.h>
#endif

#include "chunk_mesher.h"
#include "daytime.h"
#include "game/game_state.h"
#include "game/gui/screen.h"
#include "network/client_interface.h"
#include "network/server_interface.h"
#include "network/server_local.h"
#include "platform/graphics/gfx.h"
#include "platform/graphics/gfx_util.h"
#include "platform/graphics/gui_util.h"
#include "platform/input.h"
#include "world.h"

#include "cNBT/nbt.h"
#include "cglm/cglm.h"

void loading_progress(struct world* w, float p) {
	gfx_clear_buffers(0, 0, 0);
	gfx_mode_gui();
	gutil_bg();

	// TODO

	gutil_text((gfx_width() - gutil_font_width("Generating level", 16)) / 2,
			   gfx_height() / 2 - 40, "Generating level", 16);

	gutil_text((gfx_width() - gutil_font_width("Building terrain", 16)) / 2,
			   gfx_height() / 2 + 8, "Building terrain", 16);

	gfx_texture(false);
	gutil_texquad_col((gfx_width() - 200) / 2, gfx_height() / 2 + 32, 0, 0, 0,
					  0, 200, 4, 128, 128, 128);
	gutil_texquad_col((gfx_width() - 200) / 2, gfx_height() / 2 + 32, 0, 0, 0,
					  0, 200 * p, 4, 128, 255, 128);
	gfx_texture(true);

	gfx_finish(false);
}

int main(void) {
	time_reset();
	input_init();
	gfx_setup();
	blocks_init();
	screen_set(&screen_ingame);

#ifdef GEKKO
	fatInitDefault();
#endif

	gstate.camera = (struct camera) {
		.x = -344.7F,
		.y = 78.7F,
		.z = -378.9F,
		.rx = glm_rad(275.0F),
		.ry = glm_rad(92.7F),
		.controller = {0, 0, 0},
	};
	gstate.config.fov = 70.0F;
	gstate.config.render_distance = 192.0F;
	gstate.config.fog_distance = 5 * 16.0F;

	world_create(&gstate.world);

	// world_preload(&gstate.world, loading_progress);
	gstate.world_loaded = false;

	ptime_t last_frame = time_get();
	float daytime = 1.0F;

	inventory_clear(&gstate.inventory);

	clin_init();
	svin_init();
	chunk_mesher_init();

	struct server_local server;
	server_local_create(&server);

	while(1) {
		ptime_t this_frame = time_get();
		gstate.stats.dt = time_diff_s(last_frame, this_frame);
		gstate.stats.fps = 1.0F / gstate.stats.dt;
		last_frame = this_frame;

		input_poll();

		clin_update();

		bool render_world
			= gstate.current_screen->render_world && gstate.world_loaded;

		if(render_world) {
			camera_update(&gstate.camera, gstate.stats.dt);

			world_pre_render(&gstate.world, &gstate.camera, gstate.camera.view);

			struct camera* c = &gstate.camera;
			camera_ray_pick(&gstate.world, c->x, c->y, c->z,
							c->x + sin(c->rx) * sin(c->ry) * 16.0F,
							c->y + cos(c->ry) * 16.0F,
							c->z + cos(c->rx) * sin(c->ry) * 16.0F,
							&gstate.camera_hit);
		} else {
			gstate.camera_hit.hit = false;
		}

		world_build_chunks(&gstate.world, CHUNK_MESHER_QLENGTH);

		if(gstate.current_screen->update)
			gstate.current_screen->update(gstate.current_screen,
										  gstate.stats.dt);

		gfx_flip_buffers(&gstate.stats.dt_gpu, &gstate.stats.dt_vsync);

		// must not modify displaylists while still rendering!
		chunk_mesher_receive();
		world_render_completed(&gstate.world, render_world);

		vec3 top_plane_color, bottom_plane_color, atmosphere_color;
		daytime_sky_colors(daytime, top_plane_color, bottom_plane_color,
						   atmosphere_color);

		gfx_clear_buffers(atmosphere_color[0], atmosphere_color[1],
						  atmosphere_color[2]);
		gfx_fog_color(atmosphere_color[0], atmosphere_color[1],
					  atmosphere_color[2]);

		if(render_world) {
			gfx_mode_world();
			gfx_update_light(daytime_brightness(daytime));

			gfx_matrix_projection(gstate.camera.projection, true);

			gutil_sky_box(gstate.camera.view, daytime_celestial_angle(daytime),
						  top_plane_color, bottom_plane_color);

			gstate.stats.chunks_rendered
				= world_render(&gstate.world, &gstate.camera, false);
		} else {
			gstate.stats.chunks_rendered = 0;
		}

		if(gstate.current_screen->render3D)
			gstate.current_screen->render3D(gstate.current_screen,
											gstate.camera.view);

		if(render_world)
			world_render(&gstate.world, &gstate.camera, true);

		gfx_mode_gui();

		if(gstate.current_screen->render2D)
			gstate.current_screen->render2D(gstate.current_screen, gfx_width(),
											gfx_height());

		if(input_held(IB_SCROLL_LEFT)) {
			daytime += 0.1F * gstate.stats.dt;
		}

		if(input_held(IB_SCROLL_RIGHT)) {
			daytime -= 0.1F * gstate.stats.dt;
		}

		daytime = fmaxf(fminf(daytime, 1), 0);

		gfx_finish(true);

		if(input_pressed(IB_HOME))
			exit(0);
	}
	return 0;
}
