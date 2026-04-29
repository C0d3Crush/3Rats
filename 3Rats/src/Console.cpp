#include "Console.h"
#include "WaveManager.h"
#include "Enemy.h"
#include "TimeManager.h"
#include <sstream>
#include <iostream>

static const int CONSOLE_H     = 180;
static const int MAX_HISTORY   = 7;
static const int LINE_H        = 18;
static const int PADDING       = 6;

Console::Console()
    : is_open(false), font(nullptr), renderer(nullptr),
      player_array(nullptr), player_amount(0),
      map_array(nullptr),   map_amount(0),
      topography(nullptr),
      item_array(nullptr),  item_amount(0),
      tile_array(nullptr),  tile_amount(0),
      wave_manager(nullptr), enemy_array(nullptr), max_enemies(0), time_manager(nullptr)
{}

Console::~Console()
{
    if (font) TTF_CloseFont(font);
}

void Console::init(SDL_Renderer* rend,
                   Acteur* players, int p_amount,
                   Map*    maps,    int m_amount,
                   Topography* topo,
                   Item*   items,   int i_amount,
                   Tile*   tiles,   int t_amount,
                   WaveManager* wave_mgr,
                   Enemy*  enemies, int max_enemy_count,
                   TimeManager* time_mgr)
{
    renderer     = rend;
    player_array = players;  player_amount = p_amount;
    map_array    = maps;     map_amount    = m_amount;
    topography   = topo;
    item_array   = items;    item_amount   = i_amount;
    tile_array   = tiles;    tile_amount   = t_amount;
    wave_manager = wave_mgr;
    enemy_array  = enemies;  max_enemies   = max_enemy_count;
    time_manager = time_mgr;

    font = TTF_OpenFont("../fonts/sans.ttf", 13);
    if (!font) std::cerr << "Console: failed to open font: " << TTF_GetError() << std::endl;

    setup_commands();
    log("console ready. type 'help' for commands.");
}

void Console::log(const std::string& msg)
{
    history.push_back(msg);
    if ((int)history.size() > MAX_HISTORY)
        history.erase(history.begin());
}

void Console::handle_event(const SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN)
    {
        if (e.key.keysym.scancode == SDL_SCANCODE_GRAVE)
        {
            is_open = !is_open;
            if (is_open)  SDL_StartTextInput();
            else          SDL_StopTextInput();
            return;
        }

        if (!is_open) return;

        if (e.key.keysym.sym == SDLK_RETURN)
        {
            if (!input.empty())
            {
                log("> " + input);
                execute(input);
                input.clear();
            }
        }
        else if (e.key.keysym.sym == SDLK_BACKSPACE && !input.empty())
        {
            input.pop_back();
        }
    }
    else if (e.type == SDL_TEXTINPUT && is_open)
    {
        // ignore the backtick that opened the console
        if (std::string(e.text.text) != "`")
            input += e.text.text;
    }
}

