#include "Topography.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>

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

    // Visual topology map
    Logger::info(Logger::TOPOLOGY, "topology map  legend: [id]=room  -=E/W connection  |=N/S connection");
    Logger::info(Logger::TOPOLOGY, "");

    for (int y = 0; y < height; y++)
    {
        // Room row: [00]-[01] [02] ...
        std::string room_row = "  ";
        for (int x = 0; x < width; x++)
        {
            int id = y * width + x;
            std::ostringstream cell;
            cell << "[" << std::setw(2) << std::setfill('0') << id << "]";
            room_row += cell.str();

            if (x < width - 1)
                room_row += (room_connections[y][x] & (1 << 1)) ? "-" : " ";  // E bit
        }
        Logger::info(Logger::TOPOLOGY, room_row);

        // Connector row: vertical connections to next row
        if (y < height - 1)
        {
            std::string conn_row = "  ";
            for (int x = 0; x < width; x++)
            {
                conn_row += (room_connections[y][x] & (1 << 2)) ? " |  " : "    ";  // S bit
                if (x < width - 1) conn_row += " ";
            }
            Logger::info(Logger::TOPOLOGY, conn_row);
        }
    }
    Logger::info(Logger::TOPOLOGY, "");
}

void Topography::log_world_map()
{
    // Each room cell: 6 cols wide (5 interior + shared right border), 3 rows tall (2 interior + shared bottom border)
    // Full grid: width*6+1 cols, height*3+1 rows
    int gw = width  * 6 + 1;
    int gh = height * 3 + 1;

    std::vector<std::string> g(gh, std::string(gw, ' '));

    // ── corners ──────────────────────────────────────────────────────────────
    for (int ry = 0; ry <= height; ry++)
        for (int rx = 0; rx <= width; rx++)
            g[ry * 3][rx * 6] = '+';

    for (int ry = 0; ry < height; ry++)
    {
        for (int rx = 0; rx < width; rx++)
        {
            // ── top horizontal wall ──────────────────────────────────────────
            // Open (gap) if the room above connects south to this room
            bool n_open = (ry > 0) && (room_connections[ry-1][rx] & (1 << 2));
            for (int c = 1; c <= 5; c++)
                g[ry*3][rx*6+c] = (n_open && c >= 2 && c <= 4) ? ' ' : '-';

            // ── left vertical wall ───────────────────────────────────────────
            // Open if the room to the left connects east to this room
            bool w_open = (rx > 0) && (room_connections[ry][rx-1] & (1 << 1));
            for (int r = 1; r <= 2; r++)
                g[ry*3+r][rx*6] = w_open ? ' ' : '|';

            // ── room interior: id + door hints ───────────────────────────────
            int   id   = ry * width + rx;
            int   mask = room_connections[ry][rx];
            std::ostringstream ss;
            ss << std::setw(2) << std::setfill('0') << id;
            // centre of interior is at (ry*3+1, rx*6+2)
            g[ry*3+1][rx*6+2] = ss.str()[0];
            g[ry*3+1][rx*6+3] = ss.str()[1];

            // small door arrows inside the room
            if (mask & (1<<0)) g[ry*3+1][rx*6+1] = '^';   // N
            if (mask & (1<<1)) g[ry*3+1][rx*6+5] = '>';   // E
            if (mask & (1<<2)) g[ry*3+2][rx*6+3] = 'v';   // S
            if (mask & (1<<3)) g[ry*3+1][rx*6+1] = '<';   // W (overwrites N if both, rare)
        }

        // ── rightmost vertical wall ──────────────────────────────────────────
        for (int r = 1; r <= 2; r++)
            g[ry*3+r][width*6] = '|';
    }

    // ── bottom wall of last room row ─────────────────────────────────────────
    for (int rx = 0; rx < width; rx++)
        for (int c = 1; c <= 5; c++)
            g[height*3][rx*6+c] = '-';

    Logger::info(Logger::TOPOLOGY, "world map  legend: ## room id  ^ v < > door directions");
    Logger::info(Logger::TOPOLOGY, "");
    for (const auto& row : g)
        Logger::info(Logger::TOPOLOGY, "  " + row);
    Logger::info(Logger::TOPOLOGY, "");

    log_world_map();
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
