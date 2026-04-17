#include "Enemy.h"
#include "Topography.h"
#include "Acteur.h"
#include "Logger.h"
#include <cmath>

Enemy::Enemy()
    : topography(nullptr), home_map_id(-1),
      move_speed(110.0f), pos_x(0.0f), pos_y(0.0f), damage_timer(0.0f) {}

void Enemy::set_topography(Topography* t) { topography = t; }

void Enemy::set_home_map(int map_id)
{
    home_map_id = map_id;
    pos_x = (float)position_rect.x;
    pos_y = (float)position_rect.y;
    Logger::info(Logger::SYS, "enemy home set to map #" + std::to_string(map_id));
}

bool Enemy::overlaps_wall(Tile* tiles, int count)
{
    for (int i = 0; i < count; i++) {
        if (tiles[i].get_hight() != 1) continue;
        float dx = (float)(get_origin_x() - tiles[i].get_origin_x());
        float dy = (float)(get_origin_y() - tiles[i].get_origin_y());
        if (std::sqrt(dx*dx + dy*dy) < (float)(radius + tiles[i].get_radius()))
            return true;
    }
    return false;
}

void Enemy::update(float delta, Acteur* rats, int rat_count)
{
    if (!topography) return;

    if (topography->get_current_map_id() != home_map_id) {
        position_rect.x = -1000;
        return;
    }

    // Restore float position after possible off-screen frame
    position_rect.x = (int)pos_x;
    position_rect.y = (int)pos_y;

    Tile* tiles      = topography->get_tile_array();
    int   tile_count = topography->get_tile_size();

    float dx   = (float)(rats[0].get_origin_x() - get_origin_x());
    float dy   = (float)(rats[0].get_origin_y() - get_origin_y());
    float dist = std::sqrt(dx*dx + dy*dy);

    if (dist > 4.0f) {
        float nx = dx / dist;
        float ny = dy / dist;

        // X step
        pos_x += nx * move_speed * delta;
        position_rect.x = (int)pos_x;
        if (overlaps_wall(tiles, tile_count)) {
            pos_x -= nx * move_speed * delta;
            position_rect.x = (int)pos_x;
        }

        // Y step
        pos_y += ny * move_speed * delta;
        position_rect.y = (int)pos_y;
        if (overlaps_wall(tiles, tile_count)) {
            pos_y -= ny * move_speed * delta;
            position_rect.y = (int)pos_y;
        }
    }

    // Tick damage to any rat within contact radius
    damage_timer += delta;
    if (damage_timer >= DAMAGE_INTERVAL) {
        damage_timer = 0.0f;
        int my_x = get_origin_x();
        int my_y = get_origin_y();
        for (int i = 0; i < rat_count; i++) {
            float ddx = (float)(rats[i].get_origin_x() - my_x);
            float ddy = (float)(rats[i].get_origin_y() - my_y);
            if (std::sqrt(ddx*ddx + ddy*ddy) < CONTACT_RADIUS) {
                rats[i].reduce_saturation(DAMAGE_AMOUNT);
                Logger::info(Logger::ACTEUR, "enemy hit rat " + std::to_string(i)
                    + " — saturation now " + std::to_string(rats[i].get_saturation()));
            }
        }
    }
}
