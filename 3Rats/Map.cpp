#include "Map.h"

Map::Map()
{
    rec_iter = 0;
    width = 9;
    height = 6;
}

Map::Map(const Map& b)
{
    rec_iter = 0;
    width = 9;
    height = 6;
    body_amount = 9 * 6;
}

Map::Map(Tile arg[], int size, int w, int h, int type)
{
    rec_iter = 0;
	width = w;
	height = h;

	tile_array = arg;
	body_amount = size;

	int x_cord = 0;
	int y_cord = 0;

	for (int i = 0; i < body_amount; i++)
	{
		tile_array[i].set_cords(x_cord, y_cord);
		//tile_array[i].set_texture("maze_wall.png");
		x_cord += 64;

		if (x_cord >= 576)
		{
			y_cord += 64;
			x_cord = 0;
		}
	}

    switch (type)
    {

    case 0:
        make_maze();
        break;

    case 1:
        make_garden();
        break;

    default:
        break;
    }
}

Map::~Map()
{
}

void Map::Update(float delta)
{
    time += delta;
    std::cout << (int)time << std::endl;
}

void Map::Draw(SDL_Renderer* renderTarget)
{
    for (int i = 0; i < body_amount; i++)
    {
        tile_array[i].Draw(renderTarget);
    }

    for (int i = 0; i < item_amount; i++)
    {
        item_array[i].Draw(renderTarget);
    }
}


void Map::make_maze()
{
    width = 9;
    height = 6;

    int start_x = 1;
    int start_y = 1;

    int end_x = width;
    int end_y = height;

    std::vector<std::vector <int>> data(height + 2, std::vector<int>(width + 2));    //x11; 0    y8; 0 means back one node
    std::vector<std::vector <int>> map_data(height, std::vector<int>(width));

    build_frame(data, start_x, start_y, height, width);

    data[start_y][start_x] = 2;
    data[end_y][end_x] = 0;

    //print_vector(data, width + 2, height + 2);

    int val;

    while (1)
    {
        val = rec_pos(start_x, start_y, data, data[start_x][start_y]);

        if (val != 0)   // rec_pos hit path!
        {
        }
        else if (val == 0)  // found exit!
        {
            //std::cout << "found exit!" << std::endl;
            break;
        }
        else
        {
            //std::cout << "ERROR: value not allowed: " << val << std::endl;
            break;
        }
    }

    //print_vector(data, width + 2, height + 2);

    trim_boarder(data, map_data);

    //print_vector(map_data, map_data[0].size(), map_data.size());

    /*
    std::cout << "directions vector:" << std::endl;

    for (int i = directions.size() - 1; i >= 0; i--)
    {
        std::cout << directions[i];
    }
    std::cout << std::endl;
    */

    set_corners(map_data);

    /*
    std::cout << "directions vector + corners:" << std::endl;

    for (int i = directions.size() - 1; i >= 0; i--)
    {
        std::cout << directions[i];
    }
    std::cout << std::endl;
    */

    //set_textures(map_data);
    save_data(map_data);
}

void Map::make_garden()
{
    width = 9;
    height = 6;

    int start_x = 1;
    int start_y = 1;

    int end_x = width;
    int end_y = height;

    std::vector<std::vector <int>> data(height + 2, std::vector<int>(width + 2));    //x11; 0    y8; 0 means back one node
    std::vector<std::vector <int>> map_data(height, std::vector<int>(width));

    for (int h = 0; h < height + 2; h++)
    {
        for (int w = 0; w < width + 2; w++)
        {
            if (w == 0 || w == width + 1 || h == 0 || h == height + 1) data[h][w] = 1;
            else data[h][w] = 3;

        }
    }

    data[start_y][start_x] = 2;
    data[end_y][end_x] = 0;

    //print_vector(data, width + 2, height + 2);

    trim_boarder(data, map_data);

    save_data(map_data);
}


void Map::set_item_array(Item* item, int size)
{
    item_amount = size;
    item_array = item;

    int x_cord = 0;
    int y_cord = 0;

    for (int i = 0; i < body_amount; i++)
    {
        tile_array[i].set_cords(x_cord, y_cord);
        x_cord += 64;

        if (x_cord >= 576)
        {
            y_cord += 64;
            x_cord = 0;
        }
    }
}

void Map::set_tile_array(Tile* body, int size)
{
    body_amount = size;
    tile_array = body;

    int x_cord = 0;
    int y_cord = 0;

    for (int i = 0; i < body_amount; i++)
    {
        tile_array[i].set_cords(x_cord, y_cord);
        x_cord += 64;

        if (x_cord >= 576)
        {
            y_cord += 64;
            x_cord = 0;
        }
    }
}

void Map::set_type(int type)
{
    switch (type)
    {

    case 0:
        make_maze();
        break;

    case 1:
        make_garden();
        break;

    default:
        break;
    }
}

