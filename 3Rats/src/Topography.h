#pragma once

#include "Map.h"
#include "Level_Structure.h"

class Topography : public Level_Structure
{
private:
    SDL_Renderer* renderTarget;

    int current_map_id;
    int arrival_side;   // side we arrived on in the current room (0=N,1=E,2=S,3=W)

    int height, width;  // topology grid dimensions

    int  room_connections[5][5];  // bitmask per cell: bit0=N, bit1=E, bit2=S, bit3=W
    bool visited[5][5];

    Map* map_array;
    int  map_array_size;

    int  number_items_on_map;
    int* ptr;

    void carve_passages(int x, int y);

public:
    Topography();

    void set_map_array(Map* map, int size);
    Map* get_map_array();
    int  get_map_size();

    int  get_current_map_id();
    void set_current_map_id(int number);

    int  get_arrival_side();
    void set_arrival_side(int side);

    void set_up();
    void generate_connections();
    void log_world_map();

    int  get_layout(int room_id);           // returns connection bitmask for room
    int  get_neighbor(int room_id, int direction);  // returns neighbor room id, or -1

    void update(float delta);
    void draw(SDL_Renderer* renderTarget);
    void set_renderer(SDL_Renderer* renderTarget);

    Map* get_map();
};
