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

#include "../item/inventory.h"

bool inventory_collect(struct inventory* inv, struct item_data* item,
					   uint8_t* slot_priority, size_t slot_length,
					   set_inv_slot_t changes);

extern struct inventory_logic inventory_logic_player;
extern struct inventory_logic inventory_logic_crafting;
extern struct inventory_logic inventory_logic_furnace;