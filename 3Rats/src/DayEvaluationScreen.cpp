#include "DayEvaluationScreen.h"
#include "Logger.h"
#include <sstream>

DayEvaluationScreen::DayEvaluationScreen()
    : time_manager(nullptr), player_array(nullptr), player_count(0), 
      is_visible(false), display_timer(0.0f)
{
}

void DayEvaluationScreen::init(SDL_Renderer* renderer, TimeManager* tm, Acteur* players, int count)
{
    time_manager = tm;
    player_array = players;
    player_count = count;
    
    // Initialize text rendering
    Text::set_renderer(renderer);
    init_text("../fonts/sans.ttf", 20, {0, 255, 0}, 300, 150, 300, 150);
    
    // Initialize background panel
    Body::set_surface(renderer);
    set_texture("../ui_textures/fade.png");
    set_cords(0, 0);
    
    Logger::info(Logger::SYS, "DayEvaluationScreen initialized");
}

void DayEvaluationScreen::update(float delta)
{
    if (is_visible && time_manager) {
        display_timer += delta;
        display_text = get_evaluation_message();
        Panel::update();
        
        // Auto-continue after display duration
        if (display_timer >= DISPLAY_DURATION) {
            hide();
        }
    }
}

void DayEvaluationScreen::draw(SDL_Renderer* renderer)
{
    if (is_visible) {
        // Draw semi-transparent background
        Panel::draw(renderer);
    }
}

void DayEvaluationScreen::show()
{
    if (time_manager && time_manager->get_game_state() == GameState::DAY_EVALUATION) {
        is_visible = true;
        display_timer = 0.0f;
        display_text = get_evaluation_message();
        
        // Restore rat health during day evaluation
        restore_rat_health();
        
        Logger::info(Logger::SYS, "Day evaluation screen displayed for day " + 
                    std::to_string(time_manager->get_day_number()));
        in(); // Show the panel
    }
}

void DayEvaluationScreen::hide()
{
    is_visible = false;
    out(); // Hide the panel
}

bool DayEvaluationScreen::should_continue() const
{
    // Continue to next day after evaluation display finishes
    return !is_visible && display_timer > 0.0f;
}

void DayEvaluationScreen::restore_rat_health()
{
    if (!player_array || player_count <= 0) {
        return;
    }
    
    // Restore rats' saturation during day rest
    for (int i = 0; i < player_count; i++) {
        int current_saturation = player_array[i].get_saturation();
        int restored_amount = 25; // Restore 25 saturation per day
        
        player_array[i].reduce_saturation(-restored_amount); // Negative = heal
        
        Logger::info(Logger::SYS, "Rat " + std::to_string(i) + " health restored: " +
                    std::to_string(current_saturation) + " -> " + 
                    std::to_string(player_array[i].get_saturation()));
    }
}

std::string DayEvaluationScreen::get_evaluation_message() const
{
    if (!time_manager || !player_array) {
        return "DAY EVALUATION";
    }
    
    std::stringstream ss;
    ss << "DAY " << time_manager->get_day_number() << " EVALUATION\n\n";
    ss << "The rats return safely to their cage!\n";
    ss << "All rats rest and recover health...\n\n";
    
    ss << "Rat Status:\n";
    for (int i = 0; i < player_count; i++) {
        ss << "  Rat " << (i + 1) << ": " << player_array[i].get_saturation() << "% health\n";
    }
    
    ss << "\nDifficulty for next night: x" << time_manager->get_difficulty_multiplier() << "\n";
    ss << "Preparing for Day " << (time_manager->get_day_number() + 1) << "...";
    
    return ss.str();
}