#pragma once

#include <iostream>
#include <string>

#include "Body.h"

enum class ItemType
{
	FOOD,
	SPEED_BOOST,
	SHIELD,
	DAMAGE_ORB
};

struct ItemEffect
{
	ItemType type;
	float value;
	float duration;
	bool is_passive;

	ItemEffect(ItemType t = ItemType::FOOD, float v = 0.0f, float d = 0.0f, bool p = false)
		: type(t), value(v), duration(d), is_passive(p) {}
};

class Item : public Body
{
private:
	bool is_on_map;
	bool is_picked_up;
	bool is_consumable;

	double weight;		// make it that this affacts
						// how fast it can be carried arround
						// maybe: if two rats need
						// to carry it together?

	ItemEffect effect;

public:
	Item();
	~Item();

	//void Update(float delta);
	void Draw(SDL_Renderer* renderTarget);

	bool get_on_map();
	void set_on_map(bool value);

	bool get_pick_up();
	void set_pick_up(bool value);

	bool get_consumable();
	void set_consumable(bool value);

	ItemEffect get_effect();
	void set_effect(ItemEffect new_effect);

};