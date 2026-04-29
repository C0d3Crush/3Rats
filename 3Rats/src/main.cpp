#include <SDL2/SDL.h>
#include <iostream>
#include <SDL_image.h>
#include <vector>
#include <stdlib.h>     /* srand, rand */

#include "Item.h"
#include "Body.h"
#include "Map.h"
#include "Tile.h"
#include "Random.h"
//#include "Topography.h"
//#include "Text.h"
#include "Acteur.h"
#include "Clock.h"
#include "Fade.h"
#include "Overlay.h"
#include "Pause.h"
#include "Console.h"
#include "Metrics.h"
#include "Logger.h"
#include "Enemy.h"
#include "WaveManager.h"
#include "DamageNumberManager.h"
#include "WaveNotification.h"
#include "TimeManager.h"
#include "TimeDisplay.h"

static constexpr int MAX_ENEMIES = 50;

int world_seed_generation(bool value)
{
	if (value)
	{
		return 0;
	}
	else
	{
		int type_generation;
		int seed_input;

		std::cout << "What type of generation?" << std::endl;
		std::cout << "random seed: 0" << std::endl;
		std::cout << "user input: 1" << std::endl;
		std::cout << "fix seed (testing): 2" << std::endl;
		std::cin >> type_generation;

		switch (type_generation)
		{
		case 0:
			seed_input = 0;
			break;
		case 1:
			std::cin >> seed_input;
			break;
		case 2:
			seed_input = 0;
			break;
		default:
			break;
		}
		return seed_input;
	}
}
SDL_Texture* LoadTexture(std::string filePath, SDL_Renderer* renderTarget)
{
	SDL_Texture* texture = nullptr;
	SDL_Surface* surface = IMG_Load(filePath.c_str());
	if (surface == NULL)
		std::cout << "Error Surface1" << std::endl;
	else
	{
		texture = SDL_CreateTextureFromSurface(renderTarget, surface);
		if (texture == NULL)
			std::cout << "Error Texture" << std::endl;
	}

	SDL_FreeSurface(surface);

	return texture;
}

void init_fade(SDL_Renderer* render_target, Fade* fade)
{
	fade->Text::set_renderer(render_target);
	fade->init_text("../fonts/sans.ttf", 24, { 255, 0, 0 }, 999, 999, 200, 90);

	fade->Body::set_surface(render_target);
	fade->set_texture("../ui_textures/fade.png");
	fade->set_cords(999, 999);
	//clock->set_fade(&fade);
}

void init_pause(SDL_Renderer* render_target, Pause* pause)
{
	pause->Text::set_renderer(render_target);
	pause->init_text("../fonts/sans.ttf", 24, { 255, 0, 0 }, 999, 999, 200, 90);

	pause->Body::set_surface(render_target);
	pause->set_texture("../ui_textures/fade.png");
	pause->set_cords(999, 999);
}

void init_clock(SDL_Renderer* render_target, Clock* clock, Fade* fade, Overlay* overlay)
{
	clock->Text::set_renderer(render_target);
	clock->init_text("../fonts/sans.ttf", 24, { 255, 0, 0 }, 400, 330, 200, 90);

	clock->Body::set_surface(render_target);
	clock->set_texture("../ui_textures/clock_frame.png");
	clock->set_cords(400, 320);
	//clock->set_fade(&fade);
	//clock->set_overlay(overlay);
	clock->set_time(16, 30);
}

void init_overlay(SDL_Renderer* render_target, Fade* fade, Clock* clock, Overlay* overlay)
{
	overlay->init(fade, clock);
}

void init_item_array(SDL_Renderer* render_target, Item item_array[], int item_amount)
{
	Item item_templet;
	item_templet.set_surface(render_target);
	item_templet.set_texture("../meta_textures/place_holder.png");
	item_templet.set_cords(-100, -100);

	for (int i = 0; i < item_amount; i++)
	{
		item_array[i] = item_templet;
	}
}

void init_tile_array(SDL_Renderer* render_target, Tile tile_array[], int tile_amount)
{
	Tile tile_templet;
	tile_templet.set_surface(render_target);
	tile_templet.set_texture("../meta_textures/place_holder.png");
	tile_templet.set_cords(-100, -100);

	for (int i = 0; i < tile_amount; i++)
	{
		tile_array[i] = tile_templet;
	}
}

template<typename T>
void init_grid_coords(T* array, int size, int height, int width)
{
	int x_cord = 0;
	int y_cord = 0;

	int i = 0;

	for (int h = 0; h < height; h++)
	{
		for (int w = 0; w < width; w++)
		{
			x_cord = w * 64;
			y_cord = h * 64;
			array[i].set_cords(x_cord, y_cord);
			i++;
		}
	}
}

