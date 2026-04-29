#pragma once

#include <string>

enum class TimePhase {
    DAY,
    DUSK,
    NIGHT,
    DAWN
};

enum class GameState {
    ACTIVE,
    WARNING,
    GAME_OVER,
    DAY_EVALUATION
};

class WaveNotification;
class WaveManager;

class TimeManager {
private:
    // Core time tracking
    double spacetime;
    int minutes;
    int hours;
    int day_number;
    TimePhase current_phase;
    GameState game_state;
    
    // Game mechanics
    bool rats_in_cage;
    bool warning_shown;
    float warning_timer;
    float evaluation_timer;
    
    // Time control
    float time_scale;
    bool time_frozen;
    
    // Time configuration (in game minutes)
    static constexpr int DAY_START_HOUR = 16;
    static constexpr int DAY_START_MINUTE = 30;
    static constexpr int DUSK_HOUR = 20;
    static constexpr int NIGHT_HOUR = 22;
    static constexpr int DAWN_HOUR = 6;
    
    // Timing constants
    static constexpr double SECONDS_PER_GAME_MINUTE = 5.0;
    static constexpr float WARNING_DURATION = 10.0f;  // Show warning for 10 seconds
    static constexpr float EVALUATION_DURATION = 5.0f; // Show evaluation for 5 seconds
    
    // Notification system
    WaveNotification* notification_system;
    
    void update_phase();
    void check_cage_safety();
    void trigger_warning();
    void trigger_game_over();
    void trigger_day_evaluation();

public:
    TimeManager();
    
    void update(double delta);
    void set_notification_system(WaveNotification* notifications);
    
    // Time getters
    int get_hours() const { return hours; }
    int get_minutes() const { return minutes; }
    int get_day_number() const { return day_number; }
    TimePhase get_phase() const { return current_phase; }
    GameState get_game_state() const { return game_state; }
    
    // Game mechanics
    void set_rats_in_cage(bool in_cage) { rats_in_cage = in_cage; }
    bool are_rats_safe() const { return rats_in_cage; }
    bool is_night_time() const { return current_phase == TimePhase::NIGHT; }
    bool is_day_time() const { return current_phase == TimePhase::DAY; }
    
    // Game control
    void advance_to_next_day();
    void reset_game();
    void set_time(int hour, int minute);
    
    // Difficulty scaling
    float get_difficulty_multiplier() const;
    int get_max_enemies_for_night() const;
    
    // Display
    std::string get_time_string() const;
    std::string get_phase_string() const;
    std::string get_day_summary() const;
    
    // Console commands
    void force_phase(TimePhase phase);
    void force_game_over() { trigger_game_over(); }
    
    // Time control
    void set_time_scale(float scale);
    float get_time_scale() const { return time_scale; }
    void freeze_time() { time_frozen = true; }
    void unfreeze_time() { time_frozen = false; }
    bool is_time_frozen() const { return time_frozen; }
};