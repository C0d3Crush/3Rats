#pragma once

#include "Item.h"
#include <vector>

class Inventory
{
private:
	//Item inventory_slots[];
	std::vector<int> inventory_slots;
public:
	Inventory();
	Inventory(int player_amount);

	void init_inventory(int player_amount);

	void set_item_at(int index, int item);
};