#include "Sound.h"
#include "GameSettings.h"
#include "Logger.h"
#include <algorithm>

Sound::Sound()
	: current_music(nullptr), initialized(false), music_enabled(true), sfx_enabled(true),
	  master_volume(1.0f), music_volume(0.7f), sfx_volume(0.8f), current_track("")
{
}

Sound::~Sound()
{
	shutdown();
}

bool Sound::init()
{
	if (initialized) {
		Logger::warn(Logger::SYS, "Sound: Already initialized");
		return true;
	}

	// Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		Logger::warn(Logger::SYS, "Sound: Failed to initialize SDL_mixer - " + std::string(Mix_GetError()));
		return false;
	}

	// Set initial volumes
	update_volumes();
	
	initialized = true;
	Logger::info(Logger::SYS, "Sound: SDL_mixer initialized successfully");
	
	return true;
}

void Sound::shutdown()
{
	if (!initialized) return;

	// Stop all music and sounds
	stop_music();
	Mix_HaltChannel(-1);

	// Free all music tracks
	for (auto& pair : music_tracks) {
		if (pair.second) {
			Mix_FreeMusic(pair.second);
		}
	}
	music_tracks.clear();

	// Free all sound effects
	for (auto& pair : sound_effects) {
		if (pair.second) {
			Mix_FreeChunk(pair.second);
		}
	}
	sound_effects.clear();

	current_music = nullptr;
	current_track = "";

	Mix_CloseAudio();
	initialized = false;
	
	Logger::info(Logger::SYS, "Sound: SDL_mixer shutdown complete");
}

void Sound::update_volumes()
{
	if (!initialized) return;
	
	// Apply master volume to music and sound effects
	int music_vol = (int)(128 * master_volume * music_volume);
	int sfx_vol = (int)(128 * master_volume * sfx_volume);
	
	Mix_VolumeMusic(music_enabled ? music_vol : 0);
	Mix_Volume(-1, sfx_enabled ? sfx_vol : 0);  // -1 affects all channels
}

bool Sound::load_music(const std::string& name, const std::string& file_path)
{
	if (!initialized) {
		Logger::warn(Logger::SYS, "Sound: Cannot load music - not initialized");
		return false;
	}

	// Check if already loaded
	if (music_tracks.find(name) != music_tracks.end()) {
		Logger::warn(Logger::SYS, "Sound: Music track '" + name + "' already loaded");
		return true;
	}

	Mix_Music* music = Mix_LoadMUS(file_path.c_str());
	if (!music) {
		Logger::warn(Logger::SYS, "Sound: Failed to load music '" + file_path + "' - " + Mix_GetError());
		return false;
	}

	music_tracks[name] = music;
	Logger::info(Logger::SYS, "Sound: Loaded music track '" + name + "' from " + file_path);
	
	return true;
}

bool Sound::play_music(const std::string& track_name, bool loop)
{
	if (!initialized || !music_enabled) return false;

	auto it = music_tracks.find(track_name);
	if (it == music_tracks.end()) {
		Logger::warn(Logger::SYS, "Sound: Music track '" + track_name + "' not found");
		return false;
	}

	// Stop current music if playing
	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	current_music = it->second;
	current_track = track_name;

	int loops = loop ? -1 : 0;  // -1 = infinite loop, 0 = play once
	if (Mix_PlayMusic(current_music, loops) == -1) {
		Logger::warn(Logger::SYS, "Sound: Failed to play music '" + track_name + "' - " + Mix_GetError());
		current_music = nullptr;
		current_track = "";
		return false;
	}

	Logger::info(Logger::SYS, "Sound: Playing music track '" + track_name + "'");
	return true;
}

void Sound::pause_music()
{
	if (!initialized) return;
	Mix_PauseMusic();
}

void Sound::resume_music()
{
	if (!initialized || !music_enabled) return;
	Mix_ResumeMusic();
}

void Sound::stop_music()
{
	if (!initialized) return;
	Mix_HaltMusic();
	current_music = nullptr;
	current_track = "";
}

