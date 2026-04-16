#pragma once
#include <string>
#include <fstream>
#include <map>

class Logger
{
public:
    enum Level   { DEBUG, INFO, WARN, ERR };

    enum Channel {
        SYS,       // logs/system/init.log     — startup, SDL, seed
        TOPOLOGY,  // logs/generation/topology.log
        MAP,       // logs/generation/map.log
        ACTEUR,    // logs/gameplay/acteur.log
        ITEMS,     // logs/gameplay/items.log
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
