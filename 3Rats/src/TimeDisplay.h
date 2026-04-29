#pragma once

#include "Panel.h"
#include "TimeManager.h"

class TimeDisplay : public Panel {
private:
    TimeManager* time_manager;
    
    // Visual styling based on time phase
    SDL_Color get_phase_color() const;
    std::string get_phase_indicator() const;
    
public:
    TimeDisplay();
    
    void init(SDL_Renderer* renderer, TimeManager* tm);
    void update();
    void draw(SDL_Renderer* renderer);
    
    // Set time manager reference
    void set_time_manager(TimeManager* tm) { time_manager = tm; }
};