void init_map_array(SDL_Renderer* renderTarget, Tile* tile_array, int tile_amount, Item* item_array, int item_amount, Map* map_array, int map_amount, Random& random)
{
	Map map_templet;

	map_templet.set_tile_array(tile_array, tile_amount);
	init_grid_coords(tile_array, tile_amount, map_templet.get_hight(), map_templet.get_width());
	map_templet.set_item_array(item_array, item_amount);
	init_grid_coords(item_array, item_amount, map_templet.get_hight(), map_templet.get_width());
	map_templet.set_random_pointer(random);

	for (int i = 0; i < map_amount; i++)
	{
		map_array[i] = map_templet;
		map_array[i].set_map_id(i);
	}
}

void init_topography(SDL_Renderer* renderTarget, Map* map_ptr, int map_amount, Topography* topography, Random& random)
{
	topography->set_renderer(renderTarget);

	Item* item_ptr  = map_ptr[0].get_item_array();
	int   item_amount = map_ptr[0].get_item_size();

	Tile* tile_ptr  = map_ptr[0].get_tile_array();
	int   tile_amount = map_ptr[0].get_tile_size();

	topography->set_map_array(map_ptr, map_amount);
	topography->set_item_array(item_ptr, item_amount);
	topography->set_tile_array(tile_ptr, tile_amount);
	topography->set_random_pointer(random);

	topography->set_up();
	topography->generate_connections();

	for (int i = 0; i < map_amount; i++)
	{
		map_ptr[i].set_layout(topography->get_layout(i));
		map_ptr[i].set_type(i == 0 ? 2 : random.flip_coin());
	}
	map_ptr[0].set_textures();
}

void init_player_array(SDL_Renderer* render_target, Acteur* player_array, int player_amount, Topography& topography, Random& random)
{
	for (int i = 0; i < player_amount; i++)
	{
		Acteur player;
		player.set_controller_number(i);
		player.set_Topography(&topography);
		player.set_random_pointer(random);
		player_array[i] = player;
	}

	player_array[0].set_surface(render_target);
	player_array[0].set_texture("../rat_textures/mango.png");
	player_array[0].set_cords_frames(32, 32, 3, 4);

	player_array[1].set_surface(render_target);
	player_array[1].set_texture("../rat_textures/fridolin.png");
	player_array[1].set_cords_frames(32, 32, 3, 4);

	player_array[2].set_surface(render_target);
	player_array[2].set_texture("../rat_textures/remy.png");
	player_array[2].set_cords_frames(400, 300, 3, 4);
}

void init_entity(SDL_Renderer* render_target, Acteur* entitys, int entity_amount, Topography& topography, Random& random)
{
	for (int i = 0; i < entity_amount; i++)
	{
		Acteur entity;
		entity.set_controller_number(3+i);
		entity.set_Topography(&topography);
		entity.set_random_pointer(random);
		entitys[i] = entity;
	}

	entitys[0].set_surface(render_target);
	entitys[0].set_texture("../npc_textures/entity.png");
	entitys[0].set_cords_frames(4000, 4000, 1, 1);
}

uint32_t generate_seed(int seed_generation)
{
	Random key_generator;

	uint32_t seed = 1;

	std::cout << "Seed: ";

	switch (seed_generation)
	{
	case 0:
		seed = key_generator.roll_custom_dice(999 * 999);
		break;
	case 1:
		std::cin >> seed;
		break;
	case 2:
		seed = 1;
		break;
	default:
		break;
	}

	Logger::info(Logger::SYS, "Seed: " + std::to_string(seed));
	return seed;
}

