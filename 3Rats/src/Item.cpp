#include "Item.h"

Item::Item()
{
	is_on_map = false;
	is_picked_up = false;
	is_consumable = true;  // default to consumable
	effect = ItemEffect(ItemType::FOOD, 0.0f, 0.0f, false);  // default food effect
}

Item::~Item()
{
	SDL_DestroyTexture(texture);
}
	
//void Item::Update(float delta) { }

void Item::Draw(SDL_Renderer* renderTarget)
{
	SDL_RenderCopy(renderTarget, texture, &crop_rect, &position_rect);
}

bool Item::get_on_map() { return is_on_map; }

void Item::set_on_map(bool value) { is_on_map = value; }

void Item::set_pick_up(bool value)
{
	is_picked_up = value;
}

bool Item::get_pick_up()
{
	return is_picked_up;
}

bool Item::get_consumable()
{
	return is_consumable;
}

void Item::set_consumable(bool value)
{
	is_consumable = value;
}

ItemEffect Item::get_effect()
{
	return effect;
}

void Item::set_effect(ItemEffect new_effect)
{
	effect = new_effect;
}

