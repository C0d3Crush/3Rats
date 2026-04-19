#include "Controller.h"
#include "Logger.h"

void Controller::calculate_blocked_side(block_direction_counter& counter, std::vector<std::vector<bool>> blocked_i, int length)
{
	for (int i = 0; i < length; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			if (blocked_i[i][k])
			{
				if (k == 0) counter.right++;
				else if (k == 1) counter.left++;
				else if (k == 2) counter.down++;
				else if (k == 3) counter.up++;
				else {}
			}
			else
			{
				if (k == 0) counter.right--;
				else if (k == 1) counter.left--;
				else if (k == 2) counter.down--;
				else if (k == 3) counter.up--;
				else {}
			}
		}
	}
}

void Controller::get_direction_blocked(block_direction_counter& counter, block_direction& direction, int length)
{
	if (counter.right + length == 0) direction.right = false;
	else direction.right = true;

	if (counter.left + length == 0) direction.left = false;
	else direction.left = true;

	if (counter.down + length == 0) direction.down = false;
	else direction.down = true;

	if (counter.up + length == 0) direction.up = false;
	else direction.up = true;
}

void Controller::Update(float delta, const Uint8* keyState, int mode, Controller& p1)
{

}

std::pair<int, int> Controller::direction_to_offset(int direction)
{
	std::pair<int, int> pos = { 0, 0 };

	switch (direction) {
	case 0:
		pos.second -= 55;
		break;
	case 1:
		pos.second += 55;
		break;
	case 2:
		pos.first -= 55;
		break;
	case 3:
		pos.first += 55;
		break;
	default:
		break;
	}

	return pos;
}
void Controller::init_colision_map(std::vector<std::vector<bool>>& map)
{
	// initialize the outer vector with tile_array_size rows
	map.resize(tile_array_size);

	// initialize each inner vector with 4 columns and set all values to false
	for (int i = 0; i < tile_array_size; ++i) {
		map[i].resize(4, false);
	}

}
void Controller::set_controller_number(int number)
{
	controller_number = number;
}

void Controller::set_Topography(Topography* h)
{
	topography = h;
}

void Controller::set_random_pointer(Random& random)
{
	random_ptr = &random;
}


void Controller::SetNewGoal(int x, int y) { goalX = x; goalY = y; }

int Controller::GetDirection() { return current_direction; }

bool Controller::is_item_available_on_map()
{
	bool available = false;

	for (int i = 0; i < item_array_size; i++)
	{
		if (!item_array[i].get_pick_up())
		{
			if (item_array[i].get_on_map())
			{
				available = true;
			}
		}
	}
	return available;
}

void Controller::make_goal()
{
	int random_item_number = 0;

	while (!has_goal)
	{
		if (item_array[random_item_number].get_on_map() && random_ptr->flip_coin())
		{
			has_goal = true;

			SetNewGoal
			(
				item_array[random_item_number].get_origin_x(),
				item_array[random_item_number].get_origin_y()
			);

			item_search_id = random_item_number;

			Logger::debug(Logger::ACTEUR, "rat " + std::to_string(controller_number)
			    + " new goal: item " + std::to_string(item_search_id)
			    + " at (" + std::to_string(goalX) + "," + std::to_string(goalY) + ")");

		}

		random_item_number++;

		if (random_item_number == item_array_size - 1) random_item_number = 0;
	}
}

void Controller::place_item()
{
	if (!holds_item || !inventory.has_item())
	{
		return;
	}

	// Remove first item from inventory (placed on ground)
	int item_id = inventory.remove_first_item();

	if (item_id != -1)
	{
		Logger::info(Logger::ITEMS, "rat " + std::to_string(controller_number)
			+ " placed item " + std::to_string(item_id));

		// Update holds_item flag based on remaining inventory
		holds_item = inventory.has_item();
		item_type = holds_item ? 1 : 0;  // Legacy compatibility
	}
}

void Controller::set_has_goal(bool value) { has_goal = value; }

void Controller::set_enter(bool value) { wants_enter_door = value; }

