#include "WaveManager.h"
#include "WaveNotification.h"
#include "Logger.h"
#include "Random.h"

WaveManager::WaveManager()
    : spawn_timer(0.0f), current_wave(1), enemies_spawned_this_wave(0), max_enemies_per_wave(1), 
      notification_system(nullptr)
{
    Logger::info(Logger::SYS, "WaveManager initialized - Wave 1 starting");
}

void WaveManager::update(float delta, Enemy* enemy_array, int max_enemies, Map* map_array, int map_count)
{
    spawn_timer += delta;

    // Check if it's time to spawn next enemy
    if (spawn_timer >= SPAWN_INTERVAL && enemies_spawned_this_wave < max_enemies_per_wave)
    {
        bool spawned = spawn_enemy(enemy_array, max_enemies, map_array, map_count);
        if (spawned) {
            spawn_timer = 0.0f;
        } else {
            // If spawn failed due to no slots, wait longer before trying again
            spawn_timer = SPAWN_INTERVAL - 5.0f;  // Retry in 5 seconds
        }
    }

    // Check if wave is complete and time for next wave
    if (enemies_spawned_this_wave >= max_enemies_per_wave && spawn_timer >= WAVE_INTERVAL)
    {
        current_wave++;
        enemies_spawned_this_wave = 0;
        spawn_timer = 0.0f;
        
        // Increase enemies per wave: Wave 1=1, Wave 2=2, Wave 3=3, Wave 4+=3
        max_enemies_per_wave = (current_wave <= 3) ? current_wave : 3;
        
        Logger::info(Logger::SYS, "Wave " + std::to_string(current_wave) 
            + " starting - max enemies: " + std::to_string(max_enemies_per_wave));
            
        // Show wave notification
        if (notification_system) {
            notification_system->show_notification("WAVE " + std::to_string(current_wave) + " BEGINS!");
        }
    }
}

bool WaveManager::spawn_enemy(Enemy* enemy_array, int max_enemies, Map* map_array, int map_count)
{
    // Find available enemy slot
    int available_slot = -1;
    for (int i = 0; i < max_enemies; i++)
    {
        if (!enemy_array[i].get_is_active())
        {
            available_slot = i;
            break;
        }
    }

    if (available_slot == -1)
    {
        // Count active enemies for better debugging
        int active_count = 0;
        for (int i = 0; i < max_enemies; i++) {
            if (enemy_array[i].get_is_active()) active_count++;
        }
        Logger::warn(Logger::SYS, "No available enemy slots for spawning (active: " 
            + std::to_string(active_count) + "/" + std::to_string(max_enemies) + ")");
        return false;
    }

    // Find a random garden room (type 1) for spawning
    std::vector<int> garden_rooms;
    for (int i = 1; i < map_count; i++)
    {
        if (map_array[i].get_type() == 1)
        {
            garden_rooms.push_back(i);
        }
    }

    if (garden_rooms.empty())
    {
        Logger::warn(Logger::SYS, "No garden rooms available for enemy spawning");
        return false;
    }

    Random random;
    int enemy_map_id = garden_rooms[random.roll_custom_dice((int)garden_rooms.size()) - 1];

    // Activate and configure the enemy
    enemy_array[available_slot].set_home_map(enemy_map_id);
    enemy_array[available_slot].set_cords(4 * 64, 2 * 64);  // Default spawn position
    enemy_array[available_slot].activate();

    enemies_spawned_this_wave++;
    
    Logger::info(Logger::SYS, "Enemy spawned in slot " + std::to_string(available_slot)
        + " on map #" + std::to_string(enemy_map_id)
        + " - Wave " + std::to_string(current_wave)
        + " (" + std::to_string(enemies_spawned_this_wave) 
        + "/" + std::to_string(max_enemies_per_wave) + ")");
    
    return true;
}

std::string WaveManager::get_wave_info() const
{
    return "Wave " + std::to_string(current_wave) + " - Enemies: " + 
           std::to_string(enemies_spawned_this_wave) + "/" + std::to_string(max_enemies_per_wave);
}

void WaveManager::force_next_wave(Enemy* enemy_array, int max_enemies, Map* map_array, int map_count)
{
    current_wave++;
    enemies_spawned_this_wave = 0;
    spawn_timer = 0.0f;
    
    // Increase enemies per wave: Wave 1=1, Wave 2=2, Wave 3=3, Wave 4+=3
    max_enemies_per_wave = (current_wave <= 3) ? current_wave : 3;
    
    Logger::info(Logger::SYS, "Wave " + std::to_string(current_wave) 
        + " forced - max enemies: " + std::to_string(max_enemies_per_wave));
    
    // Show wave notification
    if (notification_system) {
        notification_system->show_notification("WAVE " + std::to_string(current_wave) + " BEGINS!");
    }
    
    // Immediately spawn first enemy of the wave
    bool spawned = spawn_enemy(enemy_array, max_enemies, map_array, map_count);
    if (!spawned) {
        Logger::warn(Logger::SYS, "Failed to spawn enemy for forced wave " + std::to_string(current_wave));
    }
}

void WaveManager::set_notification_system(WaveNotification* notifications)
{
    notification_system = notifications;
}

void WaveManager::reset()
{
    spawn_timer = 0.0f;
    current_wave = 1;
    enemies_spawned_this_wave = 0;
    max_enemies_per_wave = 1;
    Logger::info(Logger::SYS, "WaveManager reset to Wave 1");
}