std::vector<std::string> Console::tokenize(const std::string& cmd)
{
    std::vector<std::string> tokens;
    std::istringstream ss(cmd);
    std::string token;
    while (ss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

void Console::register_command(const std::string& name, const CommandInfo& info)
{
    command_map[name] = info;
    // Register aliases
    for (const auto& alias : info.aliases) {
        command_map[alias] = info;
    }
}

void Console::execute(const std::string& cmd)
{
    if (cmd.empty()) return;
    
    auto tokens = tokenize(cmd);
    if (tokens.empty()) return;
    
    std::string command_name = tokens[0];
    auto it = command_map.find(command_name);
    
    if (it != command_map.end()) {
        try {
            it->second.handler(tokens);
        } catch (const std::exception& e) {
            log("Error executing command: " + std::string(e.what()));
        }
    } else {
        log("unknown command: " + command_name + ". type 'help' for commands.");
    }
}

void Console::setup_commands()
{
    // Register all commands with their handlers
    register_command("help", {
        [this](const std::vector<std::string>& args) { cmd_help(args); },
        "show all available commands",
        "help [command]",
        {"h", "?"}
    });
    
    register_command("tp", {
        [this](const std::vector<std::string>& args) { cmd_teleport(args); },
        "teleport rat to tile",
        "tp <x> <y>",
        {"teleport"}
    });
    
    register_command("map", {
        [this](const std::vector<std::string>& args) { cmd_map(args); },
        "jump to map id",
        "map <id>",
        {}
    });
    
    register_command("spawn", {
        [this](const std::vector<std::string>& args) { cmd_spawn_item(args); },
        "spawn item at location or for rat",
        "spawn item <type> [rat_id] [x y]",
        {"give", "item"}
    });
    
    register_command("speed", {
        [this](const std::vector<std::string>& args) { cmd_speed(args); },
        "set rat 0 move speed",
        "speed <value>",
        {}
    });
    
    register_command("pos", {
        [this](const std::vector<std::string>& args) { cmd_position(args); },
        "print rat 0 position",
        "pos",
        {"position"}
    });
    
    register_command("regen", {
        [this](const std::vector<std::string>& args) { cmd_regen(args); },
        "regenerate current map",
        "regen <0|1|2> (0=maze 1=garden 2=cage)",
        {"regenerate"}
    });
    
    register_command("wave", {
        [this](const std::vector<std::string>& args) { cmd_wave(args); },
        "wave management commands",
        "wave <spawn|info>",
        {}
    });
    
    register_command("time", {
        [this](const std::vector<std::string>& args) { cmd_time(args); },
        "time management commands",
        "time <set|info|phase|advance|speed|freeze>",
        {}
    });
    
    register_command("actor", {
        [this](const std::vector<std::string>& args) { cmd_actor(args); },
        "actor manipulation commands",
        "actor <health|teleport|effects> <rat_id> [args...]",
        {"rat"}
    });
    
    register_command("world", {
        [this](const std::vector<std::string>& args) { cmd_world(args); },
        "world manipulation commands",  
        "world <seed|weather|lighting> [args...]",
        {}
    });
}

void Console::draw_rect_alpha(int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void Console::draw_text(const std::string& text, int x, int y, SDL_Color colour)
{
    if (!font || text.empty()) return;
    SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), colour);
    if (!surf) return;
    SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!tex) return;

    int tw, th;
    SDL_QueryTexture(tex, nullptr, nullptr, &tw, &th);
    SDL_Rect dst = {x, y, tw, th};
    SDL_RenderCopy(renderer, tex, nullptr, &dst);
    SDL_DestroyTexture(tex);
}

void Console::draw()
{
    if (!is_open) return;

    int screen_w = 600;
    int screen_h = 420;
    int cy = screen_h - CONSOLE_H;

    // background
    draw_rect_alpha(0, cy, screen_w, CONSOLE_H, 0, 0, 0, 200);
    // top border
    SDL_SetRenderDrawColor(renderer, 80, 200, 80, 255);
    SDL_RenderDrawLine(renderer, 0, cy, screen_w, cy);

    SDL_Color white  = {255, 255, 255, 255};
    SDL_Color green  = {80,  200, 80,  255};
    SDL_Color grey   = {180, 180, 180, 255};

    // history lines
    int line_y = cy + PADDING;
    for (const auto& line : history)
    {
        draw_text(line, PADDING, line_y, grey);
        line_y += LINE_H;
    }

    // input line
    int input_y = cy + CONSOLE_H - LINE_H - PADDING;
    draw_rect_alpha(0, input_y - 2, screen_w, LINE_H + 4, 20, 20, 20, 220);
    draw_text("> " + input + "_", PADDING, input_y, green);
}

// ===== COMMAND HANDLERS =====

