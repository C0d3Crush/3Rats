#include "DamageNumber.h"
#include <algorithm>

DamageNumber::DamageNumber(int damage, float pos_x, float pos_y, SDL_Color text_color)
    : x(pos_x), y(pos_y), start_x(pos_x), start_y(pos_y),
      velocity_y(-FLOAT_SPEED), lifetime(0.0f), damage_amount(damage), color(text_color)
{
}

void DamageNumber::update(float delta)
{
    lifetime += delta;
    
    // Move upward
    y += velocity_y * delta;
    
    // Slow down over time
    velocity_y += 20.0f * delta;  // Decelerate upward movement
    if (velocity_y > 0) velocity_y = 0;  // Don't fall down
}

float DamageNumber::get_alpha() const
{
    if (lifetime < FADE_START) {
        return 1.0f;  // Full opacity
    } else {
        // Fade out over the remaining time
        float fade_progress = (lifetime - FADE_START) / (MAX_LIFETIME - FADE_START);
        return std::max(0.0f, 1.0f - fade_progress);
    }
}

void DamageNumber::draw(SDL_Renderer* renderer, TTF_Font* font)
{
    if (!font) return;
    
    // Create damage text
    std::string damage_text = "-" + std::to_string(damage_amount);
    
    // Apply alpha to color
    SDL_Color fade_color = color;
    float alpha = get_alpha();
    fade_color.r = (Uint8)(fade_color.r * alpha);
    fade_color.g = (Uint8)(fade_color.g * alpha);
    fade_color.b = (Uint8)(fade_color.b * alpha);
    fade_color.a = (Uint8)(255 * alpha);
    
    // Create text surface
    SDL_Surface* text_surface = TTF_RenderText_Blended(font, damage_text.c_str(), fade_color);
    if (!text_surface) return;
    
    // Create texture from surface
    SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (!text_texture) {
        SDL_FreeSurface(text_surface);
        return;
    }
    
    // Set texture alpha blending
    SDL_SetTextureBlendMode(text_texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(text_texture, (Uint8)(255 * alpha));
    
    // Get text dimensions and render
    int text_w, text_h;
    SDL_QueryTexture(text_texture, nullptr, nullptr, &text_w, &text_h);
    
    SDL_Rect dest_rect = {
        (int)(x - text_w / 2),  // Center horizontally
        (int)y,
        text_w,
        text_h
    };
    
    SDL_RenderCopy(renderer, text_texture, nullptr, &dest_rect);
    
    // Cleanup
    SDL_DestroyTexture(text_texture);
    SDL_FreeSurface(text_surface);
}