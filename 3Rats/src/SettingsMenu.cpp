#include "SettingsMenu.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>

SettingsMenu::SettingsMenu()
    : current_state(MAIN_MENU), settings(nullptr), font(nullptr), selected_option(0)
{
    setup_main_menu();
}

SettingsMenu::~SettingsMenu()
{
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
}

bool SettingsMenu::init_font(const std::string& font_path, int font_size)
{
    font = TTF_OpenFont(font_path.c_str(), font_size);
    if (!font) {
        Logger::warn(Logger::SYS, "SettingsMenu: Failed to load font " + font_path + " - " + TTF_GetError());
        return false;
    }
    
    Logger::info(Logger::SYS, "SettingsMenu: Font loaded successfully");
    return true;
}

void SettingsMenu::set_settings(GameSettings* game_settings)
{
    settings = game_settings;
}

void SettingsMenu::reset()
{
    current_state = MAIN_MENU;
    selected_option = 0;
    setup_main_menu();
}

void SettingsMenu::setup_main_menu()
{
    menu_options.clear();
    menu_options.push_back("Audio Settings");
    menu_options.push_back("Video Settings");
    menu_options.push_back("Gameplay Settings");
    menu_options.push_back("Save Settings");
    menu_options.push_back("Reset to Defaults");
    menu_options.push_back("Back to Game");
    
    if (selected_option >= (int)menu_options.size()) {
        selected_option = 0;
    }
}

void SettingsMenu::setup_audio_menu()
{
    menu_options.clear();
    menu_options.push_back("Master Volume");
    menu_options.push_back("Music Volume");
    menu_options.push_back("SFX Volume");
    menu_options.push_back("Audio Enabled");
    menu_options.push_back("Back");
    
    if (selected_option >= (int)menu_options.size()) {
        selected_option = 0;
    }
}

void SettingsMenu::setup_video_menu()
{
    menu_options.clear();
    menu_options.push_back("Show Health Bars");
    menu_options.push_back("Show Damage Numbers");
    menu_options.push_back("Show FPS");
    menu_options.push_back("Window Width");
    menu_options.push_back("Window Height");
    menu_options.push_back("Fullscreen");
    menu_options.push_back("Back");
    
    if (selected_option >= (int)menu_options.size()) {
        selected_option = 0;
    }
}

void SettingsMenu::setup_gameplay_menu()
{
    menu_options.clear();
    menu_options.push_back("Game Speed");
    menu_options.push_back("Debug Mode");
    menu_options.push_back("God Mode");
    menu_options.push_back("Back");
    
    if (selected_option >= (int)menu_options.size()) {
        selected_option = 0;
    }
}

void SettingsMenu::handle_key_up()
{
    selected_option--;
    if (selected_option < 0) {
        selected_option = (int)menu_options.size() - 1;
    }
}

void SettingsMenu::handle_key_down()
{
    selected_option++;
    if (selected_option >= (int)menu_options.size()) {
        selected_option = 0;
    }
}

void SettingsMenu::handle_key_left()
{
    if (!settings) return;
    
    switch (current_state) {
        case AUDIO_SETTINGS:
            switch (selected_option) {
                case 0: // Master Volume
                    settings->master_volume = std::max(0.0f, settings->master_volume - 0.1f);
                    break;
                case 1: // Music Volume
                    settings->music_volume = std::max(0.0f, settings->music_volume - 0.1f);
                    break;
                case 2: // SFX Volume
                    settings->sfx_volume = std::max(0.0f, settings->sfx_volume - 0.1f);
                    break;
                case 3: // Audio Enabled
                    settings->audio_enabled = !settings->audio_enabled;
                    break;
            }
            break;
            
        case VIDEO_SETTINGS:
            switch (selected_option) {
                case 0: settings->show_health_bars = !settings->show_health_bars; break;
                case 1: settings->show_damage_numbers = !settings->show_damage_numbers; break;
                case 2: settings->show_fps = !settings->show_fps; break;
                case 3: settings->window_width = std::max(400, settings->window_width - 50); break;
                case 4: settings->window_height = std::max(300, settings->window_height - 50); break;
                case 5: settings->fullscreen = !settings->fullscreen; break;
            }
            break;
            
        case GAMEPLAY_SETTINGS:
            switch (selected_option) {
                case 0: settings->game_speed = std::max(0.1f, settings->game_speed - 0.1f); break;
                case 1: settings->debug_mode = !settings->debug_mode; break;
                case 2: settings->god_mode = !settings->god_mode; break;
            }
            break;
            
        default:
            break;
    }
}

