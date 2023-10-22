#include "Sound.h"

void Sound::init()
{
	music = Mix_LoadMUS("music/Electronic_Onslaught_short.wav");
	Mix_PlayMusic(music, -1); // Play music in a loop (-1 for infinite loop)
	Mix_PauseMusic();
}

void Sound::pause()
{
	Mix_PauseMusic();
}

void Sound::play()
{
	Mix_ResumeMusic();
}
