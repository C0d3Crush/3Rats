#pragma once

#include "Item.h"
#include <vector>

class Inventory
{
private:
	std::vector<int> inventory_slots;
	int max_slots;

public:
	static const int PICKUP_RADIUS = 48;

	Inventory();
	Inventory(int slot_count);

	void init_inventory(int slot_count);

	// Legacy method (deprecated)
	void set_item_at(int index, int item);

	// New inventory management methods
	bool add_item(int item_id);
	int remove_item(int slot);
	int remove_first_item();
	int get_item(int slot) const;
	int get_first_item() const;
	int get_first_item_slot() const;
	bool has_item() const;
	bool is_full() const;
	int get_item_count() const;
	void clear();
	int get_max_slots() const { return max_slots; }
};