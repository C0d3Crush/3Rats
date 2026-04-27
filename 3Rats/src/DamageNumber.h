#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class DamageNumber
{
private:
    float x, y;           // Current position
    float start_x, start_y;  // Starting position
    float velocity_y;     // Upward velocity
    float lifetime;       // How long this number has existed
    int damage_amount;    // The damage value to display
    SDL_Color color;      // Text color
    
    static constexpr float MAX_LIFETIME = 2.0f;   // 2 seconds total
    static constexpr float FLOAT_SPEED = 50.0f;   // pixels per second upward
    static constexpr float FADE_START = 1.0f;     // Start fading after 1 second

public:
    DamageNumber(int damage, float pos_x, float pos_y, SDL_Color text_color);
    
    void update(float delta);
    void draw(SDL_Renderer* renderer, TTF_Font* font);
    
    bool is_expired() const { return lifetime >= MAX_LIFETIME; }
    float get_alpha() const;
};