#include "TimeDisplay.h"
#include <sstream>
#include <iomanip>

TimeDisplay::TimeDisplay() : time_manager(nullptr) {
}

void TimeDisplay::init(SDL_Renderer* renderer, TimeManager* tm) {
    time_manager = tm;
    
    // Initialize as Panel with time display styling
    Text::set_renderer(renderer);
    init_text("../fonts/sans.ttf", 20, {255, 255, 255}, 20, 20, 200, 60);
    
    Body::set_surface(renderer);
    set_texture("../ui_textures/clock_frame.png");
    set_cords(10, 10);
}

void TimeDisplay::update() {
    if (!time_manager) return;
    
    // Create display text with time and phase
    std::ostringstream oss;
    oss << time_manager->get_time_string();
    oss << "\n" << time_manager->get_phase_string();
    oss << " - Day " << time_manager->get_day_number();
    
    display_text = oss.str();
    
    // Update text color based on phase
    SDL_Color phase_color = get_phase_color();
    // Note: Text color will be set during init, phase color shown via background
    
    Panel::update();
}

void TimeDisplay::draw(SDL_Renderer* renderer) {
    if (!time_manager) return;
    
    // Draw background panel
    Panel::draw(renderer);
    
    // Add phase indicator if needed
    std::string indicator = get_phase_indicator();
    if (!indicator.empty()) {
        // TODO: Could add special indicators like warning icons
    }
}

SDL_Color TimeDisplay::get_phase_color() const {
    if (!time_manager) return {255, 255, 255, 255}; // White default
    
    switch (time_manager->get_phase()) {
        case TimePhase::DAY:
            return {255, 255, 0, 255};   // Yellow for day
        case TimePhase::DUSK:
            return {255, 165, 0, 255};   // Orange for dusk
        case TimePhase::NIGHT:
            return {128, 0, 128, 255};   // Purple for night
        case TimePhase::DAWN:
            return {255, 192, 203, 255}; // Pink for dawn
        default:
            return {255, 255, 255, 255}; // White fallback
    }
}

std::string TimeDisplay::get_phase_indicator() const {
    if (!time_manager) return "";
    
    switch (time_manager->get_game_state()) {
        case GameState::WARNING:
            return "⚠"; // Warning symbol
        case GameState::GAME_OVER:
            return "💀"; // Skull for game over
        case GameState::DAY_EVALUATION:
            return "✓"; // Check mark for evaluation
        default:
            return "";
    }
}