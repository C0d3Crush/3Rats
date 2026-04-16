#pragma once

#include "Map.h"
#include "Level_Structure.h"

class Topography : public Level_Structure
{
private:
	SDL_Renderer* renderTarget;

	int current_map_id;

	const std::string EMPTY = ".";
	const std::string WALL = "#";
	const std::string FINISH = "F";
	const std::string START = "S";

	const std::string NORTH = "N";
	const std::string EAST = "E";
	const std::string SOUTH = "S";
	const std::string WEST = "W";	
	
	int height, width;

	std::vector<std::pair<std::pair<int, int >, std::pair<int, int >>> connections;

	double time;

	Map* map_array;
	int map_array_size;

	int number_items_on_map;
	int* ptr;

	std::vector<std::vector <std::pair<std::string, int>>> data;//(height + 2, std::vector<int>(width + 2));
	std::vector<std::vector <std::pair<std::string, int>>> map_data;//(std::pair<int, int>(height), std::vector<int>(width));

public:
	Topography();

	void set_map_array(Map* map, int size);
	Map* get_map_array();
	int get_map_size();

	int get_current_map_id();
	void set_current_map_id(int number);

	void set_up();

	void make_maze();
	void make_points(int a_x, int a_y, int b_x, int b_y);
	void build_frame(std::vector<std::vector <std::pair<std::string, int>>>& map_data, std::pair<int, int >entrance, std::pair<int, int >exit);
	std::string find_empty_space(int x, int y, std::vector<std::vector <std::pair<std::string, int>>>& arg, std::string& prev_direction, int iterator);
	void trim_boarder(std::vector<std::vector <std::pair<std::string, int>>>& data, std::vector<std::vector <std::pair<std::string, int>>>& map_data);
	void print_vector(std::vector<std::vector <std::pair<std::string, int>>>& arg, int size_x, int size_y);
	void print_vector_hidden(std::vector<std::vector <std::pair<std::string, int>>>& arg, int size_x, int size_y);

	void print_connections(std::vector<std::pair<std::pair<int, int >, std::pair<int, int >>>connections);

	std::string get_layout(int num);

	void update(float delta);
	void draw(SDL_Renderer* renderTarget);

	void set_renderer(SDL_Renderer* renderTarget);

	Map* get_map();

	int counter_maps;

};