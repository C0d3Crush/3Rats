#include "Food.h"

Food::Food()
{
	nutritions = 100;
	// Set up food effect - instant consumption restoring saturation
	set_effect(ItemEffect(ItemType::FOOD, nutritions, 0.0f, false));
	set_consumable(true);
}
