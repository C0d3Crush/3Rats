#pragma once

#include "Body.h"

class Tile : public Body
{
private:

public:
	Tile();
	Tile(const Tile& other);
	//Tile(bool exit, bool entrance, bool hole, std::string path, int height);
	
	~Tile();

	//Tile& operator=(Tile rhs);

	bool is_exit;
	bool is_entrance;
	bool is_hole;
	int  door_side;  // -1=not a door; 0=N, 1=E, 2=S, 3=W

};