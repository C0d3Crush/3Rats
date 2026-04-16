#include "Topography.h"
#include "Logger.h"

// N=0, E=1, S=2, W=3
static const int DIR_DX[4] = { 0,  1,  0, -1 };
static const int DIR_DY[4] = {-1,  0,  1,  0 };
static const int OPPOSITE[4] = { 2, 3, 0, 1 };

Topography::Topography()
{
    width             = 5;
    height            = 5;
    current_map_id    = 0;
    arrival_side      = 0;
    number_items_on_map = 0;
    ptr               = nullptr;
    map_array         = nullptr;
    map_array_size    = 0;
    renderTarget      = nullptr;

    for (int y = 0; y < 5; y++)
        for (int x = 0; x < 5; x++)
        {
            room_connections[y][x] = 0;
            visited[y][x]          = false;
        }
}

void Topography::carve_passages(int x, int y)
{
    visited[y][x] = true;

    // Fisher-Yates shuffle of 4 directions
    int dirs[4] = {0, 1, 2, 3};
    for (int i = 3; i > 0; i--)
    {
        int j = random_ptr->roll_custom_dice(i + 1) - 1;
        std::swap(dirs[i], dirs[j]);
    }

    for (int k = 0; k < 4; k++)
    {
        int d  = dirs[k];
        int nx = x + DIR_DX[d];
        int ny = y + DIR_DY[d];

        if (nx >= 0 && nx < width && ny >= 0 && ny < height && !visited[ny][nx])
        {
            room_connections[y][x]   |= (1 << d);
            room_connections[ny][nx] |= (1 << OPPOSITE[d]);
            carve_passages(nx, ny);
        }
    }
}

void Topography::generate_connections()
{
    Logger::info(Logger::TOPOLOGY, "generating room connections (" +
        std::to_string(width) + "x" + std::to_string(height) + " grid)");

    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            room_connections[y][x] = 0;
            visited[y][x]          = false;
        }

    carve_passages(0, 0);

    // Log the connection grid (N=1,E=2,S=4,W=8)
    Logger::debug(Logger::TOPOLOGY, "connection grid (bitmask N=1 E=2 S=4 W=8):");
    for (int y = 0; y < height; y++)
    {
        std::string row;
        for (int x = 0; x < width; x++)
            row += std::to_string(room_connections[y][x]) + " ";
        Logger::debug(Logger::TOPOLOGY, "  " + row);
    }

    // Log per-room door summary
    for (int y = 0; y < height; y++)
        for (int x = 0; x < width; x++)
        {
            int id   = y * width + x;
            int mask = room_connections[y][x];
            std::string doors;
            if (mask & 1) doors += "N";
            if (mask & 2) doors += "E";
            if (mask & 4) doors += "S";
            if (mask & 8) doors += "W";
            Logger::info(Logger::TOPOLOGY, "  room " + std::to_string(id)
                + " [" + std::to_string(x) + "," + std::to_string(y) + "]"
                + " doors=" + doors);
        }
}

int Topography::get_layout(int room_id)
{
    int x = room_id % width;
    int y = room_id / width;
    return room_connections[y][x];
}

int Topography::get_neighbor(int room_id, int direction)
{
    int x = room_id % width;
    int y = room_id / width;

    if (!(room_connections[y][x] & (1 << direction))) return -1;

    int nx = x + DIR_DX[direction];
    int ny = y + DIR_DY[direction];

    return ny * width + nx;
}

void Topography::set_map_array(Map* map, int map_size)
{
    map_array      = map;
    map_array_size = map_size;
}

Map* Topography::get_map_array() { return map_array; }

int Topography::get_map_size() { return map_array_size; }

int  Topography::get_current_map_id()        { return current_map_id; }
void Topography::set_current_map_id(int n)   { current_map_id = n; }

int  Topography::get_arrival_side()          { return arrival_side; }
void Topography::set_arrival_side(int side)  { arrival_side = side; }

void Topography::set_up()
{
    ptr = &number_items_on_map;
    for (int i = 0; i < map_array_size; i++)
        map_array[i].set_ptr(ptr);
}

void Topography::update(float delta) { }

void Topography::draw(SDL_Renderer* renderTarget)
{
    for (int i = 0; i < tile_array_size; i++)
        tile_array[i].draw(renderTarget);

    for (int i = 0; i < item_array_size; i++)
        item_array[i].draw(renderTarget);
}

void Topography::set_renderer(SDL_Renderer* r) { renderTarget = r; }

Map* Topography::get_map() { return map_array; }
