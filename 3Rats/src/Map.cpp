#include "Map.h"

Map::Map()
{
    rec_iter = 0;
    width = 9;
    height = 6;

    item_id = 0;

    map_generation_try = 0;


    //if (test_image()) std::cout<<"IMAGE ERROR"<<std::endl;
    //else std::cout<<"no image error"<<std::endl;

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

    bool item_generatio = true;

    switch (type)
    {

    case MAZE_TYPE:
        std::cout << "===========================================" << std::endl;
        std::cout << "generating maze..." << std::endl;
        std::cout << "items generation: " << item_generatio << std::endl;
        generate_maze(true, false);
        break;

    case GARDEN_TYPE:
        std::cout << "===========================================" << std::endl;
        std::cout << "generating garden..." << std::endl;
        std::cout << "items generation: " << item_generatio << std::endl;
        generate_garden(true, true);
        break;
    case CAGE_TYPE:
        std::cout << "===========================================" << std::endl;
        std::cout << "generating cage..." << std::endl;
        std::cout << "items generation: " << item_generatio << std::endl;
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

            switch (data[h][w].first)
            {
                //maybe i should define some kind of object that
                // so that you could use this as a copy for the others 
                // less code more readable

                // make that next!

            case 0: //end_door
                inspected_tile.set_texture("../maze_textures/maze_door.png");
                inspected_tile.is_exit = true;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(0);
                std::cout<<1<<std::endl;
                break;

            case 1: //wall

                inspected_tile.set_texture("../maze_textures/maze_wall.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(1);

                //inspected_tile = wall;
                std::cout<<1<<std::endl;

                break;

            case 2: //start_door
                inspected_tile.set_texture("../maze_textures/maze_door.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = true;
                inspected_tile.set_hight(0);
                std::cout<<1<<std::endl;
                break;

            case 3: //right (horizontal)
            case 4: //left (horizontal)
                inspected_tile.set_texture("../maze_textures/walk_way_shadow_horizontal.png");
                inspected_tile.is_exit = false;
                inspected_tile.is_hole = false;
                inspected_tile.is_entrance = false;
                inspected_tile.set_hight(0);
                std::cout<<1<<std::endl;
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
            else if (data[h][w].second == 1)
            {
                inspected_item.set_on_map(true);
                inspected_item.set_cords(x_cord, y_cord);
                inspected_item.set_texture("../item_textures/mushroom.png");
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

void Map::set_layout(std::string layout)
{
    if (layout == "N")
    {
        entry_direction = 3;
        exit_direction = 1;
    }
    else if (layout == "E")
    {
        entry_direction = 3;
        exit_direction = 1;
    }
    else if (layout == "S")
    {
        entry_direction = 0;
        exit_direction = 2;
    }
    else if (layout == "W")
    {
        entry_direction = 0;
        exit_direction = 2;
    }
    else
    {
        std::cout << "error! the value is: " << layout << std::endl;
    }
}

void Map::set_entity_to_map(std::vector<std::vector<int>>& map_data, std::vector<std::vector<int>>& entity_data, int height, int width, int probability)
{

}

void Map::set_items_to_map(std::vector<std::vector<int>>& map_data, std::vector<std::vector<int>>& item_data, int height, int width, int probability)
{
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
                item_data[i][j] = 1;
                (*item_on_map)++;
            }
        }
    }
}

int Map::get_map_id() { return map_id; }

Door Map::get_door(int index) { return door_array[index]; }

int Map::get_hight() { return height; }

int Map::get_width() { return width; }


void Map::generate_maze(bool item_generation, bool entity_generation)
{
    int start_x = 1;
    int start_y = 1;

    int end_x = width;
    int end_y = height;

    generate_doors(entry_direction, exit_direction, 0);

    print_doors();

    std::vector<std::vector <int>> data(height + 2, std::vector<int>(width + 2));
    std::vector<std::vector <int>> map_data(height, std::vector<int>(width));
    std::vector<std::vector <int>> item_data(height, std::vector<int>(width));

    build_frame(data, 9, 1);

    place_doors(data, door_array);
    
    while (rec_pos(door_array[0].get_x(), door_array[0].get_y(), data, data[start_x][start_y]) != 0)
    { 
        map_generation_try++;
    }
    
    trim_boarder(data, map_data);

    //set_corners(map_data);

    if (item_generation) set_items_to_map(map_data, item_data, height, width, 30); // 80 meaning 1/80  

    std::cout << "Tries to generate Map #" << map_id << " : " << map_generation_try << std::endl;
    std::cout << "saving data..." << std::endl;

    save_data(map_data, item_data);
}

void Map::generate_garden(bool item_generation, bool entity_generation)
{
    width = 9;
    height = 6;

    int start_x = 1;
    int start_y = 1;

    int end_x = width;
    int end_y = height;

    generate_doors(entry_direction, exit_direction, 1);

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

    std::cout << "Tries to generate Map #" << map_id << " : " << map_generation_try << std::endl;

    std::cout << "saving data..." << std::endl;

    save_data(map_data, item_data);
}

void Map::generate_cage(bool item_generation, bool entity_generation)
{
    width = 9;
    height = 6;

    int start_x = 1;
    int start_y = 1;

    int end_x = width;
    int end_y = height;

    generate_doors(entry_direction, exit_direction, 2);

    print_doors();
    
    std::pair<int, int> food_bowl = 
    { 
        random_ptr->roll_custom_dice(end_x), 
        random_ptr->roll_custom_dice(end_y) 
    };
    std::pair<int, int> bed = 
    { 
        random_ptr->roll_custom_dice(end_x), 
        random_ptr->roll_custom_dice(end_y) 
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

    std::cout << "Tries to generate Map #" << map_id << " : " << map_generation_try << std::endl;
    std::cout << "saving data..." << std::endl;

    save_data(map_data, item_data);
}

void Map::generate_door(int direction, int index, int type, bool active)
{
    //direction => 0 = north, 1 = east, 2 = south, 3 = west
    if (direction == 5)
    {
        if (active)
        {
            door_array[index].init_door
            (
                random_ptr->roll_custom_dice(width), 
                random_ptr->roll_custom_dice(height),
                type, 
                active
            );
        }
        else
        {
            door_array[index].init_door(-100, -100, type, active);
        }
    }
    else
    {
        switch (direction)
        {
        case 0:
            door_array[index].init_door(random_ptr->roll_custom_dice(9), 1, type, active);
            break;
        case 1:
            door_array[index].init_door(9, random_ptr->roll_custom_dice(6), type, active);
            break;
        case 2:
            door_array[index].init_door(random_ptr->roll_custom_dice(9), 6, type, active);
            break;
        case 3:
            door_array[index].init_door(1, random_ptr->roll_custom_dice(6), type, active);
            break;
        default:
            std::cout << "error" << std::endl;
            break;
        }
    }    
}

void Map::generate_doors(int entry_direction, int exit_direction, int type_generation)
{
    const int MAZE_TYPE = 0;
    const int GARDEN_TYPE = 1;
    const int CAGE_TYPE = 2;

    const int INVALID_DIRECTION = 5;

    const int DOOR_TYPE_AMOUNT = 3;
    const int DOOR_AMOUNT = 3;

    int direction[DOOR_AMOUNT] = 
    { 
        entry_direction, 
        exit_direction, 
        INVALID_DIRECTION 
    };

    bool active[DOOR_TYPE_AMOUNT][DOOR_AMOUNT] =
    {
        {true, true, false},    // maze
        {true, true, true},     // garden
        {false, false, true}    // cage
    };

    switch (type_generation)
    {
    case MAZE_TYPE:
        for (int i = 0; i < 3; i++) 
            generate_door(direction[i], i, i + 1, active[MAZE_TYPE][i]);
        break;
    case GARDEN_TYPE:
        for (int i = 0; i < 3; i++) 
            generate_door(direction[i], i, i + 1, active[GARDEN_TYPE][i]);
        break;
    case CAGE_TYPE:
        for (int i = 0; i< 3; i++)
            generate_door(direction[i], i, i + 1, active[CAGE_TYPE][i]);
        break;
    default:
        std::cout << "ERROR: generating doors!";
        break;
    }    
}

int Map::rec_pos(int x, int y, std::vector<std::vector <int>>& arg, int& prev_direction)
{
    rec_iter++;
    int direction = ERROR_DIRECTION;

    //================= set new location
    // make this one structure a new function maybe???

    switch (random_ptr->roll_custom_dice(4))
    {
    case 1:
        direction = RIGHT;
        x++;
        break;

    case 2:
        direction = LEFT;
        x--;
        break;

    case 3:
        direction = UP;
        y++;
        break;

    case 4:
        direction = DOWN;
        y--;
        break;

    default:
        direction = ERROR_DIRECTION;
        std::cout << "ERROR: Direction is wrong." << std::endl;
        break;
    }

    //================= try new location

    int point_value = arg[y][x];

    if (point_value == 0)                         // finish or path
    {
        return point_value;
    }
    else if (point_value == 1)                    // empty field == wall
    {
        arg[y][x] = direction;

        int vall = rec_pos(x, y, arg, direction);

        if (vall == 0)
        {
            directions.push_back(direction);
            return vall;
        }
        else if (vall == 1)
        {

            arg[y][x] = direction;
        }
        else
        {
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
    if (door_array[0].get_active())
        data[door_array[0].get_y()][door_array[0].get_x()] = 2;

    if (door_array[1].get_active())
        data[door_array[1].get_y()][door_array[1].get_x()] = 0;

    if (door_array[2].get_active())
        data[door_array[2].get_y()][door_array[2].get_x()] = 13;
}

void Map::print_vector(const std::vector<std::vector<int>>& arg, const int& size_x, const int& size_y)
{
    std::cout << "Vector: " << std::endl;

    for (const auto& row : arg) {
        for (const auto& element : row) {
            std::cout << element;
        }
        std::cout << '\n';
    }
}

void Map::print_doors()
{
    std::cout << "Entry: [" << door_array[0].get_x() << ";" << door_array[0].get_y() << "]" << std::endl;
    std::cout << "Exit: [" << door_array[1].get_x() << ";" << door_array[1].get_y() << "]" << std::endl;
    std::cout << "Hole: [" << door_array[2].get_x() << ";" << door_array[2].get_y() << "]" << std::endl;
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