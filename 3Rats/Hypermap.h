#pragma once

#include "Map.h"
#include "Clock.h"

class Hypermap
{
private:
	SDL_Renderer* renderTarget;

	const int WALL = 1;
	const int EMPTY = 0;

	const int NORTH = 5;
	const int EAST = 3;
	const int SOUTH = 6;
	const int WEST = 4;

	int height, width;

	std::vector<std::pair<std::pair<int, int >, std::pair<int, int >>> connections;

	double time;

	Map* map_array;
	int map_array_size;

	Item* item_array;	// implement to use instead of having maps do it
	int item_array_size;

	Tile* tile_array;	// this 2
	int tile_array_size;

	int number_items_on_map;
	int* ptr;

	std::vector<std::vector <std::pair<int, int>>> data;//(height + 2, std::vector<int>(width + 2));
	std::vector<std::vector <std::pair<int, int>>> map_data;//(std::pair<int, int>(height), std::vector<int>(width));

public:
	Hypermap();

	void set_map_array(Map* map, int size);
	Map* get_map_array();
	int get_map_size();

	void set_tile_array(Tile* tile, int tile_size);
	Tile* get_tile_array();
	int get_tile_size();

	void set_item_array(Item* item, int item_size);
	Item* get_item_array();
	int get_item_size();

	void set_up();

	void make_maze();
	void make_points(int a_x, int a_y, int b_x, int b_y);
	void build_frame(std::vector<std::vector <std::pair<int, int>>>& map_data, std::pair<int, int >entrance, std::pair<int, int >exit, int wall, int space);
	int find_empty_space(int x, int y, std::vector<std::vector <std::pair<int, int>>>& arg, int& prev_direction, int iterator);
	void trim_boarder(std::vector<std::vector <std::pair<int, int>>>& data, std::vector<std::vector <std::pair<int, int>>>& map_data);
	void print_vector(std::vector<std::vector <std::pair<int, int>>>& arg, int size_x, int size_y);
	void print_vector_hidden(std::vector<std::vector <std::pair<int, int>>>& arg, int size_x, int size_y);

	int get_layout(int num);

	void update(float delta);
	void draw(SDL_Renderer* renderTarget);

	void set_renderer(SDL_Renderer* renderTarget);

	Map* get_map();

	int counter_maps;

};