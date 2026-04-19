#pragma once

#include <SDL2/SDL.h>
#include <algorithm>

class HealthBar
{
private:
    int x, y;           // Position (top-left corner)
    int width, height;  // Bar dimensions
    int current_value;
    int max_value;

    // Color interpolation: green (100%) → yellow (50%) → red (0%)
    SDL_Color calculate_color() const;

public:
    HealthBar();
    HealthBar(int bar_width, int bar_height);

    // Update bar values
    void set_position(int px, int py);
    void set_values(int current, int max);
    void set_current(int current);

    // Getters
    int get_current() const { return current_value; }
    int get_max() const { return max_value; }
    float get_percentage() const;

    // Rendering
    void draw(SDL_Renderer* renderer);
};