void Console::cmd_help(const std::vector<std::string>& args)
{
    if (args.size() > 1) {
        // Show specific command help
        auto it = command_map.find(args[1]);
        if (it != command_map.end()) {
            log("Command: " + args[1]);
            log("Description: " + it->second.description);
            log("Usage: " + it->second.usage);
            if (!it->second.aliases.empty()) {
                std::string alias_str = "Aliases: ";
                for (size_t i = 0; i < it->second.aliases.size(); ++i) {
                    if (i > 0) alias_str += ", ";
                    alias_str += it->second.aliases[i];
                }
                log(alias_str);
            }
        } else {
            log("Unknown command: " + args[1]);
        }
    } else {
        // Show all commands
        log("Available commands:");
        for (const auto& [name, info] : command_map) {
            // Only show primary commands, not aliases
            bool is_alias = false;
            for (const auto& [other_name, other_info] : command_map) {
                if (other_name != name && &other_info == &info) {
                    is_alias = true;
                    break;
                }
            }
            if (!is_alias) {
                log("  " + name + " - " + info.description);
            }
        }
        log("Type 'help <command>' for detailed usage");
    }
}

void Console::cmd_teleport(const std::vector<std::string>& args)
{
    if (args.size() < 3) {
        log("usage: tp <tile_x> <tile_y>");
        return;
    }
    
    try {
        int tx = std::stoi(args[1]);
        int ty = std::stoi(args[2]);
        player_array[0].set_cords(tx * 64, ty * 64);
        log("teleported to tile " + std::to_string(tx) + "," + std::to_string(ty));
    } catch (const std::exception& e) {
        log("invalid coordinates: " + std::string(e.what()));
    }
}

void Console::cmd_map(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        log("usage: map <id>");
        return;
    }
    
    try {
        int id = std::stoi(args[1]);
        if (id >= 0 && id < map_amount) {
            topography->set_current_map_id(id);
            map_array[id].set_textures();
            log("switched to map " + std::to_string(id));
        } else {
            log("map id out of range (0-" + std::to_string(map_amount - 1) + ")");
        }
    } catch (const std::exception& e) {
        log("invalid map id: " + std::string(e.what()));
    }
}

void Console::cmd_spawn_item(const std::vector<std::string>& args)
{
    if (args.size() < 3) {
        log("usage: spawn item <type> [rat_id] [x y]");
        log("types: food, speed, shield, damage");
        return;
    }
    
    std::string item_type = args[2];
    
    // Create ItemEffect based on type
    ItemEffect effect;
    std::string type_name;
    
    if (item_type == "food") {
        effect = ItemEffect(ItemType::FOOD, 50.0f, 0.0f, true);
        type_name = "food";
    } else if (item_type == "speed") {
        effect = ItemEffect(ItemType::SPEED_BOOST, 100.0f, 5.0f, false);
        type_name = "speed boost";
    } else if (item_type == "shield") {
        effect = ItemEffect(ItemType::SHIELD, 1.0f, 10.0f, false);
        type_name = "shield";
    } else if (item_type == "damage") {
        effect = ItemEffect(ItemType::DAMAGE_ORB, 25.0f, 0.0f, true);
        type_name = "damage orb";
    } else {
        log("unknown item type: " + item_type);
        log("available types: food, speed, shield, damage");
        return;
    }
    
    // Determine target rat (default to rat 0)
    int rat_id = 0;
    if (args.size() > 3) {
        try {
            rat_id = std::stoi(args[3]);
            if (rat_id < 0 || rat_id >= player_amount) {
                log("invalid rat id: " + args[3] + " (valid: 0-" + std::to_string(player_amount - 1) + ")");
                return;
            }
        } catch (const std::exception& e) {
            log("invalid rat id: " + std::string(e.what()));
            return;
        }
    }
    
    // Check if coordinates are provided for world spawn
    if (args.size() >= 6) {
        try {
            int x = std::stoi(args[4]);
            int y = std::stoi(args[5]);
            
            // Find an available item slot in the world
            for (int i = 0; i < item_amount; i++) {
                if (!item_array[i].get_on_map()) {
                    item_array[i].set_effect(effect);
                    item_array[i].set_on_map(true);
                    item_array[i].set_cords(x * 64, y * 64);  // Convert tile coords to pixel coords
                    log("spawned " + type_name + " at tile (" + std::to_string(x) + "," + std::to_string(y) + ")");
                    return;
                }
            }
            log("no available item slots on map");
        } catch (const std::exception& e) {
            log("invalid coordinates: " + std::string(e.what()));
        }
    } else {
        // Give item directly to rat (old behavior)
        bool success = false;
        
        // Use the existing debug_give_item for food compatibility
        if (item_type == "food") {
            player_array[rat_id].debug_give_item(item_array, item_amount);
            success = true;
        } else {
            // For other items, try to find an available item and give it to the rat
            for (int i = 0; i < item_amount; i++) {
                if (!item_array[i].get_on_map()) {
                    item_array[i].set_effect(effect);
                    // This might need a different approach - the current system may not support
                    // directly giving items to rats. For now, spawn near the rat.
                    int rat_x = player_array[rat_id].get_origin_x();
                    int rat_y = player_array[rat_id].get_origin_y();
                    item_array[i].set_on_map(true);
                    item_array[i].set_cords(rat_x + 32, rat_y + 32);  // Slightly offset from rat
                    success = true;
                    break;
                }
            }
        }
        
        if (success) {
            log("spawned " + type_name + " for rat " + std::to_string(rat_id));
        } else {
            log("failed to spawn item - no available slots");
        }
    }
}

