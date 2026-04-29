#include "ScriptManager.h"
#include "Console.h"
#include "Logger.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

ScriptManager::ScriptManager(Console* console)
    : script_executor(console), scripts_directory("../scripts")
{
    Logger::info(Logger::SCRIPT, "ScriptManager initialized");
    discover_all_scripts();
}

ScriptManager::~ScriptManager()
{
    discovered_scripts.clear();
}

void ScriptManager::discover_all_scripts()
{
    discovered_scripts.clear();
    
    if (!file_exists(scripts_directory)) {
        Logger::warn(Logger::SCRIPT, "Scripts directory not found: " + scripts_directory);
        return;
    }
    
    try {
        // Discover scripts in main directory and subdirectories
        discover_scripts_in_directory(scripts_directory);
        discover_scripts_in_directory(scripts_directory + "/debug", "debug");
        discover_scripts_in_directory(scripts_directory + "/scenarios", "scenarios");
        discover_scripts_in_directory(scripts_directory + "/utils", "utils");
        discover_scripts_in_directory(scripts_directory + "/test", "test");
        
        Logger::info(Logger::SCRIPT, "Discovered " + std::to_string(discovered_scripts.size()) + " scripts");
        
        // Log discovered scripts
        for (const auto& script : discovered_scripts) {
            Logger::info(Logger::SCRIPT, "  " + script.category + "/" + script.filename + 
                        (script.description.empty() ? "" : " - " + script.description));
        }
        
    } catch (const std::exception& e) {
        Logger::error(Logger::SCRIPT, "Error discovering scripts: " + std::string(e.what()));
    }
}

void ScriptManager::discover_scripts_in_directory(const std::string& directory, const std::string& category)
{
    if (!file_exists(directory)) {
        return;
    }
    
    try {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filepath = entry.path().string();
                std::string filename = entry.path().filename().string();
                
                // Check if it's a .3rs file
                if (filename.length() > 4 && filename.substr(filename.length() - 4) == ".3rs") {
                    ScriptInfo script_info(filename, filepath, category);
                    
                    // Load content and extract description
                    if (load_script_file(filepath, script_info.content)) {
                        script_info.description = extract_description(script_info.content);
                        discovered_scripts.push_back(script_info);
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        Logger::error(Logger::SCRIPT, "Error reading directory " + directory + ": " + std::string(e.what()));
    }
}

bool ScriptManager::load_script_file(const std::string& filepath, std::vector<std::string>& content)
{
    content.clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Logger::error(Logger::SCRIPT, "Cannot open script file: " + filepath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        content.push_back(line);
    }
    
    file.close();
    return true;
}

std::string ScriptManager::extract_description(const std::vector<std::string>& content)
{
    // Look for description in first few comment lines
    std::string description;
    
    for (size_t i = 0; i < std::min(content.size(), size_t(5)); i++) {
        std::string line = content[i];
        
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        
        // Check if it's a comment line
        if (line.length() > 2 && line.substr(0, 2) == "--") {
            std::string comment = line.substr(2);
            
            // Trim whitespace from comment
            comment.erase(0, comment.find_first_not_of(" \t"));
            comment.erase(comment.find_last_not_of(" \t") + 1);
            
            // Skip filename comments and empty comments
            if (!comment.empty() && comment.find(".3rs") == std::string::npos) {
                description = comment;
                break;
            }
        }
    }
    
    return description;
}

std::vector<ScriptInfo> ScriptManager::get_available_scripts() const
{
    return discovered_scripts;
}

ScriptInfo* ScriptManager::find_script_by_name(const std::string& name)
{
    for (auto& script : discovered_scripts) {
        if (script.filename == name || 
            script.filename == name + ".3rs" ||
            (script.category + "/" + script.filename) == name ||
            (script.category + "/" + script.filename) == name + ".3rs") {
            return &script;
        }
    }
    return nullptr;
}

bool ScriptManager::execute_script(const std::string& script_name)
{
    ScriptInfo* script = find_script_by_name(script_name);
    if (!script) {
        Logger::error(Logger::SCRIPT, "Script not found: " + script_name);
        return false;
    }
    
    return execute_script_content(script->content, script->filename);
}

bool ScriptManager::execute_script_file(const std::string& filepath)
{
    std::vector<std::string> content;
    if (!load_script_file(filepath, content)) {
        return false;
    }
    
    std::string filename = fs::path(filepath).filename().string();
    return execute_script_content(content, filename);
}

bool ScriptManager::execute_script_content(const std::vector<std::string>& content, const std::string& name)
{
    Logger::info(Logger::SCRIPT_EXEC, "Script execution requested: " + name);
    
    script_executor.reset_execution_state();
    bool success = script_executor.execute_script_content(content, name);
    
    if (success) {
        Logger::info(Logger::SCRIPT_EXEC, "Script completed successfully: " + name);
    } else {
        Logger::error(Logger::SCRIPT_EXEC, "Script failed: " + name);
    }
    
    return success;
}

std::unordered_map<std::string, std::string> ScriptManager::get_script_variables() const
{
    return script_executor.get_all_variables();
}

void ScriptManager::clear_script_variables()
{
    script_executor.clear_variables();
}

void ScriptManager::stop_current_script()
{
    script_executor.stop_execution();
}

bool ScriptManager::is_script_running() const
{
    return !script_executor.is_execution_stopped();
}

void ScriptManager::set_scripts_directory(const std::string& directory)
{
    scripts_directory = directory;
    discover_all_scripts();
}

std::string ScriptManager::resolve_script_path(const std::string& script_name)
{
    // Try different path combinations
    std::vector<std::string> possible_paths = {
        scripts_directory + "/" + script_name,
        scripts_directory + "/" + script_name + ".3rs",
        scripts_directory + "/debug/" + script_name,
        scripts_directory + "/debug/" + script_name + ".3rs",
        scripts_directory + "/scenarios/" + script_name,
        scripts_directory + "/scenarios/" + script_name + ".3rs",
        scripts_directory + "/utils/" + script_name,
        scripts_directory + "/utils/" + script_name + ".3rs"
    };
    
    for (const auto& path : possible_paths) {
        if (file_exists(path)) {
            return path;
        }
    }
    
    return "";
}

bool ScriptManager::file_exists(const std::string& filepath)
{
    try {
        return fs::exists(filepath);
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> ScriptManager::list_directory(const std::string& directory)
{
    std::vector<std::string> files;
    
    try {
        if (fs::exists(directory) && fs::is_directory(directory)) {
            for (const auto& entry : fs::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path().filename().string());
                }
            }
        }
    } catch (const std::exception& e) {
        Logger::error(Logger::SCRIPT, "Error listing directory " + directory + ": " + std::string(e.what()));
    }
    
    std::sort(files.begin(), files.end());
    return files;
}

std::string ScriptManager::get_last_error() const
{
    return "Error details not yet implemented";
}