#pragma once

#include "Panel.h"
#include "TimeManager.h"

class GameOverScreen : public Panel {
private:
    TimeManager* time_manager;
    bool is_visible;
    float display_timer;
    
    static constexpr float DISPLAY_DURATION = 10.0f;
    
    std::string get_game_over_message() const;
    
public:
    GameOverScreen();
    
    void init(SDL_Renderer* renderer, TimeManager* tm);
    void update(float delta);
    void draw(SDL_Renderer* renderer);
    void show();
    void hide();
    
    bool get_visible() const { return is_visible; }
    bool should_restart() const;
    
    void set_time_manager(TimeManager* tm) { time_manager = tm; }
};