void Console::cmd_speed(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        log("usage: speed <value>");
        return;
    }
    
    try {
        float val = std::stof(args[1]);
        player_array[0].set_move_speed(val);
        log("speed set to " + std::to_string((int)val));
    } catch (const std::exception& e) {
        log("invalid speed value: " + std::string(e.what()));
    }
}

void Console::cmd_position(const std::vector<std::string>& args)
{
    int ox = player_array[0].get_origin_x();
    int oy = player_array[0].get_origin_y();
    log("rat0 px=" + std::to_string(ox) + " py=" + std::to_string(oy)
        + " tile=" + std::to_string(ox / 64) + "," + std::to_string(oy / 64));
}

void Console::cmd_regen(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        log("usage: regen <0|1|2>");
        return;
    }
    
    try {
        int type = std::stoi(args[1]);
        int id = topography->get_current_map_id();
        map_array[id].set_type(type);
        map_array[id].set_textures();
        log("regenerated map " + std::to_string(id) + " type " + std::to_string(type));
    } catch (const std::exception& e) {
        log("invalid map type: " + std::string(e.what()));
    }
}

void Console::cmd_wave(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        log("usage: wave <spawn|info>");
        return;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "spawn") {
        if (wave_manager && enemy_array) {
            wave_manager->force_next_wave(enemy_array, max_enemies, map_array, map_amount);
            log("forced next wave: " + wave_manager->get_wave_info());
        } else {
            log("wave manager not initialized");
        }
    } else if (subcommand == "info") {
        if (wave_manager) {
            log(wave_manager->get_wave_info());
        } else {
            log("wave manager not initialized");
        }
    } else {
        log("usage: wave <spawn|info>");
    }
}

