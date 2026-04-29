#include "TimeManager.h"
#include "WaveNotification.h"
#include "Logger.h"
#include <sstream>
#include <iomanip>

TimeManager::TimeManager()
    : spacetime(0.0), minutes(DAY_START_MINUTE), hours(DAY_START_HOUR),
      day_number(1), current_phase(TimePhase::DAY), game_state(GameState::ACTIVE),
      rats_in_cage(false), warning_shown(false), warning_timer(0.0f),
      evaluation_timer(0.0f), notification_system(nullptr),
      time_scale(1.0f), time_frozen(false)
{
    update_phase();
    Logger::info(Logger::SYS, "TimeManager initialized - Day 1 starting at " + get_time_string());
}

void TimeManager::update(double delta)
{
    // Only advance time during active gameplay and when not frozen
    if (game_state == GameState::ACTIVE && !time_frozen) {
        spacetime += delta * time_scale;
        
        // Advance time every SECONDS_PER_GAME_MINUTE
        if (spacetime >= SECONDS_PER_GAME_MINUTE) {
            spacetime = 0.0;
            minutes++;
            
            if (minutes >= 60) {
                minutes = 0;
                hours++;
            }
            
            if (hours >= 24) {
                hours = 0;
                advance_to_next_day();
            }
            
            update_phase();
        }
        
        // Check for warnings and game over conditions
        check_cage_safety();
        
    } else if (game_state == GameState::WARNING) {
        warning_timer -= delta;
        if (warning_timer <= 0.0f) {
            game_state = GameState::ACTIVE;
            warning_shown = false;
        }
        
    } else if (game_state == GameState::DAY_EVALUATION) {
        evaluation_timer -= delta;
        if (evaluation_timer <= 0.0f) {
            game_state = GameState::ACTIVE;
        }
    }
}

void TimeManager::update_phase()
{
    TimePhase old_phase = current_phase;
    
    if (hours >= DAWN_HOUR && hours < DAY_START_HOUR) {
        current_phase = TimePhase::DAWN;
    } else if (hours >= DAY_START_HOUR && hours < DUSK_HOUR) {
        current_phase = TimePhase::DAY;
    } else if (hours >= DUSK_HOUR && hours < NIGHT_HOUR) {
        current_phase = TimePhase::DUSK;
    } else {
        current_phase = TimePhase::NIGHT;
    }
    
    // Log phase changes
    if (old_phase != current_phase) {
        Logger::info(Logger::SYS, "Time phase changed to " + get_phase_string() + " at " + get_time_string());
        
        // Show notifications for important phase changes
        if (notification_system) {
            if (current_phase == TimePhase::DUSK) {
                notification_system->show_notification("DUSK APPROACHES - RETURN TO CAGE SOON!");
            } else if (current_phase == TimePhase::NIGHT) {
                notification_system->show_notification("NIGHT HAS FALLEN!");
            } else if (current_phase == TimePhase::DAWN) {
                notification_system->show_notification("DAWN BREAKS - NEW DAY BEGINS!");
                trigger_day_evaluation();
            }
        }
    }
}

void TimeManager::check_cage_safety()
{
    // During dusk, warn if rats aren't in cage
    if (current_phase == TimePhase::DUSK && !rats_in_cage && !warning_shown) {
        trigger_warning();
    }
    
    // At night, check for game over
    if (current_phase == TimePhase::NIGHT && !rats_in_cage) {
        trigger_game_over();
    }
}

void TimeManager::trigger_warning()
{
    if (!warning_shown) {
        warning_shown = true;
        warning_timer = WARNING_DURATION;
        game_state = GameState::WARNING;
        
        if (notification_system) {
            notification_system->show_notification("WARNING: GET TO CAGE BEFORE NIGHT!");
        }
        
        Logger::warn(Logger::SYS, "Cage safety warning triggered at " + get_time_string());
    }
}

void TimeManager::trigger_game_over()
{
    game_state = GameState::GAME_OVER;
    
    if (notification_system) {
        notification_system->show_notification("GAME OVER - RATS CAUGHT OUTSIDE AT NIGHT!");
    }
    
    Logger::info(Logger::SYS, "Game over triggered - rats outside during night on day " + std::to_string(day_number));
}

void TimeManager::trigger_day_evaluation()
{
    evaluation_timer = EVALUATION_DURATION;
    game_state = GameState::DAY_EVALUATION;
    
    if (notification_system) {
        std::string message = "DAY " + std::to_string(day_number) + " COMPLETE";
        if (day_number > 1) {
            message += " - DIFFICULTY INCREASED";
        }
        notification_system->show_notification(message);
    }
    
    Logger::info(Logger::SYS, get_day_summary());
}

