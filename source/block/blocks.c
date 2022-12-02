#include <assert.h>
#include <stdlib.h>

#include "blocks.h"

struct block* blocks[256];

void blocks_init() {
	for(int k = 0; k < 256; k++)
		blocks[k] = NULL;

	blocks[1] = &block_stone;
	blocks[2] = &block_grass;
	blocks[3] = &block_dirt;
	blocks[4] = &block_cobblestone;
	blocks[5] = &block_planks;
	blocks[6] = &block_sapling;
	blocks[7] = &block_bedrock;
	blocks[8] = &block_water;
	blocks[9] = &block_water;
	blocks[10] = &block_lava;
	blocks[11] = &block_lava;
	blocks[12] = &block_sand;
	blocks[13] = &block_gravel;
	blocks[14] = &block_goldore;
	blocks[15] = &block_ironore;
	blocks[16] = &block_coalore;
	blocks[17] = &block_log;
	blocks[18] = &block_leaves;
	// sponge
	blocks[20] = &block_glass;
	blocks[21] = &block_lapisore;
	blocks[22] = &block_lapis;
	// dispenser
	blocks[24] = &block_sandstone;
	// note block
	blocks[26] = &block_bed;
	blocks[27] = &block_powered_rail;
	blocks[28] = &block_detector_rail;
	// sticky piston
	// cobweb
	blocks[31] = &block_tallgrass;
	blocks[32] = &block_deadbush;
	// piston
	// piston head
	blocks[35] = &block_wool;
	// moving piston head
	blocks[37] = &block_flower;
	blocks[38] = &block_rose;
	blocks[39] = &block_brown_mushroom;
	blocks[40] = &block_red_mushroom;
	blocks[41] = &block_gold;
	blocks[42] = &block_iron;
	blocks[43] = &block_double_slab;
	blocks[44] = &block_slab;
	blocks[45] = &block_bricks;
	// tnt (flammable)
	// bookshelf (flammable)
	blocks[48] = &block_mossstone;
	blocks[49] = &block_obsidian;
	blocks[50] = &block_torch;
	blocks[51] = &block_fire;
	blocks[52] = &block_spawner;
	blocks[53] = &block_stairs;
	blocks[54] = &block_chest;
	// redstone wire
	blocks[56] = &block_diamondore;
	blocks[57] = &block_diamond;
	blocks[58] = &block_workbench;
	blocks[59] = &block_crops;
	blocks[60] = &block_farmland;
	blocks[61] = &block_furnaceoff;
	blocks[62] = &block_furnaceon;
	// sign standing
	// wooden door
	blocks[65] = &block_ladder;
	blocks[66] = &block_rail;
	// cobblestone stairs
	// sign wall mounted
	// lever
	// stone pressure plate
	// iron door
	// wooden pressure plate
	blocks[73] = &block_redstoneore;
	blocks[74] = &block_redstoneore;
	blocks[75] = &block_redstone_torch;
	blocks[76] = &block_redstone_torch_lit;
	// button
	blocks[78] = &block_snow;
	blocks[79] = &block_ice;
	// snow block
	blocks[81] = &block_cactus;
	blocks[82] = &block_clay;
	blocks[83] = &block_reed;
	// jukebox
	// fence (flammable)
	blocks[86] = &block_pumpkin;
	blocks[87] = &block_netherrack;
	blocks[88] = &block_soulsand;
	blocks[89] = &block_glowstone;
	blocks[90] = &block_portal;
	blocks[91] = &block_pumpkin_lit;
	blocks[92] = &block_cake;
	// repeater
	// repeater
	blocks[95] = &block_locked_chest;
	// trap door

	for(int k = 0; k < 256; k++) {
		if(blocks[k]) {
			assert(blocks[k]->getRenderType);
			assert(blocks[k]->getMaterial);
			assert(blocks[k]->getTextureIndex);
			assert(blocks[k]->getSideMask);
			assert(blocks[k]->getBoundingBox);
			assert(blocks[k]->getBaseColor);
			assert(blocks[k]->renderBlock);
		}
	}
}

enum side blocks_side_opposite(enum side s) {
	switch(s) {
		default:
		case SIDE_TOP: return SIDE_BOTTOM;
		case SIDE_BOTTOM: return SIDE_TOP;
		case SIDE_LEFT: return SIDE_RIGHT;
		case SIDE_RIGHT: return SIDE_LEFT;
		case SIDE_FRONT: return SIDE_BACK;
		case SIDE_BACK: return SIDE_FRONT;
	}
}

const char* block_side_name(enum side s) {
	switch(s) {
		case SIDE_TOP: return "top";
		case SIDE_BOTTOM: return "bottom";
		case SIDE_LEFT: return "left";
		case SIDE_RIGHT: return "right";
		case SIDE_FRONT: return "front";
		case SIDE_BACK: return "back";
		default: return "invalid";
	}
}

void blocks_side_offset(enum side s, int* x, int* y, int* z) {
	assert(x && y && z);

	switch(s) {
		default:
		case SIDE_TOP:
			*x = 0;
			*y = 1;
			*z = 0;
			break;
		case SIDE_BOTTOM:
			*x = 0;
			*y = -1;
			*z = 0;
			break;
		case SIDE_LEFT:
			*x = -1;
			*y = 0;
			*z = 0;
			break;
		case SIDE_RIGHT:
			*x = 1;
			*y = 0;
			*z = 0;
			break;
		case SIDE_BACK:
			*x = 0;
			*y = 0;
			*z = 1;
			break;
		case SIDE_FRONT:
			*x = 0;
			*y = 0;
			*z = -1;
			break;
	}
}
