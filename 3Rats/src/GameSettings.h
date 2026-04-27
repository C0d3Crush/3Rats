#pragma once

#include <string>

struct GameSettings
{
    // Audio settings
    float master_volume;
    float music_volume;
    float sfx_volume;
    bool audio_enabled;
    
    // Visual settings
    bool show_health_bars;
    bool show_damage_numbers;
    bool show_fps;
    int  window_width;
    int  window_height;
    bool fullscreen;
    
    // Gameplay settings
    float game_speed;
    bool debug_mode;
    bool god_mode;
    
    // Constructor with defaults
    GameSettings()
        : master_volume(1.0f),
          music_volume(0.7f),
          sfx_volume(0.8f),
          audio_enabled(true),
          show_health_bars(true),
          show_damage_numbers(true),
          show_fps(false),
          window_width(600),
          window_height(420),
          fullscreen(false),
          game_speed(1.0f),
          debug_mode(false),
          god_mode(false)
    {
    }
    
    // Save/Load settings
    bool save_to_file(const std::string& filename) const;
    bool load_from_file(const std::string& filename);
    void reset_to_defaults();
};