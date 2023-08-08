cmake_minimum_required(VERSION 3.14)

project(cavex C)

add_compile_options(-O3 -march=native -g)

find_package(ZLIB REQUIRED)
find_package(Threads REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_executable(cavex
				source/block/aabb.c
				source/block/block_bed.c
				source/block/block_bedrock.c
				source/block/block_bookshelf.c
				source/block/block_bricks.c
				source/block/block_brown_mushroom.c
				source/block/block_cactus.c
				source/block/block_cake.c
				source/block/block_cast_block.c
				source/block/block_chest.c
				source/block/block_clay.c
				source/block/block_cobblestone.c
				source/block/block_cobweb.c
				source/block/block_crops.c
				source/block/block_dirt.c
				source/block/block_dispenser.c
				source/block/block_door.c
				source/block/block_double_slab.c
				source/block/block_farmland.c
				source/block/block_fence.c
				source/block/block_fire.c
				source/block/block_flower.c
				source/block/block_furnace.c
				source/block/block_glass.c
				source/block/block_glowstone.c
				source/block/block_grass.c
				source/block/block_gravel.c
				source/block/block_ice.c
				source/block/block_jukebox.c
				source/block/block_ladder.c
				source/block/block_lava.c
				source/block/block_leaves.c
				source/block/block_log.c
				source/block/block_netherrack.c
				source/block/block_noteblock.c
				source/block/block_obsidian.c
				source/block/block_ore.c
				source/block/block_planks.c
				source/block/block_portal.c
				source/block/block_pressure_plate.c
				source/block/block_pumpkin.c
				source/block/block_rail.c
				source/block/block_red_mushroom.c
				source/block/block_reed.c
				source/block/block_rose.c
				source/block/block_sand.c
				source/block/block_sandstone.c
				source/block/block_sapling.c
				source/block/blocks.c
				source/block/block_slab.c
				source/block/block_snow.c
				source/block/block_spawner.c
				source/block/block_sponge.c
				source/block/block_stairs.c
				source/block/block_stone.c
				source/block/block_tallgrass.c
				source/block/block_tnt.c
				source/block/block_torch.c
				source/block/block_trapdoor.c
				source/block/block_water.c
				source/block/block_wool.c
				source/block/block_workbench.c
				source/block/face_occlusion.c

				source/cNBT/buffer.c
				source/cNBT/nbt_loading.c
				source/cNBT/nbt_parsing.c
				source/cNBT/nbt_treeops.c
				source/cNBT/nbt_util.c

				source/game/gui/screen_ingame.c
				source/game/gui/screen_load_world.c
				source/game/gui/screen_select_world.c
				source/game/gui/screen_inventory.c
				source/game/gui/screen.c
				source/game/camera.c
				source/game/game_state.c

				source/item/window_container.c
				source/item/inventory.c
				source/item/items.c
				source/item/tool.c

				source/network/client_interface.c
				source/network/level_archive.c
				source/network/region_archive.c
				source/network/server_interface.c
				source/network/server_local.c
				source/network/server_world.c

				source/graphics/gfx_util.c
				source/graphics/gui_util.c
				source/graphics/render_block.c
				source/graphics/render_item.c
				source/graphics/texture_atlas.c

				source/platform/displaylist.c
				source/platform/gfx.c
				source/platform/input.c
				source/platform/thread.c
				source/platform/time.c
				source/platform/texture.c

				source/chunk_mesher.c
				source/chunk.c
				source/daytime.c
				source/lighting.c
				source/main.c
				source/stack.c
				source/util.c
				source/world.c
				source/config.c

				source/lodepng/lodepng.c

				source/parson/parson.c
			)

target_compile_definitions(cavex PRIVATE PLATFORM_PC CGLM_ALL_UNALIGNED)

set_target_properties(
	cavex PROPERTIES
	C_STANDARD 99
)

set_property(TARGET cavex PROPERTY INTERPROCEDURAL_OPTIMIZATION True)

target_link_libraries(cavex ${CMAKE_THREAD_LIBS_INIT} ZLIB::ZLIB glfw GLEW::GLEW OpenGL::GL m)
