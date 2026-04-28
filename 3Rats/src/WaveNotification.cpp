#include "WaveNotification.h"
#include "Logger.h"

WaveNotification::WaveNotification()
    : is_visible(false), display_timer(0.0f), fade_alpha(1.0f)
{
}

void WaveNotification::show_notification(const std::string& message)
{
    display_text = message;
    is_visible = true;
    display_timer = 0.0f;
    fade_alpha = 1.0f;
    
    Logger::info(Logger::SYS, "Wave notification shown: " + message);
}

void WaveNotification::update_notification(float delta)
{
    if (!is_visible) return;
    
    display_timer += delta;
    
    // Handle fade out effect
    if (display_timer > (DISPLAY_DURATION - FADE_DURATION))
    {
        float fade_progress = (display_timer - (DISPLAY_DURATION - FADE_DURATION)) / FADE_DURATION;
        fade_alpha = 1.0f - fade_progress;
        
        if (fade_alpha < 0.0f) 
        {
            fade_alpha = 0.0f;
        }
    }
    
    // Hide notification after duration
    if (display_timer >= DISPLAY_DURATION)
    {
        is_visible = false;
        display_timer = 0.0f;
        fade_alpha = 1.0f;
    }
}

void WaveNotification::draw_notification(SDL_Renderer* renderer)
{
    if (!is_visible || fade_alpha <= 0.0f) return;
    
    // Window dimensions (600x420 from CLAUDE.md)
    const int window_width = 600;
    const int window_height = 420;
    
    // Background rectangle dimensions
    const int bg_width = 400;
    const int bg_height = 80;
    const int bg_x = (window_width - bg_width) / 2;
    const int bg_y = (window_height - bg_height) / 2;
    
    // Semi-transparent background with fade
    Uint8 bg_alpha = (Uint8)(128 * fade_alpha);  // 50% transparency with fade
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, bg_alpha);
    SDL_Rect background = {bg_x, bg_y, bg_width, bg_height};
    SDL_RenderFillRect(renderer, &background);
    
    // Text color with fade
    SDL_Color text_color = {255, 255, 255, (Uint8)(255 * fade_alpha)};
    
    // Load font for rendering
    TTF_Font* notification_font = TTF_OpenFont("../fonts/sans.ttf", 24);
    if (notification_font)
    {
        SDL_Surface* text_surface = TTF_RenderText_Blended(notification_font, display_text.c_str(), text_color);
        if (text_surface)
        {
            SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
            if (text_texture)
            {
                // Center text in background
                int text_width = text_surface->w;
                int text_height = text_surface->h;
                int text_x = bg_x + (bg_width - text_width) / 2;
                int text_y = bg_y + (bg_height - text_height) / 2;
                
                SDL_Rect text_rect = {text_x, text_y, text_width, text_height};
                SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
                
                SDL_DestroyTexture(text_texture);
            }
            SDL_FreeSurface(text_surface);
        }
        TTF_CloseFont(notification_font);
    }
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}