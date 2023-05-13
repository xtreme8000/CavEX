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

#ifndef TOOL_H
#define TOOL_H

enum tool_type {
	TOOL_TYPE_ANY,
	TOOL_TYPE_PICKAXE,
	TOOL_TYPE_SHOVEL,
	TOOL_TYPE_AXE,
	TOOL_TYPE_SWORD,
	TOOL_TYPE_HOE,
};

enum tool_tier {
	TOOL_TIER_ANY,
	TOOL_TIER_WOOD,
	TOOL_TIER_GOLD,
	TOOL_TIER_STONE,
	TOOL_TIER_IRON,
	TOOL_TIER_DIAMOND,
	TOOL_TIER_MAX,
};

struct block;
struct item;

int tool_tier_divider(enum tool_tier tier);
int tool_dig_delay_ms(struct block* type, struct item* it);

#endif