void Sound::fade_out_music(int ms)
{
	if (!initialized) return;
	Mix_FadeOutMusic(ms);
}

void Sound::fade_in_music(const std::string& track_name, int ms, bool loop)
{
	if (!initialized || !music_enabled) return;

	auto it = music_tracks.find(track_name);
	if (it == music_tracks.end()) {
		Logger::warn(Logger::SYS, "Sound: Music track '" + track_name + "' not found for fade in");
		return;
	}

	// Stop current music if playing
	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	current_music = it->second;
	current_track = track_name;

	int loops = loop ? -1 : 0;
	if (Mix_FadeInMusic(current_music, loops, ms) == -1) {
		Logger::warn(Logger::SYS, "Sound: Failed to fade in music '" + track_name + "' - " + Mix_GetError());
		current_music = nullptr;
		current_track = "";
	} else {
		Logger::info(Logger::SYS, "Sound: Fading in music track '" + track_name + "'");
	}
}

bool Sound::load_sound_effect(const std::string& name, const std::string& file_path)
{
	if (!initialized) {
		Logger::warn(Logger::SYS, "Sound: Cannot load sound effect - not initialized");
		return false;
	}

	// Check if already loaded
	if (sound_effects.find(name) != sound_effects.end()) {
		Logger::warn(Logger::SYS, "Sound: Sound effect '" + name + "' already loaded");
		return true;
	}

	Mix_Chunk* chunk = Mix_LoadWAV(file_path.c_str());
	if (!chunk) {
		Logger::warn(Logger::SYS, "Sound: Failed to load sound effect '" + file_path + "' - " + Mix_GetError());
		return false;
	}

	sound_effects[name] = chunk;
	Logger::info(Logger::SYS, "Sound: Loaded sound effect '" + name + "' from " + file_path);
	
	return true;
}

void Sound::play_sound_effect(const std::string& effect_name, int channel)
{
	if (!initialized || !sfx_enabled) return;

	auto it = sound_effects.find(effect_name);
	if (it == sound_effects.end()) {
		Logger::warn(Logger::SYS, "Sound: Sound effect '" + effect_name + "' not found");
		return;
	}

	if (Mix_PlayChannel(channel, it->second, 0) == -1) {
		Logger::warn(Logger::SYS, "Sound: Failed to play sound effect '" + effect_name + "' - " + Mix_GetError());
	}
}

void Sound::set_master_volume(float volume)
{
	master_volume = std::clamp(volume, 0.0f, 1.0f);
	update_volumes();
}

void Sound::set_music_volume(float volume)
{
	music_volume = std::clamp(volume, 0.0f, 1.0f);
	update_volumes();
}

void Sound::set_sfx_volume(float volume)
{
	sfx_volume = std::clamp(volume, 0.0f, 1.0f);
	update_volumes();
}

void Sound::set_music_enabled(bool enabled)
{
	music_enabled = enabled;
	update_volumes();
	
	if (!enabled && Mix_PlayingMusic()) {
		pause_music();
	} else if (enabled && Mix_PausedMusic()) {
		resume_music();
	}
}

void Sound::set_sfx_enabled(bool enabled)
{
	sfx_enabled = enabled;
	update_volumes();
	
	if (!enabled) {
		Mix_HaltChannel(-1);  // Stop all sound effects
	}
}

void Sound::apply_settings(const GameSettings& settings)
{
	set_master_volume(settings.master_volume);
	set_music_volume(settings.music_volume);
	set_sfx_volume(settings.sfx_volume);
	set_music_enabled(settings.audio_enabled);
	set_sfx_enabled(settings.audio_enabled);
	
	Logger::info(Logger::SYS, "Sound: Applied settings - Master: " 
		+ std::to_string((int)(settings.master_volume * 100)) + "%, Music: "
		+ std::to_string((int)(settings.music_volume * 100)) + "%, SFX: "
		+ std::to_string((int)(settings.sfx_volume * 100)) + "%");
}

bool Sound::is_music_playing() const
{
	return initialized && Mix_PlayingMusic() && !Mix_PausedMusic();
}
