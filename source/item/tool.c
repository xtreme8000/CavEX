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

#include <assert.h>

#include "../block/blocks.h"

int tool_tier_divider(enum tool_tier tier) {
	switch(tier) {
		case TOOL_TIER_WOOD: return 2;
		case TOOL_TIER_STONE: return 4;
		case TOOL_TIER_IRON: return 6;
		case TOOL_TIER_DIAMOND: return 8;
		case TOOL_TIER_GOLD: return 12;
		default: return 1;
	}
}

int tool_dig_delay_ms(struct block* type, struct item* it) {
	assert(type);

	if(type->digging.hardness <= 0)
		return -1;

	enum tool_type item_type = it ? it->tool.type : TOOL_TYPE_ANY;
	enum tool_tier item_tier
		= (it && item_type != TOOL_TYPE_ANY) ? it->tool.tier : TOOL_TIER_ANY;

	int delay = type->digging.hardness;

	if(type->digging.tool == item_type && item_tier >= type->digging.min) {
		delay /= tool_tier_divider(
			item_tier > type->digging.best ? type->digging.best : item_tier);
	} else if(item_type == TOOL_TYPE_SWORD) {
		delay = delay * 2 / 3;
	} else if(type->digging.min != TOOL_TIER_ANY) {
		delay = delay * 10 / 3;
	}

	return (delay + 49) / 50 * 50; // round up to next tick
}
