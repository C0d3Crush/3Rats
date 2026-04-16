#include "Acteur.h"

std::vector<std::vector<bool>> Acteur::get_blocked_array(Tile* tile_array, int length)
{

	// make vector for tile and what direction it blocks
	std::vector<std::vector<bool>> blocked_i(length, std::vector<bool>(4));

	for (int i = 0; i < length; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			blocked_i[i][k] = false;
		}
	}

	// struckt with the directions to be blocked

	struct direction {
		bool right;
		bool left;
		bool up;
		bool down;
	};

	direction block = { false, false, false, false };

	for (int i = 0; i < length; i++)
	{
		if (intersectsWithBody(tile_array[i]))
		{
			if (tile_array[i].get_hight() == 1)
			{

				int delta_x = tile_array[i].get_origin_x() - position_rect.x;
				int delta_y = tile_array[i].get_origin_y() - position_rect.y;

				if (delta_x > 0) block.right = true;
				else if (delta_x < 0) block.left = true;

				if (delta_y > 0) block.down = true;
				else if (delta_y < 0) block.up = true;

				blocked_i[i][0] = block.right;
				blocked_i[i][1] = block.left;
				blocked_i[i][2] = block.down;
				blocked_i[i][3] = block.up;
			}
			else if (tile_array[i].get_hight() == 0)
			{
				block.right = false;
				block.left = false;
				block.down = false;
				block.up = false;
			}
		}
	}
	return blocked_i;
}


void Acteur::check_door(Topography* topography, Map* map_array, int map_amount, Tile* tile_array, int length)
{
	if (!wants_enter_door) return;
	if (controller_number != 0) return;

	static const int opposite[4] = {2, 3, 0, 1};  // N<->S, E<->W

	int current_map_id = topography->get_current_map_id();

	for (int i = 0; i < length; i++)
	{
		if (!tile_array[i].is_exit)      continue;
		if (tile_array[i].door_side < 0) continue;
		if (!intersectsWithBody(tile_array[i])) continue;

		int side        = tile_array[i].door_side;
		int neighbor_id = topography->get_neighbor(current_map_id, side);
		if (neighbor_id < 0 || neighbor_id >= map_amount) continue;

		wants_enter_door = false;

		topography->set_current_map_id(neighbor_id);
		map_array[neighbor_id].set_textures();

		int arrival = opposite[side];
		topography->set_arrival_side(arrival);

		Door entry = map_array[neighbor_id].get_door(arrival);
		if (entry.get_active())
		{
			position_rect.x = entry.get_x() * 64 - crop_rect.w;
			position_rect.y = entry.get_y() * 64 - crop_rect.h;
		}
		break;
	}
}



void Acteur::make_acteur_move(controller_move move, block_direction direction, float delta)
{
	if (move.up && !direction.up)	//up
	{
		position_rect.y -= moveSpeed * delta;
		crop_rect.y = frame_height * 3;
		current_direction = 0;
	}
	else if (move.down && !direction.down)			//down
	{
		position_rect.y += moveSpeed * delta;
		crop_rect.y = 0;
		current_direction = 1;
	}
	else if (move.left && !direction.left)			//left
	{
		position_rect.x -= moveSpeed * delta;
		crop_rect.y = frame_height;
		current_direction = 2;
	}
	else if (move.right && !direction.right)			//right
	{
		position_rect.x += moveSpeed * delta;
		crop_rect.y = frame_height * 2;
		current_direction = 3;
	}
	else
	{
		is_moving = false;
	}
}

void Acteur::follow_front_rat(int rat_x, int rat_y, int front_rat_x, int front_rat_y, block_direction direction, float delta, Acteur& front_rat)
{
	if (rat_y > front_rat_y && !direction.up)
	{
		position_rect.y -= moveSpeed * delta;
		crop_rect.y = frame_height * 3;
		current_direction = 0;
	}
	else if (rat_y < front_rat_y && !direction.down)
	{
		position_rect.y += moveSpeed * delta;
		crop_rect.y = 0;
		current_direction = 1;
	}
	else if (rat_x > front_rat_x && !direction.left)
	{
		position_rect.x -= moveSpeed * delta;
		crop_rect.y = frame_height;
		current_direction = 2;
	}
	else if (rat_x < front_rat_x && !direction.right)
	{
		position_rect.x += moveSpeed * delta;
		crop_rect.y = frame_height * 2;
		current_direction = 3;
	}
	else
	{
		current_direction = front_rat.GetDirection();
	}

	if (current_direction == 0) crop_rect.y = frame_height * 3;
	if (current_direction == 1) crop_rect.y = 0;
	if (current_direction == 2) crop_rect.y = frame_height;
	if (current_direction == 3) crop_rect.y = frame_height * 2;
}

