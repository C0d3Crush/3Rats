#include "Logger.h"

#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

std::map<Logger::Channel, std::ofstream> Logger::streams;
std::ofstream                            Logger::error_stream;
bool                                     Logger::ready = false;

static struct ChannelMeta {
    const char* folder;
    const char* file;
} CHANNEL_META[] = {
    /* SYS      */ { "system",     "init.log"     },
    /* TOPOLOGY */ { "generation", "topology.log" },
    /* MAP      */ { "generation", "map.log"      },
    /* ACTEUR   */ { "gameplay",   "acteur.log"   },
    /* ITEMS    */ { "gameplay",   "items.log"    },
};

void Logger::init(const std::string& base_dir)
{
    namespace fs = std::filesystem;

    // Create every sub-folder needed
    for (auto& m : CHANNEL_META)
    {
        fs::path dir = fs::path(base_dir) / m.folder;
        fs::create_directories(dir);
    }
    fs::create_directories(fs::path(base_dir) / "system");

    // Open one stream per channel
    for (int i = SYS; i <= ITEMS; i++)
    {
        Channel ch = static_cast<Channel>(i);
        fs::path path = fs::path(base_dir) / CHANNEL_META[i].folder / CHANNEL_META[i].file;
        streams[ch].open(path, std::ios::out | std::ios::trunc);
    }

    // Shared errors file
    fs::path err_path = fs::path(base_dir) / "system" / "errors.log";
    error_stream.open(err_path, std::ios::out | std::ios::trunc);

    ready = true;
    info(SYS, "Logger initialised — base: " + base_dir);
}

void Logger::shutdown()
{
    if (!ready) return;
    info(SYS, "Logger shutting down.");
    for (auto& [ch, fs] : streams) fs.close();
    error_stream.close();
    ready = false;
}

void Logger::log(Channel ch, Level lv, const std::string& msg)
{
    std::string line = "[" + timestamp() + "] ["
                     + level_str(lv)   + "] ["
                     + channel_str(ch) + "] "
                     + msg + "\n";

    // Write to the channel's own log file
    if (ready && streams.count(ch))
        streams[ch] << line << std::flush;

    // Mirror WARN and ERR to the shared errors file
    if (ready && (lv == WARN || lv == ERR))
        error_stream << line << std::flush;

    // Print ERR to stderr so serious problems are visible in the console
    if (lv == ERR)
        std::cerr << line;
}

// ── helpers ──────────────────────────────────────────────────────────────────

std::string Logger::timestamp()
{
    using namespace std::chrono;
    auto now   = system_clock::now();
    auto tt    = system_clock::to_time_t(now);
    auto ms    = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    std::ostringstream ss;
    ss << std::put_time(std::localtime(&tt), "%H:%M:%S")
       << "." << std::setw(3) << std::setfill('0') << ms.count();
    return ss.str();
}

const char* Logger::level_str(Level lv)
{
    switch (lv) {
    case DEBUG: return "DEBUG";
    case INFO:  return "INFO ";
    case WARN:  return "WARN ";
    case ERR:   return "ERROR";
    }
    return "?????";
}

const char* Logger::channel_str(Channel ch)
{
    switch (ch) {
    case SYS:      return "SYS     ";
    case TOPOLOGY: return "TOPOLOGY";
    case MAP:      return "MAP     ";
    case ACTEUR:   return "ACTEUR  ";
    case ITEMS:    return "ITEMS   ";
    }
    return "????????";
}
