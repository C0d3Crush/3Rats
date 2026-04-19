#include "Enemy.h"
#include "Topography.h"
#include "Acteur.h"
#include "Logger.h"
#include <cmath>

Enemy::Enemy()
    : topography(nullptr), home_map_id(-1),
      move_speed(110.0f), pos_x(0.0f), pos_y(0.0f), damage_timer(0.0f),
      hp(50), max_hp(50), is_dead(false)
{
	// Default drop table: 70% FOOD, 30% SPEED_BOOST
	drop_table.push_back(ItemType::FOOD);
	drop_table.push_back(ItemType::FOOD);
	drop_table.push_back(ItemType::FOOD);
	drop_table.push_back(ItemType::SPEED_BOOST);
}

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

void Enemy::set_drop_table(std::vector<ItemType> drops)
{
	drop_table = drops;
	Logger::debug(Logger::SYS, "enemy drop table set with "
		+ std::to_string(drops.size()) + " item types");
}

void Enemy::take_damage(int amount)
{
	if (is_dead) return;

	hp -= amount;
	if (hp < 0) hp = 0;

	Logger::info(Logger::ACTEUR, "enemy took " + std::to_string(amount)
		+ " damage — HP now " + std::to_string(hp) + "/" + std::to_string(max_hp)
		+ " on map #" + std::to_string(home_map_id));

	if (hp <= 0)
	{
		on_death(topography->get_item_array(), topography->get_item_size());
	}
}

void Enemy::on_death(Item* item_array, int item_count)
{
	if (is_dead) return;  // Prevent multiple death triggers

	is_dead = true;

	Logger::info(Logger::SYS, "enemy died on map #" + std::to_string(home_map_id)
		+ " at position (" + std::to_string(position_rect.x)
		+ "," + std::to_string(position_rect.y) + ")");

	// Spawn items from drop table
	if (drop_table.empty())
	{
		Logger::warn(Logger::ITEMS, "enemy drop_table is empty — no items spawned");
	}
	else
	{
		// Drop 1-2 random items from drop table
		int drop_count = 1 + (rand() % 2);  // 1 or 2 items

		for (int i = 0; i < drop_count; i++)
		{
			// Find empty item slot
			int item_slot = -1;
			for (int j = 0; j < item_count; j++)
			{
				if (!item_array[j].get_on_map() && !item_array[j].get_pick_up())
				{
					item_slot = j;
					break;
				}
			}

			if (item_slot == -1)
			{
				Logger::warn(Logger::ITEMS, "enemy death: no free item slots for drop #"
					+ std::to_string(i + 1));
				break;
			}

			// Select random item from drop table
			ItemType dropped_type = drop_table[rand() % drop_table.size()];

			// Configure item effect based on type
			ItemEffect drop_effect;
			std::string texture_path;

			switch (dropped_type)
			{
				case ItemType::FOOD:
					drop_effect = ItemEffect(ItemType::FOOD, 30.0f, 0.0f, false);
					texture_path = "../item_textures/mushroom.png";
					break;
				case ItemType::SPEED_BOOST:
					drop_effect = ItemEffect(ItemType::SPEED_BOOST, 100.0f, 5.0f, false);
					texture_path = "../item_textures/mushroom.png";  // TODO: add speed_boost.png
					break;
				case ItemType::SHIELD:
					drop_effect = ItemEffect(ItemType::SHIELD, 1.0f, 10.0f, false);
					texture_path = "../item_textures/mushroom.png";  // TODO: add shield.png
					break;
				case ItemType::DAMAGE_ORB:
					drop_effect = ItemEffect(ItemType::DAMAGE_ORB, 20.0f, 0.0f, true);
					texture_path = "../item_textures/mushroom.png";  // TODO: add damage_orb.png
					break;
			}

			// Spawn item at enemy position
			item_array[item_slot].set_effect(drop_effect);
			item_array[item_slot].set_consumable(true);
			item_array[item_slot].set_on_map(true);
			item_array[item_slot].set_pick_up(false);
			item_array[item_slot].set_texture(texture_path);
			item_array[item_slot].set_cords(position_rect.x, position_rect.y);

			Logger::info(Logger::ITEMS, "enemy dropped item type "
				+ std::to_string(static_cast<int>(dropped_type))
				+ " at slot " + std::to_string(item_slot)
				+ " position (" + std::to_string(position_rect.x)
				+ "," + std::to_string(position_rect.y) + ")");
		}
	}

	// Move enemy far off-screen
	position_rect.x = -10000;
	position_rect.y = -10000;
	pos_x = -10000.0f;
	pos_y = -10000.0f;
}

void Enemy::update(float delta, Acteur* rats, int rat_count)
{
    if (!topography) return;
    if (is_dead) return;  // Skip update if dead

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