void Console::cmd_time(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        log("usage: time <set|info|phase|advance|speed|freeze>");
        return;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "set") {
        if (args.size() < 4) {
            log("usage: time set <hour> <minute>");
            return;
        }
        try {
            int hour = std::stoi(args[2]);
            int minute = std::stoi(args[3]);
            if (time_manager) {
                time_manager->set_time(hour, minute);
                log("time set to " + time_manager->get_time_string() + 
                    " (" + time_manager->get_phase_string() + ")");
            } else {
                log("time manager not initialized");
            }
        } catch (const std::exception& e) {
            log("invalid time values: " + std::string(e.what()));
        }
    } else if (subcommand == "info") {
        if (time_manager) {
            log("Current time: " + time_manager->get_time_string());
            log("Phase: " + time_manager->get_phase_string());
            log("Day: " + std::to_string(time_manager->get_day_number()));
            log("Difficulty: x" + std::to_string(time_manager->get_difficulty_multiplier()));
        } else {
            log("time manager not initialized");
        }
    } else if (subcommand == "phase") {
        if (args.size() < 3) {
            log("usage: time phase <day|dusk|night|dawn>");
            return;
        }
        std::string phase_str = args[2];
        if (time_manager) {
            TimePhase phase = TimePhase::DAY;
            if (phase_str == "day") phase = TimePhase::DAY;
            else if (phase_str == "dusk") phase = TimePhase::DUSK;
            else if (phase_str == "night") phase = TimePhase::NIGHT;
            else if (phase_str == "dawn") phase = TimePhase::DAWN;
            else {
                log("invalid phase. use: day, dusk, night, dawn");
                return;
            }
            
            time_manager->force_phase(phase);
            log("forced phase to " + time_manager->get_phase_string());
        } else {
            log("time manager not initialized");
        }
    } else if (subcommand == "advance") {
        if (time_manager) {
            int old_day = time_manager->get_day_number();
            time_manager->advance_to_next_day();
            log("advanced from day " + std::to_string(old_day) + 
                " to day " + std::to_string(time_manager->get_day_number()));
        } else {
            log("time manager not initialized");
        }
    } else if (subcommand == "speed") {
        if (args.size() < 3) {
            if (time_manager) {
                log("Current time speed: " + std::to_string(time_manager->get_time_scale()) + "x");
                log("Frozen: " + std::string(time_manager->is_time_frozen() ? "yes" : "no"));
            }
            log("usage: time speed <multiplier>");
            log("multiplier: 0.1-10.0 (1.0 = normal speed)");
            return;
        }
        
        try {
            float speed = std::stof(args[2]);
            if (time_manager) {
                time_manager->set_time_scale(speed);
                log("time speed set to " + std::to_string(speed) + "x");
            } else {
                log("time manager not initialized");
            }
        } catch (const std::exception& e) {
            log("invalid speed value: " + std::string(e.what()));
        }
    } else if (subcommand == "freeze") {
        if (time_manager) {
            if (time_manager->is_time_frozen()) {
                time_manager->unfreeze_time();
                log("time unfrozen - resuming at " + std::to_string(time_manager->get_time_scale()) + "x speed");
            } else {
                time_manager->freeze_time();
                log("time frozen at " + time_manager->get_time_string());
            }
        } else {
            log("time manager not initialized");
        }
    } else {
        log("usage: time <set|info|phase|advance|speed|freeze>");
    }
}

