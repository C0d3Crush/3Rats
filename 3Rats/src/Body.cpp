#include "Body.h"
#include <memory>
#include <filesystem>



Body::Body() : texture(nullptr), ptr_renderer(nullptr),
    crop_rect{0, 0, 0, 0}, position_rect{0, 0, 0, 0},
    frame_width(0), frame_height(0), texture_width(0),
    origin_x(0), origin_y(0), radius(0), hight(0) { }

Body::~Body() { if (texture) SDL_DestroyTexture(texture); }

void Body::update(float delta) { }

void Body::draw(SDL_Renderer* renderTarget) { SDL_RenderCopy(renderTarget, texture, &crop_rect, &position_rect); }

void Body::set_surface(SDL_Renderer* renderTarget)
{
	file_path = "../meta_textures/place_holder.png";
	ptr_renderer = renderTarget;

	SDL_Surface* surface = IMG_Load(file_path.c_str());
	if (surface == NULL)
		std::cout << "Error Body Surface" << std::endl;
	else
	{
		texture = SDL_CreateTextureFromSurface(ptr_renderer, surface);
		if (texture == NULL)
			std::cout << "Error Body Texture" << std::endl;
	}

	SDL_FreeSurface(surface);

	SDL_QueryTexture(texture, NULL, NULL, &crop_rect.w, &crop_rect.h);
}

void Body::set_texture(const std::string& filePath)
{
	bool testing_textures = false;
	if (testing_textures == true)
	{
		std::cout << " " << std::endl;
		std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
		std::cout << "Current filepath: " << file_path << std::endl;
		std::cout << "coordinates of body -> x:" <<get_origin_x()<<" y: "<< get_origin_y()<<std::endl;
	}
	
	this->file_path = filePath;
	SDL_Surface* surface = IMG_Load(file_path.c_str());
	if (surface == nullptr) {
        std::cerr << "Failed to load image: " << IMG_GetError() << std::endl;
		//return false;
    }
	if (texture) SDL_DestroyTexture(texture);
	texture = SDL_CreateTextureFromSurface(ptr_renderer, surface);

	SDL_FreeSurface(surface);

	if (texture == nullptr) {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        //return false;
    }
	
	SDL_QueryTexture(texture, NULL, NULL, &crop_rect.w, &crop_rect.h);
	//return true;
}


void Body::set_cords(int x, int y)
{
	position_rect.x = x;
	position_rect.y = y;

	texture_width = crop_rect.w;

	frame_width = position_rect.w = crop_rect.w;
	frame_height = position_rect.h = crop_rect.h;

	origin_x = frame_width / 2;
	origin_y = frame_height / 2;

	radius = frame_width / 2;
}

void Body::set_hight(int value) { this->hight = value; }


int Body::get_hight() { return this->hight; }

int Body::get_origin_x() { return position_rect.x + origin_x; }

int Body::get_origin_y() { return position_rect.y + origin_y; }

int Body::get_radius() { return radius; }