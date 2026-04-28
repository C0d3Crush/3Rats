#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

#include "Acteur.h"
#include "Map.h"
#include "Topography.h"
#include "Item.h"
#include "Tile.h"

class WaveManager;
class Enemy;

class Console
{
private:
    bool is_open;
    std::string input;
    std::vector<std::string> history;

    TTF_Font* font;
    SDL_Renderer* renderer;

    Acteur*    player_array;
    int        player_amount;
    Map*       map_array;
    int        map_amount;
    Topography* topography;
    Item*      item_array;
    int        item_amount;
    Tile*      tile_array;
    int        tile_amount;
    WaveManager* wave_manager;
    Enemy*     enemy_array;
    int        max_enemies;

    void execute(const std::string& cmd);
    void log(const std::string& msg);
    void draw_rect_alpha(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void draw_text(const std::string& text, int x, int y, SDL_Color colour);

public:
    Console();
    ~Console();

    void init(SDL_Renderer* rend,
              Acteur* players, int p_amount,
              Map* maps,       int m_amount,
              Topography* topo,
              Item* items,     int i_amount,
              Tile* tiles,     int t_amount,
              WaveManager* wave_mgr,
              Enemy* enemies,  int max_enemy_count);

    void handle_event(const SDL_Event& e);
    void draw();

    bool get_open() const { return is_open; }
};
