#pragma once

#include "Panel.h"
#include "TimeManager.h"
#include "Acteur.h"

class DayEvaluationScreen : public Panel {
private:
    TimeManager* time_manager;
    Acteur* player_array;
    int player_count;
    bool is_visible;
    float display_timer;
    
    static constexpr float DISPLAY_DURATION = 5.0f;
    
    std::string get_evaluation_message() const;
    void restore_rat_health();
    
public:
    DayEvaluationScreen();
    
    void init(SDL_Renderer* renderer, TimeManager* tm, Acteur* players, int player_count);
    void update(float delta);
    void draw(SDL_Renderer* renderer);
    void show();
    void hide();
    
    bool get_visible() const { return is_visible; }
    bool should_continue() const;
    
    void set_time_manager(TimeManager* tm) { time_manager = tm; }
    void set_players(Acteur* players, int count) { player_array = players; player_count = count; }
};