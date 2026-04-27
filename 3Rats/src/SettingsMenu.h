#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include "GameSettings.h"
#include "Body.h"

class SettingsMenu
{
public:
    enum MenuState {
        MAIN_MENU,
        AUDIO_SETTINGS,
        VIDEO_SETTINGS,
        GAMEPLAY_SETTINGS
    };

private:
    MenuState current_state;
    GameSettings* settings;
    TTF_Font* font;
    
    int selected_option;
    std::vector<std::string> menu_options;
    
    static constexpr int MENU_X = 150;
    static constexpr int MENU_Y = 100;
    static constexpr int OPTION_HEIGHT = 30;
    static constexpr int MENU_WIDTH = 300;
    static constexpr int MENU_HEIGHT = 220;

    void setup_main_menu();
    void setup_audio_menu();
    void setup_video_menu();
    void setup_gameplay_menu();
    
    void handle_main_menu_selection();
    void handle_audio_menu_selection();
    void handle_video_menu_selection();
    void handle_gameplay_menu_selection();
    
    void draw_background(SDL_Renderer* renderer);
    void draw_menu_options(SDL_Renderer* renderer);
    void draw_current_values(SDL_Renderer* renderer);

public:
    SettingsMenu();
    ~SettingsMenu();
    
    bool init_font(const std::string& font_path, int font_size = 14);
    void set_settings(GameSettings* game_settings);
    void reset();
    
    void handle_key_up();
    void handle_key_down();
    void handle_key_left();
    void handle_key_right();
    void handle_enter();
    void handle_escape();
    
    void update(float delta);
    void draw(SDL_Renderer* renderer);
    
    MenuState get_state() const { return current_state; }
    bool is_in_main_menu() const { return current_state == MAIN_MENU; }
};