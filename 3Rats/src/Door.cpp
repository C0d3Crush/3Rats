#include "Door.h"

Door::Door(int x_pos, int y_pos, int t, bool active)
{
	x = x_pos;
	y = y_pos;
	type = t;
	is_active = active;
	side = -1;
}

Door::Door()
{
	x = 0;
	y = 0;
	type = 0;
	is_active = false;
	side = -1;
}

int Door::get_x()
{
	return x;
}

int Door::get_y()
{
	return y;
}

int Door::get_type()
{
	return type;
}

bool Door::get_active()
{
	return is_active;
}

void Door::init_door(int x_pos, int y_pos, int t, bool active, int s)
{
	x = x_pos;
	y = y_pos;
	type = t;
	is_active = active;
	side = s;
}

int Door::get_side() { return side; }

void Door::set_cords(int x_pos, int y_pos)
{
	x = x_pos;
	y = y_pos;
}

void Door::set_active(bool value)
{
	is_active = value;
}