void Map::set_textures()
{
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            switch (data[h][w])
            {
            case 0: //end_door
                tile_array[get_tile(w, h)].set_texture("maze_textures/maze_door.png");
                tile_array[get_tile(w, h)].is_exit = true;
                tile_array[get_tile(w, h)].set_hight(0);
                break;

            case 1: //wall
                tile_array[get_tile(w, h)].set_texture("maze_textures/maze_wall.png");
                tile_array[get_tile(w, h)].set_hight(1);
                break;

            case 2: //start_door
                tile_array[get_tile(w, h)].set_texture("maze_textures/maze_door.png");
                tile_array[get_tile(w, h)].is_entrance = true;
                tile_array[get_tile(w, h)].set_hight(0);
                break;

            case 3: //right (horizontal)
            case 4: //left (horizontal)
                tile_array[get_tile(w, h)].set_texture("maze_textures/walk_way_shadow_horizontal.png");
                tile_array[get_tile(w, h)].set_hight(0);
                break;

            case 5: //up (vertical)
            case 6: //down (vertical)
                tile_array[get_tile(w, h)].set_texture("maze_textures/walk_way_shadow_vertical.png");
                break;

            case 7: //left-up
                tile_array[get_tile(w, h)].set_texture("maze_textures/walk_way_shadow_left_up.png");
                tile_array[get_tile(w, h)].set_hight(0);
                break;

            case 8: //right-up 
                tile_array[get_tile(w, h)].set_texture("maze_textures/walk_way_shadow_right_down.png");
                tile_array[get_tile(w, h)].set_hight(0);
                break;

            case 9: //left-down
                tile_array[get_tile(w, h)].set_texture("maze_textures/walk_way_shadow_left_down.png");
                tile_array[get_tile(w, h)].set_hight(0);
                break;

            case 10: //right-down
                tile_array[get_tile(w, h)].set_hight(0);
                tile_array[get_tile(w, h)].set_texture("maze_textures/walk_way_shadow_right_down.png");
                break;

            case 11: //hard-wall 
                tile_array[get_tile(w, h)].set_hight(0);
                tile_array[get_tile(w, h)].set_texture("maze_textures/maze_wall.png");
                break;

            case 12:
                break;

            default:
                break;
            }
        }
    }
}


void Map::show_it()
{
    std::cout << "iteration: " << rec_iter << std::endl;
    rec_iter = 0;
}

int Map::rec_pos(int x, int y, std::vector<std::vector <int>>& arg, int& prev_direction)
{
    Random rand;

    rec_iter++;
    int direction;

    //print_vector(arg, width + 2, height + 2);

    //================= set new location
    if (rand.flip_coin())
    {//horizontal
        if (rand.flip_coin())
        {
            direction = 3;
            x++;
        }//right
        else
        {
            direction = 4;
            x--;
        }//left
    }
    else
    {//vertical
        if (rand.flip_coin())
        {
            direction = 5;
            y++;
        }//up
        else
        {
            direction = 6;
            y--;
        }//down
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

void Map::build_frame(std::vector<std::vector <int>>& data, int start_x, int start_y, int end_x, int end_y)
{
    for (int h = 0; h < height + 2; h++)
    {
        for (int w = 0; w < width + 2; w++)
        {
            if (w == 0 || w == width + 1 || h == 0 || h == height + 1) data[h][w] = 9;
            else data[h][w] = 1;

        }
    }
}

void Map::print_vector(std::vector<std::vector <int>>& arg, int size_x, int size_y)
{
    std::cout << "vector: " << std::endl;

    for (int h = 0; h < size_y; h++)
    {
        for (int w = 0; w < size_x; w++)
        {
            std::cout << arg[h][w];

        }
        std::cout << std::endl;
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

void Map::set_corners(std::vector<std::vector<int>>& map_data)
{
    int direction, prev_direction;

    for (int i = directions.size() - 1; i > 0; i--)
    {
        prev_direction = directions[i];
        direction = directions[i - 1];

        if (prev_direction == direction) { }
        else if (prev_direction == 3)
        {
            if (direction == 5)
            { 
                directions[i] = 9;
            }
            else if (direction == 6)
            {
                directions[i] = 8;
            }
        }
        else if (prev_direction == 4)
        {
            if (direction == 5)
            {
                directions[i] = 7;
            }
            else if (direction == 6)
            {
                directions[i] = 9;
            }
        }
        else if (prev_direction == 5)
        {
            if (direction == 3)
            {
                directions[i] = 10;
            }
            else if (direction == 4)
            {
                directions[i] = 11;
            }
        }
        else if (prev_direction == 6)
        {
            if (direction == 3)
            {
                directions[i] = 12;
            }
            else if (direction == 4)
            {
                directions[i] = 13;
            }
        }
        else
        {
            std::cout << "ERROR DIRECTION" << std::endl;
        }
    }
}

void Map::save_data(std::vector<std::vector<int>>& map_data)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            data[i][j] = map_data[i][j];
        }
    }
}


int Map::get_tile(int x, int y)
{
    return y * width + x;
}