#pragma once

#include <SDL_mixer.h>
#include <string>
#include <map>

class GameSettings;

class Sound
{
private:
	Mix_Music* current_music;
	std::map<std::string, Mix_Music*> music_tracks;
	std::map<std::string, Mix_Chunk*> sound_effects;
	
	bool initialized;
	bool music_enabled;
	bool sfx_enabled;
	float master_volume;
	float music_volume;
	float sfx_volume;
	
	std::string current_track;
	
	void update_volumes();

public:
	Sound();
	~Sound();

	bool init();
	void shutdown();
	
	// Music controls
	bool load_music(const std::string& name, const std::string& file_path);
	bool play_music(const std::string& track_name, bool loop = true);
	void pause_music();
	void resume_music();
	void stop_music();
	void fade_out_music(int ms = 1000);
	void fade_in_music(const std::string& track_name, int ms = 1000, bool loop = true);
	
	// Sound effects
	bool load_sound_effect(const std::string& name, const std::string& file_path);
	void play_sound_effect(const std::string& effect_name, int channel = -1);
	
	// Volume controls (0.0f to 1.0f)
	void set_master_volume(float volume);
	void set_music_volume(float volume);
	void set_sfx_volume(float volume);
	void set_music_enabled(bool enabled);
	void set_sfx_enabled(bool enabled);
	
	// Apply settings from GameSettings
	void apply_settings(const GameSettings& settings);
	
	// Getters
	bool is_music_playing() const;
	bool is_initialized() const { return initialized; }
	float get_master_volume() const { return master_volume; }
	float get_music_volume() const { return music_volume; }
	float get_sfx_volume() const { return sfx_volume; }
	std::string get_current_track() const { return current_track; }
};