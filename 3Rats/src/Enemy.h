#pragma once
#include "Body.h"
#include "Tile.h"
#include "Item.h"
#include <vector>

class Topography;
class Acteur;

class Enemy : public Body {
    Topography* topography;
    int   home_map_id;
    float move_speed;
    float pos_x, pos_y;   // float position for smooth movement
    float damage_timer;

    // HP system
    int hp;
    int max_hp;
    bool is_dead;

    // Drop system
    std::vector<ItemType> drop_table;

    static constexpr float DAMAGE_INTERVAL = 1.0f;
    static constexpr int   DAMAGE_AMOUNT   = 5;
    static constexpr float CONTACT_RADIUS  = 48.0f;

    bool overlaps_wall(Tile* tiles, int count);
    void on_death(Item* item_array, int item_count);

public:
    Enemy();
    void set_topography(Topography* t);
    void set_home_map(int map_id);
    void set_drop_table(std::vector<ItemType> drops);
    void update(float delta, Acteur* rats, int rat_count);

    // HP management
    void take_damage(int amount);
    bool is_alive() const { return !is_dead; }
    int get_hp() const { return hp; }
    int get_max_hp() const { return max_hp; }
};
