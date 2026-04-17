#pragma once
#include "Body.h"
#include "Tile.h"

class Topography;
class Acteur;

class Enemy : public Body {
    Topography* topography;
    int   home_map_id;
    float move_speed;
    float pos_x, pos_y;   // float position for smooth movement
    float damage_timer;

    static constexpr float DAMAGE_INTERVAL = 1.0f;
    static constexpr int   DAMAGE_AMOUNT   = 5;
    static constexpr float CONTACT_RADIUS  = 48.0f;

    bool overlaps_wall(Tile* tiles, int count);

public:
    Enemy();
    void set_topography(Topography* t);
    void set_home_map(int map_id);
    void update(float delta, Acteur* rats, int rat_count);
};
