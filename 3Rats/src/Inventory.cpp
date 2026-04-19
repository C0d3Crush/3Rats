#include "Inventory.h"
#include "Logger.h"
#include <algorithm>

Inventory::Inventory()
{
	max_slots = 3;
	inventory_slots.resize(max_slots, -1); // -1 = empty slot
}

Inventory::Inventory(int slot_count)
{
	max_slots = slot_count;
	inventory_slots.resize(max_slots, -1);
}

void Inventory::init_inventory(int slot_count)
{
	max_slots = slot_count;
	inventory_slots.assign(max_slots, -1);
}

void Inventory::set_item_at(int index, int item)
{
	if (index >= 0 && index < max_slots)
	{
		inventory_slots[index] = item;
	}
}

bool Inventory::add_item(int item_id)
{
	// Find first empty slot
	for (int i = 0; i < max_slots; i++)
	{
		if (inventory_slots[i] == -1)
		{
			inventory_slots[i] = item_id;
			Logger::info(Logger::ITEMS, "added item " + std::to_string(item_id)
				+ " to inventory slot " + std::to_string(i));
			return true;
		}
	}

	Logger::warn(Logger::ITEMS, "failed to add item " + std::to_string(item_id)
		+ " — inventory full (" + std::to_string(max_slots) + " slots)");
	return false;
}

int Inventory::remove_item(int slot)
{
	if (slot < 0 || slot >= max_slots)
	{
		Logger::warn(Logger::ITEMS, "attempted to remove item from invalid slot "
			+ std::to_string(slot));
		return -1;
	}

	int item_id = inventory_slots[slot];
	if (item_id != -1)
	{
		inventory_slots[slot] = -1;
		Logger::info(Logger::ITEMS, "removed item " + std::to_string(item_id)
			+ " from inventory slot " + std::to_string(slot));
	}

	return item_id;
}

int Inventory::get_item(int slot) const
{
	if (slot < 0 || slot >= max_slots)
	{
		return -1;
	}
	return inventory_slots[slot];
}

int Inventory::get_first_item() const
{
	for (int i = 0; i < max_slots; i++)
	{
		if (inventory_slots[i] != -1)
		{
			return inventory_slots[i];
		}
	}
	return -1;
}

int Inventory::get_first_item_slot() const
{
	for (int i = 0; i < max_slots; i++)
	{
		if (inventory_slots[i] != -1)
		{
			return i;
		}
	}
	return -1;
}

int Inventory::remove_first_item()
{
	int slot = get_first_item_slot();
	if (slot != -1)
	{
		return remove_item(slot);
	}
	return -1;
}

bool Inventory::has_item() const
{
	return get_first_item() != -1;
}

bool Inventory::is_full() const
{
	for (int i = 0; i < max_slots; i++)
	{
		if (inventory_slots[i] == -1)
		{
			return false;
		}
	}
	return true;
}

int Inventory::get_item_count() const
{
	int count = 0;
	for (int i = 0; i < max_slots; i++)
	{
		if (inventory_slots[i] != -1)
		{
			count++;
		}
	}
	return count;
}

void Inventory::clear()
{
	for (int i = 0; i < max_slots; i++)
	{
		if (inventory_slots[i] != -1)
		{
			Logger::debug(Logger::ITEMS, "cleared inventory slot " + std::to_string(i)
				+ " (was item " + std::to_string(inventory_slots[i]) + ")");
			inventory_slots[i] = -1;
		}
	}
}
