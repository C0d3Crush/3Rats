#include "Inventory.h"

Inventory::Inventory()
{
	inventory_slots.resize(3);
}

Inventory::Inventory(int player_amount)
{
	inventory_slots.resize(player_amount);
}

void Inventory::init_inventory(int player_amount)
{
	inventory_slots.assign(player_amount, 0);
}

void Inventory::set_item_at(int index, int item)
{
	inventory_slots[index] = item;
}
