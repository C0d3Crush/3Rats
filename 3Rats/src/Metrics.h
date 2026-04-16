#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>

#include "Acteur.h"
#include "Topography.h"

class Metrics
{
private:
    bool is_open;

    TTF_Font*   font;
    SDL_Renderer* renderer;

    Acteur*     player_array;
    int         player_amount;
    Topography* topography;

    Uint32 last_tick;
    float  fps;
    int    frame_count;

    size_t get_memory_mb();
    void   draw_text(const std::string& text, int x, int y, SDL_Color colour);
    void   draw_rect_alpha(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

public:
    Metrics();
    ~Metrics();

    void init(SDL_Renderer* rend, Acteur* players, int p_amount, Topography* topo);
    void update();
    void draw();
    void toggle() { is_open = !is_open; }
    bool get_open() const { return is_open; }
};