void Acteur::follow_goal(int rat_x, int rat_y, int goal_x, int goal_y, block_direction direction, float delta, Item& item)
{
	if (rat_y > goal_y && !direction.up)
	{
		position_rect.y -= moveSpeed * delta;
		crop_rect.y = frame_height * 3;
		current_direction = 0;
	}
	else if (rat_y < goal_y && !direction.down)
	{
		position_rect.y += moveSpeed * delta;
		crop_rect.y = 0;
		current_direction = 1;
	}
	else if (rat_x > goal_x && !direction.left)
	{
		position_rect.x -= moveSpeed * delta;
		crop_rect.y = frame_height;
		current_direction = 2;
	}
	else if (rat_x < goal_x && !direction.right)
	{
		position_rect.x += moveSpeed * delta;
		crop_rect.y = frame_height * 2;
		current_direction = 3;
	}
	else if (rat_x == goal_x && rat_y == goal_y && !item.get_pick_up())
	{
		std::cout << "found!" << " p: " << controller_number << "item number: " << item_search_id << std::endl;;

		item_hold_id = item_search_id;
		holds_item = true;
		item.set_pick_up(true);
		has_goal = false;

		item_type = 1;

	}
	else if (rat_x == goal_x && rat_y == goal_y && item.get_pick_up())
	{
		std::cout << "did not found!" << " p: " << controller_number << "item number: " << item_search_id << std::endl;;
		has_goal = false;
	}
}

void Acteur::hold_item_in_mouth(Item& item)
{
	// add he offsets to make more sense. just for example i did it fo 14
	int offset = 14;

	if (current_direction == 0)
	{
		item.set_cords(get_origin_x() - 24, get_origin_y() - 32 - offset);
	}
	else if (current_direction == 1)
	{
		item.set_cords(get_origin_x() - 24, get_origin_y() - 32 + offset);
	}
	else if (current_direction == 2)
	{
		item.set_cords(get_origin_x() - 24 - offset, get_origin_y() - 32);
	}
	else if (current_direction == 3)
	{
		item.set_cords(get_origin_x() - 24 + offset, get_origin_y() - 32);
	}
}

void Acteur::teleport_to_entrence()
{
	Map* map_ptr = topography->get_map_array();
	int  map_id  = topography->get_current_map_id();

	for (int side = 0; side < 4; side++)
	{
		Door d = map_ptr[map_id].get_door(side);
		if (d.get_active())
		{
			set_cords(d.get_x() * 64 - crop_rect.w,
			          d.get_y() * 64 - crop_rect.h);
			return;
		}
	}
}

void Acteur::teleport_to_door(int side)
{
	Map* map_ptr = topography->get_map_array();
	int  map_id  = topography->get_current_map_id();
	Door d       = map_ptr[map_id].get_door(side);

	if (d.get_active())
		set_cords(d.get_x() * 64 - crop_rect.w,
		          d.get_y() * 64 - crop_rect.h);
	else
		teleport_to_entrence();
}

Acteur::Acteur()
{
	dead = false;
	item_type = 0;

	saturation      = 100;
	hungerTimer     = 0.0f;
	nextHungerTick  = 3.0f;

	file_path = "../meta_textures/place_holder.png";
	item_search_id = 0;
	item_hold_id = 0;
	has_goal = false;
	holds_item = false;
	wants_enter_door = false;

	is_moving = false;

	keys[0] = SDL_SCANCODE_W;
	keys[1] = SDL_SCANCODE_S;
	keys[2] = SDL_SCANCODE_A;
	keys[3] = SDL_SCANCODE_D;

	moveSpeed = 200.0f;

	wait = false;
	last_map_id = 0;

	searchCounter = rand() % 100;
	searchCounter /= 100.0f;
}

Acteur::~Acteur() { }


void Acteur::set_cords_frames(int x, int y, int framesX, int framesY)
{
	position_rect.x = x;
	position_rect.y = y;

	texture_width = crop_rect.w;

	crop_rect.w /= framesX;
	crop_rect.h /= framesY;

	frame_width = position_rect.w = crop_rect.w;
	frame_height = position_rect.h = crop_rect.h;

	origin_x = frame_width / 2;
	origin_y = frame_height / 2;

	radius = frame_width / 2;
}


