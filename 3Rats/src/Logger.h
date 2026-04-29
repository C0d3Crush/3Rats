#pragma once
#include <string>
#include <fstream>
#include <map>

class Logger
{
public:
    enum Level   { DEBUG, INFO, WARN, ERR };

    enum Channel {
        SYS,         // logs/system/init.log     — startup, core systems
        TOPOLOGY,    // logs/generation/topology.log
        MAP,         // logs/generation/map.log
        ACTEUR,      // logs/gameplay/acteur.log
        ITEMS,       // logs/gameplay/items.log
        SCRIPT,      // logs/scripting/script.log — script execution and events
        SCRIPT_EXEC, // logs/scripting/execution.log — script execution flow
        SCRIPT_CMD,  // logs/scripting/commands.log — console commands from scripts
        SCRIPT_VAR,  // logs/scripting/variables.log — variable operations
        SDL_INIT,    // logs/system/sdl_init.log — SDL initialization, window creation
        SDL_RENDER,  // logs/system/rendering.log — frame rendering, texture loading
        SDL_EVENT,   // logs/system/events.log — input events, window events
        GAME_STATE,  // logs/gameplay/game_state.log — win/lose conditions, state changes
        GAME_TIMING, // logs/system/timing.log — frame rate, game loop timing
        GAME_RULES,  // logs/gameplay/rules.log — rule violations, safety checks
        ASSETS,      // logs/system/assets.log — texture loading, font loading, resources
    };

    static void init(const std::string& base_dir = "../logs");
    static void shutdown();

    static void log  (Channel ch, Level lv, const std::string& msg);
    static void debug(Channel ch, const std::string& msg) { log(ch, DEBUG, msg); }
    static void info (Channel ch, const std::string& msg) { log(ch, INFO,  msg); }
    static void warn (Channel ch, const std::string& msg) { log(ch, WARN,  msg); }
    static void error(Channel ch, const std::string& msg) { log(ch, ERR,   msg); }

private:
    static std::map<Channel, std::ofstream> streams;
    static std::ofstream                    error_stream; // mirrors all WARN+ERR
    static bool                             ready;

    static std::string  timestamp();
    static const char*  level_str  (Level   lv);
    static const char*  channel_str(Channel ch);
};
