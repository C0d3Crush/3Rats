#include "Clock.h"

// the code from here is modified frome this stack echange
// https://stackoverflow.com/a/22889483/18288994


Clock::Clock()
{
	update_time = true;
	is_running = true;
	day_counter = 0;
	day = true;

}

Clock::~Clock()
{
}

void Clock::update(double delta)
{
	count += delta;

	//std::cout << count << std::endl;

	if (count >= 5)
	{
		count = 0;
		min++;
		update_time = true;
	}

	if (min >= 60)
	{
		min = 0;
		hour++;
		update_time = true;
	}

	if (hour >= 24)
	{
		hour = 0;
		update_time = true;
	}

	str_time_min = std::to_string((int)min);
	str_time_hour = std::to_string((int)hour);

	if (str_time_min.size() == 1)
	{
		str_time_min = "0" + str_time_min;
	}

	if (str_time_hour.size() == 1)
	{
		str_time_hour = "0" + str_time_hour;
	}

	time = str_time_hour + ":" + str_time_min;

	// befor i made it with this update flag memory was to big to fast. 
	// alternative idea: somehow free space like in the ~clock.

	if (update_time)
	{
		text->update(time);
		update_time = false;
	}

	if (hour == 16 && min == 35 && day && !1)
	{
		day_counter++;
		std::cout << "day# " << day_counter << " passed. its time to sleep now!" << std::endl;
		// is player back at base?
		is_running = false;
		day = false;
	}
}

void Clock::draw(SDL_Renderer* renderTarget)
{
	(*clockframe).draw(renderTarget);
	text->draw(renderTarget);
}

void Clock::set_body(Body* clock_frame_ptr)
{
	this->clockframe = clock_frame_ptr;
}

void Clock::set_text(Text* text_time)
{
	this->text = text_time;
}

void Clock::set_time(int hour, int min)
{
	count = 0;
	this->min = min;
	this->hour = hour;
}