void TimeManager::advance_to_next_day()
{
    day_number++;
    Logger::info(Logger::SYS, "Advanced to day " + std::to_string(day_number));
}

void TimeManager::reset_game()
{
    spacetime = 0.0;
    minutes = DAY_START_MINUTE;
    hours = DAY_START_HOUR;
    day_number = 1;
    current_phase = TimePhase::DAY;
    game_state = GameState::ACTIVE;
    rats_in_cage = false;
    warning_shown = false;
    warning_timer = 0.0f;
    evaluation_timer = 0.0f;
    
    update_phase();
    Logger::info(Logger::SYS, "TimeManager reset - Day 1 starting");
}

void TimeManager::set_time(int hour, int minute)
{
    spacetime = 0.0;
    hours = hour;
    minutes = minute;
    
    // Reset game state to active when manually setting time
    // This prevents day evaluation loops when scripts change time
    game_state = GameState::ACTIVE;
    evaluation_timer = 0.0f;
    warning_timer = 0.0f;
    warning_shown = false;
    
    // Update phase without triggering day evaluation for manual time changes
    TimePhase old_phase = current_phase;
    
    if (hours >= DAWN_HOUR && hours < DAY_START_HOUR) {
        current_phase = TimePhase::DAWN;
    } else if (hours >= DAY_START_HOUR && hours < DUSK_HOUR) {
        current_phase = TimePhase::DAY;
    } else if (hours >= DUSK_HOUR && hours < NIGHT_HOUR) {
        current_phase = TimePhase::DUSK;
    } else {
        current_phase = TimePhase::NIGHT;
    }
    
    // Log phase changes but don't trigger day evaluation for manual time changes
    if (old_phase != current_phase) {
        Logger::info(Logger::SYS, "Time phase changed to " + get_phase_string() + " at " + get_time_string());
        
        // Show notifications for important phase changes (but not day evaluation)
        if (notification_system) {
            if (current_phase == TimePhase::DUSK) {
                notification_system->show_notification("DUSK APPROACHES - RETURN TO CAGE SOON!");
            } else if (current_phase == TimePhase::NIGHT) {
                notification_system->show_notification("NIGHT HAS FALLEN!");
            } else if (current_phase == TimePhase::DAWN) {
                notification_system->show_notification("DAWN BREAKS - NEW DAY BEGINS!");
                // Don't trigger day evaluation for manual time changes
            }
        }
    }
    
    Logger::info(Logger::SYS, "Time manually set to " + get_time_string());
}

float TimeManager::get_difficulty_multiplier() const
{
    // Difficulty scales with day number
    return 1.0f + (day_number - 1) * 0.2f; // 20% increase per day
}

int TimeManager::get_max_enemies_for_night() const
{
    // Base enemies + scaling per night
    int base_enemies = 3;
    int additional = (day_number - 1) * 2;
    return base_enemies + additional;
}

std::string TimeManager::get_time_string() const
{
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << hours << ":"
        << std::setfill('0') << std::setw(2) << minutes;
    return oss.str();
}

std::string TimeManager::get_phase_string() const
{
    switch (current_phase) {
        case TimePhase::DAY: return "Day";
        case TimePhase::DUSK: return "Dusk";
        case TimePhase::NIGHT: return "Night";
        case TimePhase::DAWN: return "Dawn";
        default: return "Unknown";
    }
}

std::string TimeManager::get_day_summary() const
{
    std::ostringstream oss;
    oss << "Day " << day_number << " summary - ";
    oss << "Difficulty multiplier: " << get_difficulty_multiplier() << " ";
    oss << "Max enemies: " << get_max_enemies_for_night();
    return oss.str();
}

void TimeManager::force_phase(TimePhase phase)
{
    switch (phase) {
        case TimePhase::DAY:
            set_time(DAY_START_HOUR, DAY_START_MINUTE);
            break;
        case TimePhase::DUSK:
            set_time(DUSK_HOUR, 0);
            break;
        case TimePhase::NIGHT:
            set_time(NIGHT_HOUR, 0);
            break;
        case TimePhase::DAWN:
            set_time(DAWN_HOUR, 0);
            break;
    }
}

void TimeManager::set_notification_system(WaveNotification* notifications)
{
    notification_system = notifications;
}

void TimeManager::set_time_scale(float scale)
{
    if (scale < 0.0f) scale = 0.0f;
    if (scale > 10.0f) scale = 10.0f;  // Reasonable maximum
    
    time_scale = scale;
    Logger::info(Logger::SYS, "Time scale set to " + std::to_string(time_scale));
}