void SettingsMenu::handle_key_right()
{
    if (!settings) return;
    
    switch (current_state) {
        case AUDIO_SETTINGS:
            switch (selected_option) {
                case 0: // Master Volume
                    settings->master_volume = std::min(1.0f, settings->master_volume + 0.1f);
                    break;
                case 1: // Music Volume
                    settings->music_volume = std::min(1.0f, settings->music_volume + 0.1f);
                    break;
                case 2: // SFX Volume
                    settings->sfx_volume = std::min(1.0f, settings->sfx_volume + 0.1f);
                    break;
                case 3: // Audio Enabled
                    settings->audio_enabled = !settings->audio_enabled;
                    break;
            }
            break;
            
        case VIDEO_SETTINGS:
            switch (selected_option) {
                case 0: settings->show_health_bars = !settings->show_health_bars; break;
                case 1: settings->show_damage_numbers = !settings->show_damage_numbers; break;
                case 2: settings->show_fps = !settings->show_fps; break;
                case 3: settings->window_width = std::min(1920, settings->window_width + 50); break;
                case 4: settings->window_height = std::min(1080, settings->window_height + 50); break;
                case 5: settings->fullscreen = !settings->fullscreen; break;
            }
            break;
            
        case GAMEPLAY_SETTINGS:
            switch (selected_option) {
                case 0: settings->game_speed = std::min(5.0f, settings->game_speed + 0.1f); break;
                case 1: settings->debug_mode = !settings->debug_mode; break;
                case 2: settings->god_mode = !settings->god_mode; break;
            }
            break;
            
        default:
            break;
    }
}

void SettingsMenu::handle_enter()
{
    switch (current_state) {
        case MAIN_MENU:
            handle_main_menu_selection();
            break;
        case AUDIO_SETTINGS:
            handle_audio_menu_selection();
            break;
        case VIDEO_SETTINGS:
            handle_video_menu_selection();
            break;
        case GAMEPLAY_SETTINGS:
            handle_gameplay_menu_selection();
            break;
    }
}

void SettingsMenu::handle_escape()
{
    if (current_state == MAIN_MENU) {
        // Exit settings menu (handled by main game loop)
    } else {
        // Go back to main menu
        current_state = MAIN_MENU;
        selected_option = 0;
        setup_main_menu();
    }
}

void SettingsMenu::handle_main_menu_selection()
{
    switch (selected_option) {
        case 0: // Audio Settings
            current_state = AUDIO_SETTINGS;
            selected_option = 0;
            setup_audio_menu();
            break;
        case 1: // Video Settings
            current_state = VIDEO_SETTINGS;
            selected_option = 0;
            setup_video_menu();
            break;
        case 2: // Gameplay Settings
            current_state = GAMEPLAY_SETTINGS;
            selected_option = 0;
            setup_gameplay_menu();
            break;
        case 3: // Save Settings
            if (settings) {
                settings->save_to_file("game_settings.cfg");
            }
            break;
        case 4: // Reset to Defaults
            if (settings) {
                settings->reset_to_defaults();
            }
            break;
        case 5: // Back to Game
            // Exit handled by main game loop
            break;
    }
}

void SettingsMenu::handle_audio_menu_selection()
{
    if (selected_option == 4) { // Back
        current_state = MAIN_MENU;
        selected_option = 0;
        setup_main_menu();
    }
}

void SettingsMenu::handle_video_menu_selection()
{
    if (selected_option == 6) { // Back
        current_state = MAIN_MENU;
        selected_option = 0;
        setup_main_menu();
    }
}

void SettingsMenu::handle_gameplay_menu_selection()
{
    if (selected_option == 3) { // Back
        current_state = MAIN_MENU;
        selected_option = 0;
        setup_main_menu();
    }
}

void SettingsMenu::update(float delta)
{
    // Settings menu doesn't need delta-based updates currently
}

void SettingsMenu::draw_background(SDL_Renderer* renderer)
{
    // Semi-transparent background
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    
    SDL_Rect full_screen = {0, 0, 600, 420};
    SDL_RenderFillRect(renderer, &full_screen);
    
    // Menu background
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_Rect menu_bg = {MENU_X - 10, MENU_Y - 10, MENU_WIDTH + 20, MENU_HEIGHT + 20};
    SDL_RenderFillRect(renderer, &menu_bg);
    
    // Menu border
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(renderer, &menu_bg);
}

