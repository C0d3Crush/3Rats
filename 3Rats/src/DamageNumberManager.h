#pragma once

#include "DamageNumber.h"
#include <vector>
#include <SDL2/SDL_ttf.h>

class DamageNumberManager
{
private:
    std::vector<DamageNumber> damage_numbers;
    TTF_Font* font;

public:
    DamageNumberManager();
    ~DamageNumberManager();
    
    bool init_font(const std::string& font_path, int font_size);
    void add_damage_number(int damage, float x, float y, SDL_Color color = {255, 100, 100, 255});
    void update(float delta);
    void draw(SDL_Renderer* renderer);
    void clear();
};