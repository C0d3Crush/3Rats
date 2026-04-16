#pragma once	

#include <iostream>
#include <string>

#include "Body.h"
#include "Item.h"
#include "Map.h"
#include "Tile.h"
//#include "Topography.h"
#include "Door.h"
#include "Controller.h"

class Acteur : public Body, public Controller
{
protected:

	SDL_Scancode keys[4];

	float frameCounter, searchCounter;




	// holding item type 
	// should be done by the inventory not in the acteur!


	int item_hold_id;

	float waitCounter;
	bool wait;

	int last_map_id;



	float moveSpeed;
	bool is_moving;

	bool dead;

	int   saturation;		// 0 - 100
	float hungerTimer;
	float nextHungerTick;	// randomised interval between decays

// for update fuction
	std::vector<std::vector<bool>> get_blocked_array(Tile* tile_array, int length);
	void check_door(Topography* topography, Map* map_array, int map_amount, Tile* tile_array, int length);
	void make_acteur_move(controller_move move, block_direction direction, float delta);
	void follow_front_rat(int rat_x, int rat_y, int front_rat_x, int front_rat_y, block_direction direction, float delta, Acteur& front_rat);
	void follow_goal(int rat_x, int rat_y, int goal_x, int goal_y, block_direction direction, float delta, Item& item);
	void hold_item_in_mouth(Item& item);

public:

	void Draw(SDL_Renderer* renderTarget);

	//need body
	Acteur();

	void Update(float delta, const Uint8* keyState, int mode, Acteur& p1);

	void set_cords_frames(int x, int y, int framesX, int framesY);

	void teleport_to_entrence();
	void teleport_to_door(int side);
	bool intersectsWithBody(Body& b);		//get intersect

	//not
	~Acteur();
	void use_item();		// set_item_use

	void set_move_speed(float speed) { moveSpeed = speed; }
	void debug_give_item(Item* items, int count);
	int  get_saturation() const { return saturation; }
};
