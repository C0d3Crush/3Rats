#pragma once

#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include "Acteur.h"
#include "Map.h"
#include "Topography.h"
#include "Item.h"
#include "Tile.h"

class WaveManager;
class Enemy;
class TimeManager;

struct CommandInfo {
    std::function<void(const std::vector<std::string>&)> handler;
    std::string description;
    std::string usage;
    std::vector<std::string> aliases;
};

class Console
{
private:
    bool is_open;
    std::string input;
    std::vector<std::string> history;
    std::unordered_map<std::string, CommandInfo> command_map;

    TTF_Font* font;
    SDL_Renderer* renderer;

    Acteur*    player_array;
    int        player_amount;
    Map*       map_array;
    int        map_amount;
    Topography* topography;
    Item*      item_array;
    int        item_amount;
    Tile*      tile_array;
    int        tile_amount;
    WaveManager* wave_manager;
    Enemy*     enemy_array;
    int        max_enemies;
    TimeManager* time_manager;

    void execute(const std::string& cmd);
    void setup_commands();
    void register_command(const std::string& name, const CommandInfo& info);
    std::vector<std::string> tokenize(const std::string& cmd);
    void log(const std::string& msg);
    void draw_rect_alpha(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
    void draw_text(const std::string& text, int x, int y, SDL_Color colour);
    
    // Command handlers
    void cmd_help(const std::vector<std::string>& args);
    void cmd_teleport(const std::vector<std::string>& args);
    void cmd_map(const std::vector<std::string>& args);
    void cmd_spawn_item(const std::vector<std::string>& args);
    void cmd_speed(const std::vector<std::string>& args);
    void cmd_position(const std::vector<std::string>& args);
    void cmd_regen(const std::vector<std::string>& args);
    void cmd_wave(const std::vector<std::string>& args);
    void cmd_time(const std::vector<std::string>& args);
    void cmd_actor(const std::vector<std::string>& args);
    void cmd_world(const std::vector<std::string>& args);

public:
    Console();
    ~Console();

    void init(SDL_Renderer* rend,
              Acteur* players, int p_amount,
              Map* maps,       int m_amount,
              Topography* topo,
              Item* items,     int i_amount,
              Tile* tiles,     int t_amount,
              WaveManager* wave_mgr,
              Enemy* enemies,  int max_enemy_count,
              TimeManager* time_mgr);

    void handle_event(const SDL_Event& e);
    void draw();

    bool get_open() const { return is_open; }
};
