#include "Body.h"
#include <memory>
#include "Tile.h"

Tile::Tile()
{
	is_entrance = false;
	is_exit = false;
	is_hole = false;
	door_side = -1;

	file_path = "../meta_textures/place_holder.png";
}

Tile::Tile(const Tile& other)
{
	std::cout << "copy constructor tile" << std::endl;
	is_exit = other.is_exit;
	is_entrance = other.is_entrance;
	is_hole = other.is_hole;
	door_side = other.door_side;
	this->hight = other.hight;

	this->file_path = other.file_path;
	this->ptr_renderer = other.ptr_renderer;

	set_surface(other.ptr_renderer);
	set_texture(other.file_path);
	set_cords(other.origin_x, other.origin_y);
}

Tile::~Tile()
{
	//std::cout << "destructor" << std::endl;
}