#include "Pause.h"

void Pause::update(std::string update_text)
{
	this->display_text = update_text;
	Panel::update();
}
