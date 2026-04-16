#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "Body.h"
#include "Item.h"
#include "Tile.h"
#include "Level_Structure.h"
#include "Door.h"

class Map : public Level_Structure
{
private:

	enum Direction
	{
		RIGHT = 3,
		LEFT = 4,
		UP = 5,
		DOWN = 6,
		ERROR_DIRECTION = 20
	};

	int map_id;

	int map_generation_try;

	Door door_array[4];  // indexed by side: 0=N, 1=E, 2=S, 3=W

	int connection_mask; // bitmask of connected sides: bit0=N, bit1=E, bit2=S, bit3=W

	int item_id;
	int* item_on_map;
	double time;

	int width;
	int height;
	
	int rec_iter;

	std::vector<int> directions;
	std::vector<std::vector <std::vector<std::vector<int>>>>hyper_map;

	std::pair<int, int> data[6][9];
	
	//types of generation
	void generate_maze(bool item_generation, bool entity_generation);
	void generate_garden(bool item_generations, bool entity_generation);
	void generate_cage(bool item_generation, bool entity_generation);

	// helper functions for generation
	void generate_door(int side, int type_generation);
	void generate_doors(int mask, int type_generation);
	void carve_path(std::vector<std::vector<int>>& data, int x1, int y1, int x2, int y2);

	//functions for generation
	int rec_pos(int x, int y, std::vector<std::vector <int>>& arg, int& prev_direction);
	void build_frame(std::vector<std::vector <int>>& map_data, int wall, int space);
	void place_doors(std::vector<std::vector<int>>& data, Door* door_array);
	void print_vector(const std::vector<std::vector<int>>& arg, const int& size_x, const int& size_y);
	void print_doors();
	void trim_boarder(std::vector<std::vector <int>>& data, std::vector<std::vector <int>>& map_data);
	//void set_corners(std::vector<std::vector <int>>& map_data);
	void save_data(const std::vector<std::vector <int>>& map_data, const std::vector<std::vector <int>>& item_data);
	
	void set_entity_to_map(std::vector<std::vector <int>>& map_data, std::vector<std::vector <int>>& entity_data, int height, int width, int propability);
	void set_items_to_map(std::vector<std::vector <int>>& map_data, std::vector<std::vector <int>>& item_data, int height, int width, int propability);

	int get_tile(int x, int y);

public:
	Map();
	~Map();

	void Update(float delta);
	void Draw(SDL_Renderer* renderTarget);

	int test_image();

	void set_type(int type);
	void set_textures();
	void set_ptr(int* ptr);
	void set_map_id(int numer);
	void set_layout(int mask);


	int get_map_id();
	int get_hight();
	int get_width();
	Door get_door(int index);
};