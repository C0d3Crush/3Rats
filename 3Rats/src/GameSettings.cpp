#include "GameSettings.h"
#include "Logger.h"
#include <fstream>
#include <sstream>

bool GameSettings::save_to_file(const std::string& filename) const
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::warn(Logger::SYS, "GameSettings: Failed to open file for writing: " + filename);
        return false;
    }
    
    file << "# 3Rats Game Settings\n";
    file << "master_volume=" << master_volume << "\n";
    file << "music_volume=" << music_volume << "\n";
    file << "sfx_volume=" << sfx_volume << "\n";
    file << "audio_enabled=" << (audio_enabled ? 1 : 0) << "\n";
    file << "show_health_bars=" << (show_health_bars ? 1 : 0) << "\n";
    file << "show_damage_numbers=" << (show_damage_numbers ? 1 : 0) << "\n";
    file << "show_fps=" << (show_fps ? 1 : 0) << "\n";
    file << "window_width=" << window_width << "\n";
    file << "window_height=" << window_height << "\n";
    file << "fullscreen=" << (fullscreen ? 1 : 0) << "\n";
    file << "game_speed=" << game_speed << "\n";
    file << "debug_mode=" << (debug_mode ? 1 : 0) << "\n";
    file << "god_mode=" << (god_mode ? 1 : 0) << "\n";
    
    file.close();
    
    Logger::info(Logger::SYS, "GameSettings: Settings saved to " + filename);
    return true;
}

bool GameSettings::load_from_file(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::info(Logger::SYS, "GameSettings: No settings file found, using defaults: " + filename);
        return false;
    }
    
    std::string line;
    int loaded_count = 0;
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        size_t equals_pos = line.find('=');
        if (equals_pos == std::string::npos) continue;
        
        std::string key = line.substr(0, equals_pos);
        std::string value = line.substr(equals_pos + 1);
        
        if (key == "master_volume") {
            master_volume = std::stof(value);
            loaded_count++;
        }
        else if (key == "music_volume") {
            music_volume = std::stof(value);
            loaded_count++;
        }
        else if (key == "sfx_volume") {
            sfx_volume = std::stof(value);
            loaded_count++;
        }
        else if (key == "audio_enabled") {
            audio_enabled = (std::stoi(value) != 0);
            loaded_count++;
        }
        else if (key == "show_health_bars") {
            show_health_bars = (std::stoi(value) != 0);
            loaded_count++;
        }
        else if (key == "show_damage_numbers") {
            show_damage_numbers = (std::stoi(value) != 0);
            loaded_count++;
        }
        else if (key == "show_fps") {
            show_fps = (std::stoi(value) != 0);
            loaded_count++;
        }
        else if (key == "window_width") {
            window_width = std::stoi(value);
            loaded_count++;
        }
        else if (key == "window_height") {
            window_height = std::stoi(value);
            loaded_count++;
        }
        else if (key == "fullscreen") {
            fullscreen = (std::stoi(value) != 0);
            loaded_count++;
        }
        else if (key == "game_speed") {
            game_speed = std::stof(value);
            loaded_count++;
        }
        else if (key == "debug_mode") {
            debug_mode = (std::stoi(value) != 0);
            loaded_count++;
        }
        else if (key == "god_mode") {
            god_mode = (std::stoi(value) != 0);
            loaded_count++;
        }
    }
    
    file.close();
    
    Logger::info(Logger::SYS, "GameSettings: Loaded " + std::to_string(loaded_count) 
                 + " settings from " + filename);
    return true;
}

void GameSettings::reset_to_defaults()
{
    *this = GameSettings();
    Logger::info(Logger::SYS, "GameSettings: Reset to default values");
}