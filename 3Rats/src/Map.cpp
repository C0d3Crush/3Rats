#include "Map.h"
#include "Logger.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iomanip>

Map::Map()
{
    rec_iter = 0;
    width = 9;
    height = 6;

    item_id = 0;
    connection_mask = 0;
    map_generation_try = 0;
}

Map::~Map()
{
}

void Map::Update(float delta)
{
    
}

void Map::Draw(SDL_Renderer* renderTarget)
{
}

int Map::test_image()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL_image
    int imgFlags = IMG_INIT_PNG; // or IMG_INIT_JPG, depending on the image format you want to support
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cout << "SDL_image initialization failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    return 0;
}

void Map::set_type(int type)
{
    const int MAZE_TYPE = 0;
    const int GARDEN_TYPE = 1;
    const int CAGE_TYPE = 2;

    map_type = type;

    bool item_generatio = true;

    switch (type)
    {

    case MAZE_TYPE:
        Logger::info(Logger::MAP, "map #" + std::to_string(map_id) + " — generating maze");
        generate_maze(true, false);
        break;

    case GARDEN_TYPE:
        Logger::info(Logger::MAP, "map #" + std::to_string(map_id) + " — generating garden");
        generate_garden(true, true);
        break;
    case CAGE_TYPE:
        Logger::info(Logger::MAP, "map #" + std::to_string(map_id) + " — generating cage");
        generate_cage(false, false);
        break;

    default:
        break;
    }
}

