#pragma once

#include <utility>

class Door
{
private:
	int type;
	int x, y;
	int side;        // 0=N, 1=E, 2=S, 3=W, -1=none
	bool is_active;

public:
	Door(int x_pos, int y_pos, int t, bool active);
	Door();

	void init_door(int x_pos, int y_pos, int t, bool is_active, int side = -1);

	int get_x();
	int get_y();
	int get_type();
	int get_side();
	bool get_active();

	void set_cords(int x_pos, int y_pos);
	void set_active(bool value);

};