void Acteur::Update(float delta, const Uint8* keyState, int mode, Acteur& front_rat)
{
	map_array = topography->get_map_array();
	map_array_size = topography->get_map_size();

	tile_array = topography->get_tile_array();
	tile_array_size = topography->get_tile_size();

	item_array = topography->get_item_array();
	item_array_size = topography->get_item_size();

	// teleport follower rats when the player switches rooms
	int current_map_id = topography->get_current_map_id();
	if (controller_number != 0 && current_map_id != last_map_id)
	{
		teleport_to_door(topography->get_arrival_side());
	}
	last_map_id = current_map_id;

	// hunger decay
	hungerTimer += delta;
	if (hungerTimer >= nextHungerTick)
	{
		hungerTimer = 0.0f;
		nextHungerTick = 3.0f + random_ptr->roll_custom_dice(30) / 10.0f; // 3.1 – 6.0 s
		if (saturation > 0) saturation--;
	}

	if (is_item_available_on_map())
	{
		make_goal();	// make it so: goal = make_goal();
	}
	else
	{
		mode = 0;
	}

	int rat_x = this->get_origin_x();
	int rat_y = this->get_origin_y();

	// focus point of a non-acteur rat.
	// maybe put this inside the function that makes 
	// the cords to it to stand and hole items
	int frontRatX = front_rat.get_origin_x();
	int frontRatY = front_rat.get_origin_y();

	std::pair <int, int> offests = direction_to_offset(front_rat.GetDirection());

	rat_x += offests.first;
	rat_y += offests.second;

	/*
	float dist1 = sqrt(pow(abs(front_rat.get_origin_x() - rat_x), 2) + pow(abs(front_rat.get_origin_y() - rat_y), 2));
	float dist2 = sqrt(pow(abs(front_rat.get_origin_x() - rat_x), 2) + pow(abs(front_rat.get_origin_y() - rat_y), 2));
	*/

	std::vector<std::vector<bool>> collision_map;

	init_colision_map(collision_map);
	block_direction_counter collision_counter = { 0, 0, 0, 0 };
	block_direction direction = { 0, 0, 0, 0 };

	// colision with door check
	check_door(topography, map_array, map_array_size, tile_array, tile_array_size);

	collision_map = get_blocked_array(tile_array, tile_array_size);

	calculate_blocked_side(collision_counter, collision_map, tile_array_size);

	get_direction_blocked(collision_counter, direction, tile_array_size);

	// slow rat when starving
	float saved_speed = moveSpeed;
	if (saturation == 0) moveSpeed *= 0.5f;

	// make acteurs move

	controller_move move = {
		static_cast<bool>(keyState[keys[0]]),
		static_cast<bool>(keyState[keys[1]]),
		static_cast<bool>(keyState[keys[2]]),
		static_cast<bool>(keyState[keys[3]])
	};

	// acteur 1
	if (controller_number == 0)//--------------------Acteur control
	{
		make_acteur_move(move, direction, delta);
	}
	// acteur 2 & 3
	else
	{
		if (!wait)
		{
			//find item control
			if (mode == 1 && !holds_item)
			{
				follow_goal(rat_x, rat_y, goalX, goalY, direction, delta, item_array[item_search_id]);
			}
			// autopilot 
			else if (mode == 1 && holds_item)
			{
				follow_front_rat(rat_x, rat_y, frontRatX, frontRatY, direction, delta, front_rat);

			}
			else if (mode == 0)
			{
				follow_front_rat(rat_x, rat_y, frontRatX, frontRatY, direction, delta, front_rat);
			}
		}
	}

	moveSpeed = saved_speed;

	// make item visible on a acteur
	if (holds_item)
	{
		hold_item_in_mouth(item_array[item_hold_id]);
	}

	// make movement in texture for acteur
	if (is_moving)
	{
		frameCounter += delta;
		searchCounter += delta;
		if (wait) waitCounter += delta;


		if (frameCounter >= 0.25f)
		{
			frameCounter = 0;
			crop_rect.x += frame_width;
			if (crop_rect.x >= texture_width)
				crop_rect.x = 0;
		}

		if (searchCounter >= 2.0f)
		{
			searchCounter = 0;
			wait = true;
		}

		if (waitCounter >= 0.5f)
		{
			waitCounter = 0;
			wait = false;
		}

	}
	else
	{
		frameCounter = 0;
		crop_rect.x = frame_width;
	}
}

void Acteur::Draw(SDL_Renderer* renderTarget) { SDL_RenderCopy(renderTarget, texture, &crop_rect, &position_rect); }


bool Acteur::intersectsWithBody(Body& b)
{
	if (sqrt(pow(get_origin_x() - b.get_origin_x(), 2) + pow(get_origin_y() - b.get_origin_y(), 2)) >= radius + b.get_radius())
	{
		return false;
	}
	return true;
}


void Acteur::debug_give_item(Item* items, int count)
{
    if (holds_item) return;
    for (int i = 0; i < count; i++)
    {
        if (!items[i].get_pick_up())
        {
            items[i].set_pick_up(true);
            items[i].set_on_map(true);
            item_hold_id = i;
            holds_item   = true;
            item_type    = 1;
            return;
        }
    }
}

void Acteur::use_item()
{
	if (item_type == 0)
	{

	}
	else if (item_type == 1)
	{
		saturation = 100;

		holds_item = false;
		item_array[item_hold_id].set_cords(-100, -100);
		item_array[item_hold_id].set_on_map(false);

		std::cout << "yumm!" << std::endl;
		item_type = 0;

	}
}
