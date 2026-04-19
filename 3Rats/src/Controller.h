#pragma once
#include <SDL2/SDL.h>

#include <vector>

#include "Topography.h"
#include "Random.h"
#include "Inventory.h"

class Controller {
protected:

	struct block_direction_counter {
		int right;
		int left;
		int up;
		int down;
	};

	struct block_direction {
		bool right;
		bool left;
		bool up;
		bool down;
	};

	struct controller_move
	{
		bool up;
		bool down;
		bool left;
		bool right;
	};


	Map* map_array;
	int map_array_size;
	int* map_number;

	Item* item_array;
	int item_array_size;

	Tile* tile_array;
	int tile_array_size;

	Topography* topography;

	int controller_number;

	int goalX, goalY;
	Random* random_ptr;
	int current_direction;
	bool has_goal;
	int item_search_id;
	int item_type;  // DEPRECATED: use inventory instead
	bool holds_item;
	bool wants_enter_door;

	Inventory inventory;



	void calculate_blocked_side(block_direction_counter& counter, std::vector<std::vector<bool>> blocked_i, int length);
	void get_direction_blocked(block_direction_counter& counter, block_direction& direction, int length);
	void Update(float delta, const Uint8* keyState, int mode, Controller& p1);
	std::pair<int, int> direction_to_offset(int direction);
	void init_colision_map(std::vector<std::vector<bool>>& map);

public:
	void set_controller_number(int number);
	void set_Topography(Topography* topography);
	void set_random_pointer(Random& random);

	void set_has_goal(bool value);	// this is for debug 
	void set_enter(bool value);
	void place_item();		//set_item
	void SetNewGoal(int x, int y);
	int GetDirection();
	bool is_item_available_on_map();		//get available item on map
	void make_goal();		// get new goal

};