int main(int argc, char* argv[])
{
	// Initiallaizing and loading variables
	SDL_Window* window = nullptr;
	SDL_Renderer* renderTarget = nullptr;
	int currentTime = 0;
	int prevTime = 0;
	float delta = 0.0f;
	const Uint8* keyState; 
	int levelWidth, levelHeight;
	int mode = 0;
	bool menuOn = false;
	int bananAmount = 0;
	int hunger = 3;

	int wait = 12.0f;	//12.0f

	int time = 0;

	const int screen_width = 600;
	const int screen_hight = 420;

	const int tile_amount = 54;
	const int map_amount = 25;  // 5x5 topology grid
	const int item_amount = 54;
	const int player_amount = 3;
	const int entity_amount = 1;

	Logger::init();

	// Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        Logger::error(Logger::SYS, std::string("SDL init failed: ") + SDL_GetError());
        return 1;
    }

    // Create a window
	window = SDL_CreateWindow("3Rats", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_hight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
        std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

	// Create a renderer
	renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderTarget == nullptr) {
        Logger::error(Logger::SYS, std::string("renderer creation failed: ") + SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
	Logger::info(Logger::SYS, "SDL window + renderer created (" +
	    std::to_string(screen_width) + "x" + std::to_string(screen_hight) + ")");

	// Initialize SDL_ttf
	if (TTF_Init() < 0)
		Logger::error(Logger::SYS, std::string("TTF_Init failed: ") + TTF_GetError());

	// Initialize SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        Logger::error(Logger::SYS, std::string("SDL_image init failed: ") + IMG_GetError());
        SDL_DestroyRenderer(renderTarget);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

	//WHY TF DOES THIS MESS UP THE CODE?????
	
	/* 
	bool testing = false;
	if(testing == true)
	{
		SDL_version compiled;
		SDL_version linked;

		SDL_VERSION(&compiled);
		SDL_GetVersion(&linked);

		std::cout << "Compiled SDL version: " << (int)compiled.major << "." << (int)compiled.minor << "." << (int)compiled.patch << std::endl;
		std::cout << "Linked SDL version: " << (int)linked.major << "." << (int)linked.minor << "." << (int)linked.patch << std::endl;

	}
	*/
	
	// ================================ INIT GAME OBJECTS ================================
	// ===================================================================================
	
	int seed_input = world_seed_generation(1); // 1 = testing/ 2 = normal
	
	uint32_t seed = generate_seed(seed_input);
	//std::cout << "Seed: " << seed << std::endl;

	// random object
	Random random(seed);

	Fade fade;
	init_fade(renderTarget, &fade);

	Pause pause;
	init_pause(renderTarget, &pause);

	Clock clock;
	Overlay overlay;
	init_clock(renderTarget, &clock, &fade, &overlay);
	init_overlay(renderTarget, &fade, &clock, &overlay);

	// Body* clock_frame_ptr = &clock_frame; // ahhhhh! thats how pointers work
	
	Item item_array[item_amount];
	init_item_array(renderTarget, item_array, item_amount);

	Tile tile_array[tile_amount];
	init_tile_array(renderTarget, tile_array, tile_amount);

	Map map_array[map_amount];
	init_map_array(renderTarget, tile_array, tile_amount, item_array, item_amount, map_array, map_amount, random);

	Topography topography;
	init_topography(renderTarget, map_array, map_amount, &topography, random);

	Acteur player_array[player_amount];
	init_player_array(renderTarget, player_array, player_amount, topography, random);

	Acteur entity[entity_amount];
	init_entity(renderTarget, entity, entity_amount, topography, random);

	// Initialize enemy array
	Enemy enemy_array[MAX_ENEMIES];
	for (int i = 0; i < MAX_ENEMIES; i++) {
		enemy_array[i].set_surface(renderTarget);
		enemy_array[i].set_texture("../npc_textures/entity.png");
		enemy_array[i].set_topography(&topography);
	}

	// Initialize wave manager
	WaveManager wave_manager;

	// Initialize wave notification system
	WaveNotification wave_notification;
	wave_notification.set_surface(renderTarget);
	wave_manager.set_notification_system(&wave_notification);

	// Initialize damage number manager
	DamageNumberManager damage_manager;
	damage_manager.init_font("../fonts/sans.ttf", 16);
	
	// Set damage manager for all enemies
	for (int i = 0; i < MAX_ENEMIES; i++) {
		enemy_array[i].set_damage_manager(&damage_manager);
	}

	// Initialize time management system
	TimeManager time_manager;
	time_manager.set_notification_system(&wave_notification);
	
	// Initialize time display UI
	TimeDisplay time_display;
	time_display.init(renderTarget, &time_manager);

	Metrics metrics;
	metrics.init(renderTarget, player_array, player_amount, &topography);

	Console console;
	console.init(renderTarget,
	             player_array, player_amount,
	             map_array,    map_amount,
	             &topography,
	             item_array,   item_amount,
	             tile_array,   tile_amount,
	             &wave_manager,
	             enemy_array, MAX_ENEMIES);

	// ===================================================================================


	SDL_Texture* texture = LoadTexture("../backgound.png", renderTarget);
	SDL_QueryTexture(texture, NULL, NULL, &levelWidth, &levelHeight);
	
	bool isRunning = true;
	SDL_Event ev;

	while (isRunning)
	{
		prevTime = currentTime;
		currentTime = SDL_GetTicks();
		delta = (currentTime - prevTime) / 1000.0f;

		while (SDL_PollEvent(&ev) != 0)		// ------------- key-events
		{
			console.handle_event(ev);

			// Getting the quit and keyboard events
			if (ev.type == SDL_QUIT)
				isRunning = false;
			else if (ev.type == SDL_KEYDOWN && !console.get_open()) {
				switch (ev.key.keysym.sym)
				{
				case SDLK_SPACE:
					mode++;
					if (mode == 2) mode = 0;
					break;

				case SDLK_1:
					texture = LoadTexture("../empty.png", renderTarget);
					break;
				case SDLK_u:
					player_array[0].use_item();
					player_array[1].use_item();
					player_array[2].use_item();
					player_array[0].set_enter(false);
					break;

				case SDLK_r:
					player_array[0].teleport_to_entrence();
					player_array[1].teleport_to_entrence();
					player_array[2].teleport_to_entrence();
					player_array[0].set_enter(false);
					player_array[1].set_enter(false);
					player_array[2].set_enter(false);
					break;
				case SDLK_o:
					entity[0].teleport_to_entrence();
					break;

				case SDLK_p:
					player_array[1].place_item();
					player_array[2].place_item();
					player_array[0].set_enter(false);
					break;

				case SDLK_n:
					player_array[1].set_has_goal(false);
					player_array[2].set_has_goal(false);
					player_array[0].set_enter(false);
					break;
				case SDLK_e:
					player_array[0].set_enter(true);
					break;
				case SDLK_t:
					std::cout << "tp next room" << std::endl;
					break;
				case SDLK_z:
					std::cout << "tp prev room" << std::endl;
					break;
				case SDLK_m:
					pause.in();
					break;
				case SDLK_l:
					pause.out();
					break;
				case SDLK_0:
					break;
				case SDLK_TAB:
					metrics.toggle();
					break;
				}
			}
		}

		keyState = SDL_GetKeyboardState(NULL);

		// =================================== UPDATE GAME ===================================
		// ===================================================================================

		metrics.update();
		//topography.update(delta);

		// Update time management system and check cage safety
		time_manager.update(delta);
		
		// Check if all rats are in cage room (map_type == 2 is cage)
		int current_map_id = topography.get_current_map_id();
		bool all_rats_in_cage = map_array[current_map_id].get_type() == 2;
		time_manager.set_rats_in_cage(all_rats_in_cage);
		
		// Update time display
		time_display.update();

		player_array[0].Update(delta, keyState, mode, player_array[2]);
		for (int i = 1; i < player_amount; i++)
		{
			player_array[i].Update(delta, keyState, mode, player_array[i - 1]);
		}

		entity[0].update(delta);
		
		// Update wave manager and enemies
		wave_manager.update(delta, enemy_array, MAX_ENEMIES, map_array, map_amount);
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemy_array[i].get_is_active()) {
				enemy_array[i].update(delta, player_array, player_amount);
			}
		}
		
		// Update damage numbers and wave notifications
		damage_manager.update(delta);
		wave_notification.update_notification(delta);
		std::string pause_message = "Pause.";
		pause.update(pause_message);
		clock.update(delta);
		fade.update(std::to_string(clock.get_day()));
		overlay.update(delta);

		SDL_QueryTexture(texture, NULL, NULL, &levelWidth, &levelHeight);

		// Drawing the curent image to the window
		SDL_RenderClear(renderTarget);
		SDL_RenderCopy(renderTarget, texture, NULL, NULL);

		// ==================================== DRAW GAME ====================================
		// ===================================================================================

		topography.draw(renderTarget);

		for (int i = 0; i < player_amount; i++)
		{
			player_array[i].Draw(renderTarget);
		}
		entity[0].draw(renderTarget);
		
		// Draw all active enemies and their health bars
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (enemy_array[i].get_is_active()) {
				enemy_array[i].draw(renderTarget);
				enemy_array[i].draw_health_bar(renderTarget);
			}
		}
		clock.draw(renderTarget);
		fade.draw(renderTarget);
		pause.draw(renderTarget);
		overlay.draw(renderTarget);
		damage_manager.draw(renderTarget);
		wave_notification.draw_notification(renderTarget);
		time_display.draw(renderTarget);
		metrics.draw();
		console.draw();

		SDL_RenderPresent(renderTarget);
	}

	// Freeing the memory
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderTarget);
	SDL_DestroyTexture(texture);

	window = nullptr;
	renderTarget = nullptr;
	texture = nullptr;

	IMG_Quit();
	SDL_Quit();
	Logger::shutdown();

	return 0;

}
