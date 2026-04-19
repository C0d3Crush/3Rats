#include "HealthBar.h"
#include "Logger.h"

HealthBar::HealthBar()
    : x(0), y(0), width(48), height(6), current_value(100), max_value(100)
{
}

HealthBar::HealthBar(int bar_width, int bar_height)
    : x(0), y(0), width(bar_width), height(bar_height), current_value(100), max_value(100)
{
}

void HealthBar::set_position(int px, int py)
{
    x = px;
    y = py;
}

void HealthBar::set_values(int current, int max)
{
    current_value = std::max(0, std::min(current, max));
    max_value = std::max(1, max);  // Prevent division by zero
}

void HealthBar::set_current(int current)
{
    current_value = std::max(0, std::min(current, max_value));
}

float HealthBar::get_percentage() const
{
    if (max_value == 0) return 0.0f;
    return static_cast<float>(current_value) / static_cast<float>(max_value);
}

SDL_Color HealthBar::calculate_color() const
{
    float percentage = get_percentage();
    SDL_Color color;

    if (percentage > 0.5f) {
        // Green (100%) → Yellow (50%)
        // Interpolate from green (0,255,0) to yellow (255,255,0)
        float t = (percentage - 0.5f) * 2.0f;  // 0.0 at 50%, 1.0 at 100%
        color.r = static_cast<Uint8>(255 * (1.0f - t));
        color.g = 255;
        color.b = 0;
    } else {
        // Yellow (50%) → Red (0%)
        // Interpolate from yellow (255,255,0) to red (255,0,0)
        float t = percentage * 2.0f;  // 0.0 at 0%, 1.0 at 50%
        color.r = 255;
        color.g = static_cast<Uint8>(255 * t);
        color.b = 0;
    }

    color.a = 255;
    return color;
}

void HealthBar::draw(SDL_Renderer* renderer)
{
    // Draw background (dark gray)
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_Rect bg_rect = {x, y, width, height};
    SDL_RenderFillRect(renderer, &bg_rect);

    // Draw border (black)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &bg_rect);

    // Draw health bar (colored based on percentage)
    float percentage = get_percentage();
    int fill_width = static_cast<int>(width * percentage);

    if (fill_width > 0) {
        SDL_Color color = calculate_color();
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_Rect fill_rect = {x, y, fill_width, height};
        SDL_RenderFillRect(renderer, &fill_rect);
    }
}