void Console::cmd_actor(const std::vector<std::string>& args)
{
    if (args.size() < 3) {
        log("usage: actor <health|teleport|effects|speed> <rat_id> [args...]");
        return;
    }
    
    std::string subcommand = args[1];
    
    // Parse rat ID
    int rat_id;
    try {
        rat_id = std::stoi(args[2]);
        if (rat_id < 0 || rat_id >= player_amount) {
            log("invalid rat id: " + args[2] + " (valid: 0-" + std::to_string(player_amount - 1) + ")");
            return;
        }
    } catch (const std::exception& e) {
        log("invalid rat id: " + std::string(e.what()));
        return;
    }
    
    if (subcommand == "health") {
        if (args.size() < 4) {
            int current_health = player_array[rat_id].get_saturation();
            log("rat " + std::to_string(rat_id) + " health: " + std::to_string(current_health) + "%");
            log("usage: actor health <rat_id> <value>");
            return;
        }
        
        try {
            int new_health = std::stoi(args[3]);
            if (new_health < 0) new_health = 0;
            if (new_health > 100) new_health = 100;
            
            int current_health = player_array[rat_id].get_saturation();
            int health_change = new_health - current_health;
            
            // Use reduce_saturation with negative value to increase health
            player_array[rat_id].reduce_saturation(-health_change);
            
            log("rat " + std::to_string(rat_id) + " health set to " + std::to_string(new_health) + "%");
        } catch (const std::exception& e) {
            log("invalid health value: " + std::string(e.what()));
        }
    }
    else if (subcommand == "teleport") {
        if (args.size() < 5) {
            log("usage: actor teleport <rat_id> <x> <y>");
            return;
        }
        
        try {
            int x = std::stoi(args[3]);
            int y = std::stoi(args[4]);
            
            player_array[rat_id].set_cords(x * 64, y * 64);
            log("teleported rat " + std::to_string(rat_id) + " to tile (" + 
                std::to_string(x) + "," + std::to_string(y) + ")");
        } catch (const std::exception& e) {
            log("invalid coordinates: " + std::string(e.what()));
        }
    }
    else if (subcommand == "speed") {
        if (args.size() < 4) {
            log("usage: actor speed <rat_id> <value>");
            return;
        }
        
        try {
            float speed = std::stof(args[3]);
            player_array[rat_id].set_move_speed(speed);
            log("rat " + std::to_string(rat_id) + " speed set to " + std::to_string((int)speed));
        } catch (const std::exception& e) {
            log("invalid speed value: " + std::string(e.what()));
        }
    }
    else if (subcommand == "effects") {
        if (args.size() < 4) {
            log("usage: actor effects <rat_id> <list|clear>");
            log("       actor effects <rat_id> add <type> <value> <duration>");
            return;
        }
        
        std::string effect_cmd = args[3];
        if (effect_cmd == "list") {
            log("active effects listing not yet implemented");
        } else if (effect_cmd == "clear") {
            log("effect clearing not yet implemented");
        } else if (effect_cmd == "add") {
            log("effect addition not yet implemented");
        } else {
            log("unknown effects command: " + effect_cmd);
            log("available: list, clear, add");
        }
    }
    else {
        log("unknown actor command: " + subcommand);
        log("available: health, teleport, speed, effects");
    }
}

void Console::cmd_world(const std::vector<std::string>& args)
{
    if (args.size() < 2) {
        log("usage: world <seed|info|lighting> [args...]");
        return;
    }
    
    std::string subcommand = args[1];
    
    if (subcommand == "seed") {
        if (args.size() < 3) {
            log("usage: world seed <number>");
            log("regenerates world with specified seed");
            return;
        }
        
        try {
            uint32_t seed = std::stoul(args[2]);
            log("world regeneration with custom seed not yet implemented");
            log("requested seed: " + std::to_string(seed));
        } catch (const std::exception& e) {
            log("invalid seed: " + std::string(e.what()));
        }
    }
    else if (subcommand == "info") {
        log("Current world info:");
        log("  Total maps: " + std::to_string(map_amount));
        log("  Current map: " + std::to_string(topography->get_current_map_id()));
        if (topography->get_current_map_id() < map_amount) {
            int map_type = map_array[topography->get_current_map_id()].get_type();
            std::string type_name = (map_type == 0) ? "maze" : 
                                  (map_type == 1) ? "garden" : 
                                  (map_type == 2) ? "cage" : "unknown";
            log("  Current map type: " + std::to_string(map_type) + " (" + type_name + ")");
        }
        log("  Total items: " + std::to_string(item_amount));
        log("  Active enemies: checking...");
        
        int active_enemies = 0;
        for (int i = 0; i < max_enemies; i++) {
            if (enemy_array && enemy_array[i].get_is_active()) {
                active_enemies++;
            }
        }
        log("  Active enemies: " + std::to_string(active_enemies) + "/" + std::to_string(max_enemies));
    }
    else if (subcommand == "lighting") {
        if (args.size() < 3) {
            log("usage: world lighting <day|night|custom>");
            log("lighting override not yet implemented");
            return;
        }
        log("lighting control not yet implemented");
        log("requested: " + args[2]);
    }
    else if (subcommand == "weather") {
        log("weather system not yet implemented");
        log("planned: world weather <clear|rain|storm>");
    }
    else {
        log("unknown world command: " + subcommand);
        log("available: seed, info, lighting, weather");
    }
}
