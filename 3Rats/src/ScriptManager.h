#pragma once

#include "ScriptExecutor.h"
#include <string>
#include <vector>

struct ScriptInfo {
    std::string filename;
    std::string filepath;
    std::string category;  // debug, scenarios, utils, etc.
    std::string description;
    std::vector<std::string> content;
    
    ScriptInfo(const std::string& file, const std::string& path, const std::string& cat)
        : filename(file), filepath(path), category(cat) {}
};

class Console;

class ScriptManager {
private:
    std::vector<ScriptInfo> discovered_scripts;
    ScriptExecutor script_executor;
    std::string scripts_directory;
    
    // File operations
    bool load_script_file(const std::string& filepath, std::vector<std::string>& content);
    void discover_scripts_in_directory(const std::string& directory, const std::string& category = "");
    std::string extract_description(const std::vector<std::string>& content);
    
    // Path utilities
    std::string resolve_script_path(const std::string& script_name);
    bool file_exists(const std::string& filepath);
    std::vector<std::string> list_directory(const std::string& directory);
    
public:
    ScriptManager(Console* console);
    ~ScriptManager();
    
    // Script discovery and management
    void discover_all_scripts();
    std::vector<ScriptInfo> get_available_scripts() const;
    ScriptInfo* find_script_by_name(const std::string& name);
    
    // Script execution
    bool execute_script(const std::string& script_name);
    bool execute_script_file(const std::string& filepath);
    bool execute_script_content(const std::vector<std::string>& content, const std::string& name);
    
    // Variable access
    std::unordered_map<std::string, std::string> get_script_variables() const;
    void clear_script_variables();
    
    // Execution control
    void stop_current_script();
    bool is_script_running() const;
    
    // Configuration
    void set_scripts_directory(const std::string& directory);
    std::string get_scripts_directory() const { return scripts_directory; }
    
    // Error handling
    std::string get_last_error() const;
};