void Map::set_textures()
{
    /*Tile end_door(true, false, false, "maze_textures/maze_door.png", 0);
    Tile start_door(false, true, false, "maze_textures/maze_door.png", 0);
    Tile horizontal(false, false, false, "maze_textures/walk_way_shadow_horizontal.png", 0);
    Tile vertical(false, false, false, "maze_textures/walk_way_shadow_vertical.png", 0);
    Tile ground(false, false, false, "maze_textures/ground.png", 0);
    Tile maze_hole(false, false, true, "maze_textures/maze_hole.png", 0);
    */
    /*
    Tile wall;
    wall.set_surface(render_target, "meta_textures/place_holder.png");
    wall.set_cords(-100, -100);

    */

    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            Tile& inspected_tile = tile_array[get_tile(w, h)];
            Item& inspected_item = item_array[get_tile(w, h)];

            // Reset flags before each tile assignment
            inspected_tile.is_exit     = false;
            inspected_tile.is_entrance = false;
            inspected_tile.is_hole     = false;
            inspected_tile.door_side   = -1;

            switch (data[h][w].first)
            {
            case 20: // N door
            case 21: // E door
            case 22: // S door
            case 23: // W door
            {
                int s = data[h][w].first - 20;
                inspected_tile.set_texture("../maze_textures/maze_door.png");
                inspected_tile.is_exit   = true;
                inspected_tile.door_side = s;
                inspected_tile.set_hight(0);
                break;
            }

            case 0: //end_door (legacy)
                inspected_tile.set_texture("../maze_textures/maze_door.png");
                inspected_tile.is_exit = true;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(0);

                break;

            case 1: //wall

                inspected_tile.set_texture("../maze_textures/maze_wall.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(1);

                //inspected_tile = wall;


                break;

            case 2: //start_door
                inspected_tile.set_texture("../maze_textures/maze_door.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = true;
                inspected_tile.set_hight(0);

                break;

            case 3: //right (horizontal)
            case 4: //left (horizontal)
                inspected_tile.set_texture("../maze_textures/walk_way_shadow_horizontal.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(0);

                break;

            case 5: //up (vertical)
            case 6: //down (vertical)
                //inspected_tile.set_texture("maze_textures/ground.png");
                inspected_tile.set_texture("../maze_textures/walk_way_shadow_vertical.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(0);
                break;

            case 7: //left-up
                //inspected_tile.set_texture("maze_textures/walk_way_shadow_left_up.png");
                inspected_tile.set_texture("../maze_textures/ground.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(0);

                break;

            case 8: //right-up 
                //inspected_tile.set_texture("maze_textures/walk_way_shadow_right_down.png");
                inspected_tile.set_texture("../maze_textures/ground.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(0);

                break;

            case 9: //left-down
                //inspected_tile.set_texture("maze_textures/walk_way_shadow_left_down.png");
                inspected_tile.set_texture("../maze_textures/ground.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(0);

                break;

            case 10: //right-down
                inspected_tile.set_hight(0);
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_texture("../maze_textures/ground.png");
                //inspected_tile.set_texture("maze_textures/walk_way_shadow_right_down.png");

                break;

            case 11: //hard-wall 
                inspected_tile.set_hight(0);
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_texture("../maze_textures/ground.png");
                //inspected_tile.set_texture("maze_textures/maze_wall.png");

                break;

            case 12:
                inspected_tile.set_hight(0);
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_texture("../maze_textures/ground.png");
                //inspected_tile.set_texture("place_holder.png");

                break;
            case 13:

                inspected_tile.set_hight(0);
                inspected_tile.is_exit = false;
                inspected_tile.is_entrance = false;
                inspected_tile.is_hole = true;
                inspected_tile.set_texture("../maze_textures/maze_hole.png");
                break;
            case 14:
                //std::cout<<"drawn_woodn_floor"<<std::endl;
                inspected_tile.set_hight(0);
                inspected_tile.is_exit = false;
                inspected_tile.is_entrance = false;
                inspected_tile.is_hole = false;
                inspected_tile.set_texture("../maze_textures/wooden_floor.png");
                break;

            case 15:
                inspected_tile.set_hight(0);
                inspected_tile.is_exit = false;
                inspected_tile.is_entrance = false;
                inspected_tile.is_hole = false;
                inspected_tile.set_texture("../textures/interactable/test.png");
                break;

            case 16:

                inspected_tile.set_hight(0);
                inspected_tile.is_exit = false;
                inspected_tile.is_entrance = false;
                inspected_tile.is_hole = false;
                inspected_tile.set_texture("../textures/interactable/test.png");
                break;

            default:

                inspected_tile.set_hight(0);
                inspected_tile.is_exit = false;
                inspected_tile.is_entrance = false;
                inspected_tile.is_hole = false;
                inspected_tile.set_texture("../maze_textures/place_holder.png");
                break;
            }

            int x_cord = w * 64;
            int y_cord = h * 64;

            if (inspected_item.get_pick_up())
            {
                // item is carried by a rat — leave texture and position alone
            }
            else if (data[h][w].second >= 1 && data[h][w].second <= 3)
            {
                // Item present - configure based on type
                inspected_item.set_on_map(true);
                inspected_item.set_cords(x_cord, y_cord);
                inspected_item.set_consumable(true);

                int item_type = data[h][w].second;
                ItemEffect item_effect;
                std::string texture_path;

                switch (item_type) {
                    case 1:  // FOOD
                        item_effect = ItemEffect(ItemType::FOOD, 50.0f, 0.0f, false);
                        texture_path = "../item_textures/mushroom.png";
                        break;
                    case 2:  // SPEED_BOOST
                        item_effect = ItemEffect(ItemType::SPEED_BOOST, 100.0f, 5.0f, false);
                        texture_path = "../item_textures/mushroom.png";  // TODO: add speed_boost texture
                        break;
                    case 3:  // SHIELD
                        item_effect = ItemEffect(ItemType::SHIELD, 1.0f, 10.0f, false);
                        texture_path = "../item_textures/mushroom.png";  // TODO: add shield texture
                        break;
                    default:
                        item_effect = ItemEffect(ItemType::FOOD, 50.0f, 0.0f, false);
                        texture_path = "../item_textures/mushroom.png";
                        break;
                }

                inspected_item.set_effect(item_effect);
                inspected_item.set_texture(texture_path);
                item_id++;
            }
            else if (data[h][w].second == 0)
            {
                inspected_item.set_on_map(false);
                inspected_item.set_cords(-100, -100);
                inspected_item.set_texture("../item_textures/place_holder.png");
            }
        }
    }
}

void Map::set_ptr(int* ptr)
{
    item_on_map = ptr;
}

void Map::set_map_id(int numer) { map_id = numer; }

void Map::set_layout(int mask)
{
    connection_mask = mask;
}

void Map::set_entity_to_map(std::vector<std::vector<int>>& map_data, std::vector<std::vector<int>>& entity_data, int height, int width, int probability)
{

}

void Map::set_items_to_map(std::vector<std::vector<int>>& map_data, std::vector<std::vector<int>>& item_data, int height, int width, int probability)
{
    int food_count = 0, speed_count = 0, shield_count = 0;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (map_data[i][j] == 0 || map_data[i][j] == 1 || map_data[i][j] == 2)
            {
                item_data[i][j] = 0;
            }
            else if (*item_on_map < item_array_size && random_ptr->roll_custom_dice(probability) == 1)
            {
                // Weighted item type selection:
                // 50% FOOD (type 1), 30% SPEED_BOOST (type 2), 20% SHIELD (type 3)
                int roll = random_ptr->roll_custom_dice(10);  // 1-10

                if (roll <= 5) {
                    // 1-5: FOOD (type 1)
                    item_data[i][j] = 1;
                    food_count++;
                } else if (roll <= 8) {
                    // 6-8: SPEED_BOOST (type 2)
                    item_data[i][j] = 2;
                    speed_count++;
                } else {
                    // 9-10: SHIELD (type 3)
                    item_data[i][j] = 3;
                    shield_count++;
                }
                (*item_on_map)++;
            }
        }
    }

    // Log item distribution for this map
    if (food_count + speed_count + shield_count > 0) {
        Logger::debug(Logger::MAP, "map #" + std::to_string(map_id)
            + " item distribution: FOOD=" + std::to_string(food_count)
            + " SPEED=" + std::to_string(speed_count)
            + " SHIELD=" + std::to_string(shield_count));

        if (shield_count > 0) {
            Logger::info(Logger::ITEMS, "map #" + std::to_string(map_id)
                + " spawned " + std::to_string(shield_count) + " rare SHIELD item(s)");
        }
    }
}

int Map::get_map_id() { return map_id; }

Door Map::get_door(int index) { return door_array[index]; }

int Map::get_hight() { return height; }

int Map::get_width() { return width; }


void Map::generate_maze(bool item_generation, bool entity_generation)
{
    generate_doors(connection_mask, 0);
    print_doors();

    std::vector<std::vector<int>> data(height + 2, std::vector<int>(width + 2));
    std::vector<std::vector<int>> map_data(height, std::vector<int>(width));
    std::vector<std::vector<int>> item_data(height, std::vector<int>(width));

    build_frame(data, 9, 1);

    // All active doors start as value 0 (success target for rec_pos)
    for (int side = 0; side < 4; side++)
        if (door_array[side].get_active())
            data[door_array[side].get_y()][door_array[side].get_x()] = 0;

    // A 1-door (leaf) room has no second target for rec_pos to reach.
    // Place a phantom exit at the room centre so the walk has somewhere to go.
    // After the walk the centre becomes a regular carved tile.
    int cx = (width  + 1) / 2;
    int cy = (height + 1) / 2;
    bool phantom = (__builtin_popcount(connection_mask) == 1);
    if (phantom)
        data[cy][cx] = 0;

    // Walk from each door.
    // After each walk, carved tiles (3-6) are promoted to permanent markers
    // (100=horiz, 101=vert) so subsequent walks can connect quickly to the
    // existing network rather than hunting for a specific door tile.
    for (int side = 0; side < 4; side++)
    {
        if (!door_array[side].get_active()) continue;

        int dx = door_array[side].get_x();
        int dy = door_array[side].get_y();

        data[dy][dx] = 99;

        while (rec_pos(dx, dy, data, data[1][1]) != 0)
            map_generation_try++;

        data[dy][dx] = 0;  // restore so later walks can connect here

        // Promote this walk's tiles to permanent so walk N+1 can reach them
        for (int h = 1; h <= height; h++)
            for (int w = 1; w <= width; w++)
            {
                if (data[h][w] == 3 || data[h][w] == 4) data[h][w] = 100;
                else if (data[h][w] == 5 || data[h][w] == 6) data[h][w] = 101;
            }
    }

    // Restore permanent markers to direction values for texturing
    for (int h = 1; h <= height; h++)
        for (int w = 1; w <= width; w++)
        {
            if      (data[h][w] == 100) data[h][w] = 3;
            else if (data[h][w] == 101) data[h][w] = 5;
        }

    // Convert phantom centre to regular floor if still uncarved
    if (phantom && data[cy][cx] == 0)
        data[cy][cx] = 3;

    // Stamp proper directional door types (20-23) over the 0 markers
    place_doors(data, door_array);

    trim_boarder(data, map_data);
    if (item_generation) set_items_to_map(map_data, item_data, height, width, 30);

    Logger::info(Logger::MAP, "map #" + std::to_string(map_id)
        + " (maze) tries=" + std::to_string(map_generation_try)
        + " doors=" + std::to_string(__builtin_popcount(connection_mask)));
    save_data(map_data, item_data);
    log_ascii_map();
}

void Map::generate_garden(bool item_generation, bool entity_generation)
{
    generate_doors(connection_mask, 1);

    print_doors();

    std::vector<std::vector <int>> data(height + 2, std::vector<int>(width + 2));    //x11; 0    y8; 0 means back one node
    std::vector<std::vector <int>> map_data(height, std::vector<int>(width));
    std::vector<std::vector <int>> item_data(height, std::vector<int>(width));
    std::vector<std::vector <int>> entity_data(height, std::vector<int>(width));

    build_frame(data, 1, 12);

    place_doors(data, door_array);

    trim_boarder(data, map_data);

    if (item_generation) set_items_to_map(map_data, item_data, height, width, 10);  //20 meaning 1/20

    if (entity_generation) set_entity_to_map(map_data, entity_data, height, width, 70);

    Logger::info(Logger::MAP, "map #" + std::to_string(map_id)
        + " (garden) doors=" + std::to_string(__builtin_popcount(connection_mask)));
    save_data(map_data, item_data);
    log_ascii_map();
}

void Map::generate_cage(bool item_generation, bool entity_generation)
{
    generate_doors(connection_mask, 2);

    print_doors();
    
    std::pair<int, int> food_bowl =
    {
        random_ptr->roll_custom_dice(width),
        random_ptr->roll_custom_dice(height)
    };
    std::pair<int, int> bed =
    {
        random_ptr->roll_custom_dice(width),
        random_ptr->roll_custom_dice(height)
    };

    std::vector<std::vector <int>> data(height + 2, std::vector<int>(width + 2));    //x11; 0    y8; 0 means back one node
    std::vector<std::vector <int>> map_data(height, std::vector<int>(width));
    std::vector<std::vector <int>> item_data(height, std::vector<int>(width));

    // in this case it has to generate a hole. maybe not ?
    // maybe it generates a hole only if the player does an action?
    // for now for testing it is this

    build_frame(data, 1, 14);

    place_doors(data, door_array);

    data[food_bowl.second][food_bowl.first] = 15;
    data[bed.second][bed.first] = 16;


    trim_boarder(data, map_data);

    //if (item_generation) set_items_to_map(map_data, item_data, height, width, 70);  //20 meaning 1/20

    Logger::info(Logger::MAP, "map #" + std::to_string(map_id)
        + " (cage) doors=" + std::to_string(__builtin_popcount(connection_mask)));
    save_data(map_data, item_data);
    log_ascii_map();
}

void Map::generate_door(int side, int type_generation)
{
    // side: 0=N (top), 1=E (right), 2=S (bottom), 3=W (left)
    // coordinates are in data-space (1-indexed interior)
    int x, y;
    switch (side)
    {
    case 0: // N
        x = random_ptr->roll_custom_dice(width);
        y = 1;
        break;
    case 1: // E
        x = width;
        y = random_ptr->roll_custom_dice(height);
        break;
    case 2: // S
        x = random_ptr->roll_custom_dice(width);
        y = height;
        break;
    case 3: // W
        x = 1;
        y = random_ptr->roll_custom_dice(height);
        break;
    default:
        x = -100; y = -100;
        break;
    }
    door_array[side].init_door(x, y, type_generation + 1, true, side);
}

void Map::generate_doors(int mask, int type_generation)
{
    for (int side = 0; side < 4; side++)
    {
        if (mask & (1 << side))
            generate_door(side, type_generation);
        else
            door_array[side].init_door(-100, -100, 0, false, side);
    }
}

int Map::rec_pos(int x, int y, std::vector<std::vector<int>>& arg, int& prev_direction)
{
    rec_iter++;
    int direction = ERROR_DIRECTION;

    switch (random_ptr->roll_custom_dice(4))
    {
    case 1: direction = RIGHT; x++; break;
    case 2: direction = LEFT;  x--; break;
    case 3: direction = UP;    y++; break;
    case 4: direction = DOWN;  y--; break;
    default: Logger::error(Logger::MAP, "rec_pos: invalid direction roll"); break;
    }

    int point_value = arg[y][x];

    if (point_value == 0 || point_value == 100 || point_value == 101)
    {
        return 0;
    }
    else if (point_value == 1)
    {
        arg[y][x] = direction;

        int vall = rec_pos(x, y, arg, direction);

        if (vall == 0)
        {
            directions.push_back(direction);
            return 0;
        }
        else
        {
            // Dead end or border — un-carve and let the while loop retry
            arg[y][x] = 1;
            return vall;
        }
    }
    else
    {
        return point_value;
    }
}

void Map::build_frame(std::vector<std::vector<int>>& data, int wall, int space)
{
    for (int h = 0; h < height + 2; h++)
    {
        for (int w = 0; w < width + 2; w++)
        {
            if      (w == 0)            data[h][w] = wall;
            else if (w == width + 1)    data[h][w] = wall;
            else if (h == 0)            data[h][w] = wall;
            else if (h == height + 1)   data[h][w] = wall;
            else                        data[h][w] = space;
        }
    }
}

void Map::place_doors(std::vector<std::vector<int>>& data, Door* door_array)
{
    // Tile types 20-23 encode the door side (20=N, 21=E, 22=S, 23=W)
    for (int side = 0; side < 4; side++)
    {
        if (door_array[side].get_active())
            data[door_array[side].get_y()][door_array[side].get_x()] = 20 + side;
    }
}


void Map::print_vector(const std::vector<std::vector<int>>& arg, const int& size_x, const int& size_y)
{
    Logger::debug(Logger::MAP, "map #" + std::to_string(map_id) + " grid:");
    for (const auto& row : arg) {
        std::string line;
        for (const auto& element : row) line += std::to_string(element);
        Logger::debug(Logger::MAP, "  " + line);
    }
}

void Map::print_doors()
{
    static const char* names[4] = {"N", "E", "S", "W"};
    for (int side = 0; side < 4; side++)
    {
        if (door_array[side].get_active())
            Logger::info(Logger::MAP, "  map #" + std::to_string(map_id)
                + " door " + names[side] + " ["
                + std::to_string(door_array[side].get_x()) + ";"
                + std::to_string(door_array[side].get_y()) + "]");
    }
}

void Map::trim_boarder(std::vector<std::vector <int>>& data, std::vector<std::vector <int>>& map_data)   //trim boarder
{
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            map_data[h][w] = data[h + 1][w + 1];
        }
    }
}
/*
void Map::set_corners(std::vector<std::vector<int>>& map_data)
{
    //make something smart here
}
*/

void Map::save_data(const std::vector<std::vector<int>>& map_data, const std::vector<std::vector<int>>& item_data)
{
    int i = 0;
    for (auto& row : data) {
        int j = 0;
        for (auto& cell : row) {
            cell.first = map_data[i][j];
            cell.second = item_data[i][j];
            j++;
        }
        i++;
    }
}

int Map::get_tile(int x, int y) { return y * width + x; }

void Map::log_ascii_map()
{
    // Map tile types to readable single characters
    auto tile_char = [](int type) -> char {
        switch (type) {
        case  0: return 'X';   // legacy exit door
        case  1: return '#';   // wall
        case  2: return 'Y';   // legacy entry door
        case  3: return '-';   // horizontal walkway (right)
        case  4: return '-';   // horizontal walkway (left)
        case  5: return '|';   // vertical walkway (up)
        case  6: return '|';   // vertical walkway (down)
        case  7: case 8: case 9: case 10: return '+';  // corners / junctions
        case 11: return '#';   // hard wall
        case 12: return ' ';   // garden open floor
        case 13: return 'O';   // hole
        case 14: return '_';   // cage wooden floor
        case 15: return 'F';   // food bowl
        case 16: return 'B';   // bed
        case 20: return 'N';   // north door
        case 21: return 'E';   // east door
        case 22: return 'S';   // south door
        case 23: return 'W';   // west door
        default: return '?';
        }
    };

    // Per-map file: logs/generation/maps/map_XX.log
    namespace fs = std::filesystem;
    fs::path dir = fs::path("../logs/generation/maps");
    fs::create_directories(dir);

    std::ostringstream id_str;
    id_str << std::setw(2) << std::setfill('0') << map_id;
    std::ofstream f(dir / ("map_" + id_str.str() + ".log"));

    // Collect door info
    static const char* side_names[4] = {"N", "E", "S", "W"};
    std::string door_list;
    for (int s = 0; s < 4; s++)
        if (door_array[s].get_active())
            door_list += std::string(side_names[s])
                + "[" + std::to_string(door_array[s].get_x())
                + ";" + std::to_string(door_array[s].get_y()) + "] ";

    f << "map #" << map_id
      << "  doors=" << __builtin_popcount(connection_mask)
      << "  " << door_list << "\n"
      << "legend: # wall  - | path  N/E/S/W door  * item  O hole\n\n";

    std::string border(width + 2, '#');
    f << border << "\n";
    for (int h = 0; h < height; h++)
    {
        std::string row = "#";
        for (int w = 0; w < width; w++)
        {
            int tile = data[h][w].first;
            int item = data[h][w].second;
            row += (item == 1) ? '*' : tile_char(tile);
        }
        f << row << "#\n";
    }
    f << border << "\n";

    // Summary line in the shared map.log
    Logger::info(Logger::MAP, "map #" + std::to_string(map_id)
        + " written to maps/map_" + id_str.str() + ".log"
        + "  doors=" + door_list);
}