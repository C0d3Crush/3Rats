#include "GameOverScreen.h"
#include "Logger.h"
#include <sstream>

GameOverScreen::GameOverScreen()
    : time_manager(nullptr), is_visible(false), display_timer(0.0f)
{
}

void GameOverScreen::init(SDL_Renderer* renderer, TimeManager* tm)
{
    time_manager = tm;
    
    // Initialize text rendering
    Text::set_renderer(renderer);
    init_text("../fonts/sans.ttf", 24, {255, 0, 0}, 300, 150, 300, 120);
    
    // Initialize background panel
    Body::set_surface(renderer);
    set_texture("../ui_textures/fade.png");
    set_cords(0, 0);
    
    Logger::info(Logger::SYS, "GameOverScreen initialized");
}

void GameOverScreen::update(float delta)
{
    if (is_visible && time_manager) {
        display_timer += delta;
        display_text = get_game_over_message();
        Panel::update();
        
        // Auto-hide after display duration
        if (display_timer >= DISPLAY_DURATION) {
            hide();
        }
    }
}

void GameOverScreen::draw(SDL_Renderer* renderer)
{
    if (is_visible) {
        // Draw semi-transparent background
        Panel::draw(renderer);
    }
}

void GameOverScreen::show()
{
    if (time_manager && time_manager->get_game_state() == GameState::GAME_OVER) {
        is_visible = true;
        display_timer = 0.0f;
        display_text = get_game_over_message();
        Logger::info(Logger::SYS, "Game Over screen displayed: " + display_text);
        in(); // Show the panel
    }
}

void GameOverScreen::hide()
{
    is_visible = false;
    out(); // Hide the panel
}

bool GameOverScreen::should_restart() const
{
    // Game should restart after game over display finishes
    return !is_visible && display_timer > 0.0f;
}

std::string GameOverScreen::get_game_over_message() const
{
    if (!time_manager) {
        return "GAME OVER";
    }
    
    std::stringstream ss;
    ss << "GAME OVER\n\n";
    ss << "The rats failed to return to their cage!\n";
    ss << "Survived until: " << time_manager->get_time_string() << "\n";
    ss << "Day: " << time_manager->get_day_number() << "\n\n";
    ss << "Press R to restart or ESC to quit";
    
    return ss.str();
}