#pragma once	

#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include <string>

class Body
{
protected:
	SDL_Rect cropRect;
	SDL_Texture* texture;
	SDL_Rect positionRect;

	SDL_Renderer* ptr_renderer;

	float frameCounter, searchCounter;
	float frameWidth, frameHeight;
	int textureWidth;
	SDL_Scancode keys[4];
	int originX, originY;
	int radius;
	std::string filePath;

	int hight;

public:

	Body();
	~Body();

	std::string get_filepath();
	void Update(float delta);
	void Draw(SDL_Renderer* renderTarget);

	void set_surface(SDL_Renderer* renderTarget, std::string name);
	void set_texture(std::string name);

	void set_cords(int x, int y);
	void SetX(int x);
	void SetY(int y);

	int GetOriginX();
	int GetOriginY();
	int GetRadius();

	void set_hight(int value);
	int get_hight();
};