void SettingsMenu::draw_menu_options(SDL_Renderer* renderer)
{
    if (!font) return;
    
    SDL_Color normal_color = {200, 200, 200, 255};
    SDL_Color selected_color = {255, 255, 100, 255};
    
    for (size_t i = 0; i < menu_options.size(); i++) {
        SDL_Color color = (i == selected_option) ? selected_color : normal_color;
        
        SDL_Surface* text_surface = TTF_RenderText_Blended(font, menu_options[i].c_str(), color);
        if (!text_surface) continue;
        
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        if (!text_texture) {
            SDL_FreeSurface(text_surface);
            continue;
        }
        
        int text_w, text_h;
        SDL_QueryTexture(text_texture, nullptr, nullptr, &text_w, &text_h);
        
        SDL_Rect dest_rect = {
            MENU_X,
            MENU_Y + (int)i * OPTION_HEIGHT,
            text_w,
            text_h
        };
        
        SDL_RenderCopy(renderer, text_texture, nullptr, &dest_rect);
        
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
    }
}

void SettingsMenu::draw_current_values(SDL_Renderer* renderer)
{
    if (!font || !settings) return;
    
    SDL_Color value_color = {150, 255, 150, 255};
    
    for (size_t i = 0; i < menu_options.size(); i++) {
        std::string value_text;
        
        switch (current_state) {
            case AUDIO_SETTINGS:
                switch (i) {
                    case 0: {
                        std::ostringstream oss;
                        oss << std::fixed << std::setprecision(1) << (settings->master_volume * 100.0f) << "%";
                        value_text = oss.str();
                        break;
                    }
                    case 1: {
                        std::ostringstream oss;
                        oss << std::fixed << std::setprecision(1) << (settings->music_volume * 100.0f) << "%";
                        value_text = oss.str();
                        break;
                    }
                    case 2: {
                        std::ostringstream oss;
                        oss << std::fixed << std::setprecision(1) << (settings->sfx_volume * 100.0f) << "%";
                        value_text = oss.str();
                        break;
                    }
                    case 3: value_text = settings->audio_enabled ? "ON" : "OFF"; break;
                }
                break;
                
            case VIDEO_SETTINGS:
                switch (i) {
                    case 0: value_text = settings->show_health_bars ? "ON" : "OFF"; break;
                    case 1: value_text = settings->show_damage_numbers ? "ON" : "OFF"; break;
                    case 2: value_text = settings->show_fps ? "ON" : "OFF"; break;
                    case 3: value_text = std::to_string(settings->window_width); break;
                    case 4: value_text = std::to_string(settings->window_height); break;
                    case 5: value_text = settings->fullscreen ? "ON" : "OFF"; break;
                }
                break;
                
            case GAMEPLAY_SETTINGS:
                switch (i) {
                    case 0: {
                        std::ostringstream oss;
                        oss << std::fixed << std::setprecision(1) << settings->game_speed << "x";
                        value_text = oss.str();
                        break;
                    }
                    case 1: value_text = settings->debug_mode ? "ON" : "OFF"; break;
                    case 2: value_text = settings->god_mode ? "ON" : "OFF"; break;
                }
                break;
                
            default:
                continue;
        }
        
        if (value_text.empty()) continue;
        
        SDL_Surface* text_surface = TTF_RenderText_Blended(font, value_text.c_str(), value_color);
        if (!text_surface) continue;
        
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        if (!text_texture) {
            SDL_FreeSurface(text_surface);
            continue;
        }
        
        int text_w, text_h;
        SDL_QueryTexture(text_texture, nullptr, nullptr, &text_w, &text_h);
        
        SDL_Rect dest_rect = {
            MENU_X + 200,  // Right-aligned values
            MENU_Y + (int)i * OPTION_HEIGHT,
            text_w,
            text_h
        };
        
        SDL_RenderCopy(renderer, text_texture, nullptr, &dest_rect);
        
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(text_surface);
    }
}

void SettingsMenu::draw(SDL_Renderer* renderer)
{
    draw_background(renderer);
    draw_menu_options(renderer);
    draw_current_values(renderer);
}