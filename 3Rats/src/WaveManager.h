#pragma once

#include "Enemy.h"
#include "Map.h"
#include <vector>
#include <functional>

class WaveNotification;

class WaveManager
{
private:
    float spawn_timer;
    int current_wave;
    int enemies_spawned_this_wave;
    int max_enemies_per_wave;
    
    static constexpr float SPAWN_INTERVAL = 15.0f;  // 15 seconds between spawns
    static constexpr float WAVE_INTERVAL = 45.0f;   // 45 seconds between waves
    
    WaveNotification* notification_system;

public:
    WaveManager();
    void update(float delta, Enemy* enemy_array, int max_enemies, Map* map_array, int map_count);
    bool spawn_enemy(Enemy* enemy_array, int max_enemies, Map* map_array, int map_count);
    int get_current_wave() const { return current_wave; }
    int get_enemies_spawned() const { return enemies_spawned_this_wave; }
    int get_max_enemies_per_wave() const { return max_enemies_per_wave; }
    std::string get_wave_info() const;
    void force_next_wave(Enemy* enemy_array, int max_enemies, Map* map_array, int map_count);
    void set_notification_system(WaveNotification* notifications);
    void reset();
};