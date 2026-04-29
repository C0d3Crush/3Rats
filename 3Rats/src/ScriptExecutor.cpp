#include "ScriptExecutor.h"
#include "Console.h"
#include "Logger.h"
#include <sstream>
#include <algorithm>
#include <cctype>

ScriptExecutor::ScriptExecutor(Console* console)
    : console_ref(console), current_line(0), execution_stopped(false),
      max_loop_iterations(1000)  // Safety limit for while loops
{
    Logger::info(Logger::SYS, "ScriptExecutor initialized");
}

ScriptExecutor::~ScriptExecutor()
{
    clear_variables();
    clear_event_handlers();
}

bool ScriptExecutor::execute_line(const std::string& line)
{
    if (execution_stopped) {
        return false;
    }
    
    current_line++;
    std::string trimmed = trim(line);
    
    // Skip empty lines and comments
    if (trimmed.empty() || trimmed.substr(0, 2) == "--") {
        return true;
    }
    
    // Check if we should execute this line based on control flow
    if (!should_execute_line()) {
        return true;
    }
    
    try {
        auto tokens = tokenize_line(trimmed);
        if (tokens.empty()) {
            return true;
        }
        
        std::string command = tokens[0];
        
        // Handle script-specific commands
        if (command == "var") {
            handle_var_command(tokens);
        } else if (command == "set") {
            handle_set_command(tokens);
        } else if (command == "log") {
            handle_log_command(tokens);
        } else if (command == "if") {
            handle_if_command(tokens);
        } else if (command == "while") {
            handle_while_command(tokens);
        } else if (command == "end") {
            handle_end_command(tokens);
        } else if (command == "include") {
            handle_include_command(tokens);
        } else if (command == "on") {
            handle_on_command(tokens);
        } else {
            // Delegate to console command system
            execute_console_command(tokens);
        }
        
        return true;
    } catch (const std::exception& e) {
        log_error("Execution error: " + std::string(e.what()));
        return false;
    }
}

bool ScriptExecutor::execute_script_content(const std::vector<std::string>& lines, const std::string& filename)
{
    current_file = filename;
    current_line = 0;
    execution_stopped = false;
    
    Logger::info(Logger::SCRIPT_EXEC, "Executing script: " + filename);
    
    // Also log to console
    if (console_ref) {
        console_ref->add_log_message("Starting script: " + filename);
    }
    
    for (const auto& line : lines) {
        if (!execute_line(line)) {
            log_error("Script execution failed at line " + std::to_string(current_line));
            return false;
        }
        
        if (execution_stopped) {
            Logger::info(Logger::SCRIPT_EXEC, "Script execution stopped");
            if (console_ref) {
                console_ref->add_log_message("Script execution stopped");
            }
            break;
        }
    }
    
    // Log script completion
    Logger::info(Logger::SCRIPT_EXEC, "Script completed: " + filename);
    if (console_ref) {
        console_ref->add_log_message("Script completed: " + filename);
    }
    
    return true;
}

std::vector<std::string> ScriptExecutor::tokenize_line(const std::string& line)
{
    std::vector<std::string> tokens;
    
    // First substitute all variables in the entire line
    std::string substituted_line = substitute_variables(line);
    
    std::istringstream ss(substituted_line);
    std::string token;
    bool in_quotes = false;
    std::string current_token;
    
    for (char c : substituted_line) {
        if (c == '"' && !in_quotes) {
            in_quotes = true;
        } else if (c == '"' && in_quotes) {
            in_quotes = false;
            tokens.push_back(current_token);
            current_token.clear();
        } else if (std::isspace(c) && !in_quotes) {
            if (!current_token.empty()) {
                tokens.push_back(current_token);
                current_token.clear();
            }
        } else {
            current_token += c;
        }
    }
    
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }
    
    return tokens;
}

std::string ScriptExecutor::substitute_variables(const std::string& text)
{
    std::string result = text;
    size_t pos = 0;
    
    while ((pos = result.find('$', pos)) != std::string::npos) {
        size_t end = pos + 1;
        
        // Find the end of the variable name
        while (end < result.length() && 
               (std::isalnum(result[end]) || result[end] == '_')) {
            end++;
        }
        
        if (end > pos + 1) {
            std::string var_name = result.substr(pos + 1, end - pos - 1);
            std::string var_value = get_variable(var_name);
            
            result.replace(pos, end - pos, var_value);
            pos += var_value.length();
        } else {
            pos++;
        }
    }
    
    return result;
}

void ScriptExecutor::handle_var_command(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 4 || tokens[2] != "=") {
        log_error("Invalid var syntax. Usage: var name = value");
        return;
    }
    
    std::string var_name = tokens[1];
    std::string var_value = tokens[3];
    
    // Join multiple tokens for the value if needed
    for (size_t i = 4; i < tokens.size(); i++) {
        var_value += " " + tokens[i];
    }
    
    set_variable(var_name, var_value);
}

void ScriptExecutor::handle_set_command(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 3) {
        log_error("Invalid set syntax. Usage: set property [target] value");
        return;
    }
    
    // Convert script 'set' commands to console commands
    std::vector<std::string> console_tokens;
    
    if (tokens[1] == "saturation" && tokens.size() >= 5) {
        // set saturation rat 0 100 -> actor health 0 100
        console_tokens = {"actor", "health", tokens[3], tokens[4]};
    } else if (tokens[1] == "timescale" && tokens.size() >= 3) {
        // set timescale 2.0 -> time speed 2.0
        console_tokens = {"time", "speed", tokens[2]};
    } else if (tokens[1] == "time" && tokens.size() >= 4) {
        // set time 22 0 -> time set 22 0
        console_tokens = {"time", "set", tokens[2], tokens[3]};
    } else {
        // Pass through as-is for unknown set commands
        console_tokens = tokens;
    }
    
    execute_console_command(console_tokens);
}

void ScriptExecutor::handle_log_command(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        return;
    }
    
    std::string message = tokens[1];
    for (size_t i = 2; i < tokens.size(); i++) {
        message += " " + tokens[i];
    }
    
    // Remove quotes if present
    if (message.length() >= 2 && message.front() == '"' && message.back() == '"') {
        message = message.substr(1, message.length() - 2);
    }
    
    Logger::info(Logger::SCRIPT, message);
    
    // Also output to console if available
    if (console_ref) {
        console_ref->add_log_message("[SCRIPT] " + message);
    }
}

void ScriptExecutor::handle_if_command(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        log_error("Invalid if syntax. Usage: if condition");
        return;
    }
    
    std::string condition;
    for (size_t i = 1; i < tokens.size(); i++) {
        if (i > 1) condition += " ";
        condition += tokens[i];
    }
    
    enter_block(ScriptBlockType::IF, condition);
}

void ScriptExecutor::handle_while_command(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        log_error("Invalid while syntax. Usage: while condition");
        return;
    }
    
    std::string condition;
    for (size_t i = 1; i < tokens.size(); i++) {
        if (i > 1) condition += " ";
        condition += tokens[i];
    }
    
    enter_block(ScriptBlockType::WHILE, condition);
}

void ScriptExecutor::handle_end_command(const std::vector<std::string>& tokens)
{
    if (control_stack.empty()) {
        log_error("'end' without matching block start");
        return;
    }
    
    exit_block();
}

void ScriptExecutor::handle_include_command(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        log_error("Invalid include syntax. Usage: include \"filename\"");
        return;
    }
    
    std::string filename = tokens[1];
    // Remove quotes if present
    if (filename.length() >= 2 && filename.front() == '"' && filename.back() == '"') {
        filename = filename.substr(1, filename.length() - 2);
    }
    
    Logger::info(Logger::SYS, "Include not yet implemented: " + filename);
}

void ScriptExecutor::handle_on_command(const std::vector<std::string>& tokens)
{
    if (tokens.size() < 2) {
        log_error("Invalid on syntax. Usage: on event_name");
        return;
    }
    
    std::string event_name = tokens[1];
    
    // For now, just log that event handlers are not implemented
    Logger::info(Logger::SYS, "Event handler registration not yet implemented: " + event_name);
}

void ScriptExecutor::execute_console_command(const std::vector<std::string>& tokens)
{
    if (!console_ref) {
        log_error("Console not available for command execution");
        return;
    }
    
    // Validate command safety before execution
    if (!validate_command_safety(tokens)) {
        Logger::warn(Logger::SCRIPT_EXEC, "Command blocked for safety: " + tokens[0]);
        Logger::warn(Logger::GAME_RULES, "Script command blocked for safety: " + tokens[0]);
        if (console_ref) {
            console_ref->add_log_message("SAFETY: Command blocked - " + tokens[0]);
        }
        return;
    }
    
    // Reconstruct command string for console execution
    std::string command_line;
    for (size_t i = 0; i < tokens.size(); i++) {
        if (i > 0) command_line += " ";
        command_line += tokens[i];
    }
    
    Logger::info(Logger::SCRIPT_CMD, "Console command: " + command_line);
    console_ref->execute(command_line);
}

bool ScriptExecutor::evaluate_condition(const std::string& condition)
{
    std::string trimmed_condition = trim(condition);
    
    // Handle simple boolean values
    if (trimmed_condition == "true" || trimmed_condition == "1") {
        return true;
    }
    if (trimmed_condition == "false" || trimmed_condition == "0") {
        return false;
    }
    
    // Look for comparison operators
    std::vector<std::string> operators = {"==", "!=", "<=", ">=", "<", ">"};
    
    for (const auto& op : operators) {
        size_t pos = trimmed_condition.find(op);
        if (pos != std::string::npos) {
            std::string left = trim(trimmed_condition.substr(0, pos));
            std::string right = trim(trimmed_condition.substr(pos + op.length()));
            
            // Substitute variables in both operands
            left = substitute_variables(left);
            right = substitute_variables(right);
            
            // Try numeric comparison if both are numbers
            if (is_number(left) && is_number(right)) {
                double left_val = std::stod(left);
                double right_val = std::stod(right);
                
                if (op == "==") return left_val == right_val;
                if (op == "!=") return left_val != right_val;
                if (op == "<")  return left_val < right_val;
                if (op == "<=") return left_val <= right_val;
                if (op == ">")  return left_val > right_val;
                if (op == ">=") return left_val >= right_val;
            } else {
                // String comparison
                if (op == "==") return left == right;
                if (op == "!=") return left != right;
                if (op == "<")  return left < right;
                if (op == "<=") return left <= right;
                if (op == ">")  return left > right;
                if (op == ">=") return left >= right;
            }
        }
    }
    
    // If no operators found, check if it's a variable with truthy value
    std::string value = substitute_variables(trimmed_condition);
    if (value == "true" || value == "1" || (!value.empty() && value != "0" && value != "false")) {
        return true;
    }
    
    return false;
}

void ScriptExecutor::enter_block(ScriptBlockType type, const std::string& condition)
{
    ScriptBlock new_block(type, condition, current_line);
    
    if (type == ScriptBlockType::IF) {
        // For IF blocks, evaluate condition immediately
        new_block.is_active = evaluate_condition(condition);
    } else if (type == ScriptBlockType::WHILE) {
        // For WHILE blocks, set initial active state
        new_block.is_active = evaluate_condition(condition);
    }
    
    control_stack.push(new_block);
}

void ScriptExecutor::exit_block()
{
    if (!control_stack.empty()) {
        control_stack.pop();
    }
}

bool ScriptExecutor::should_execute_line()
{
    // If no control blocks, always execute
    if (control_stack.empty()) {
        return true;
    }
    
    // We need to access the actual stack to modify is_active
    // For now, simplified logic: evaluate the top-most condition
    ScriptBlock& top_block = const_cast<ScriptBlock&>(control_stack.top());
    
    if (top_block.type == ScriptBlockType::IF) {
        if (!top_block.is_active) {
            // Evaluate condition for IF blocks once
            top_block.is_active = evaluate_condition(top_block.condition);
        }
        return top_block.is_active;
    } else if (top_block.type == ScriptBlockType::WHILE) {
        // For WHILE loops, re-evaluate condition each time
        top_block.is_active = evaluate_condition(top_block.condition);
        return top_block.is_active;
    }
    
    return true;
}

// Variable management
void ScriptExecutor::set_variable(const std::string& name, const std::string& value)
{
    variables[name] = value;
    Logger::info(Logger::SCRIPT_VAR, "Variable set: " + name + " = " + value);
}

std::string ScriptExecutor::get_variable(const std::string& name)
{
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    return "";  // Return empty string for undefined variables
}

void ScriptExecutor::clear_variables()
{
    variables.clear();
}

std::unordered_map<std::string, std::string> ScriptExecutor::get_all_variables() const
{
    return variables;
}

// Utility functions
std::string ScriptExecutor::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ScriptExecutor::is_number(const std::string& str)
{
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    
    bool has_dot = false;
    for (size_t i = start; i < str.length(); i++) {
        if (str[i] == '.' && !has_dot) {
            has_dot = true;
        } else if (!std::isdigit(str[i])) {
            return false;
        }
    }
    
    return start < str.length();
}

void ScriptExecutor::reset_execution_state()
{
    execution_stopped = false;
    current_line = 0;
    current_file.clear();
    
    // Clear control stack
    while (!control_stack.empty()) {
        control_stack.pop();
    }
}

std::string ScriptExecutor::get_current_context() const
{
    return current_file + ":" + std::to_string(current_line);
}

void ScriptExecutor::log_error(const std::string& message)
{
    std::string full_message = "[" + get_current_context() + "] " + message;
    Logger::error(Logger::SCRIPT_EXEC, full_message);
    
    // Also log error to console
    if (console_ref) {
        console_ref->add_log_message("SCRIPT ERROR: " + full_message);
    }
    
    execution_stopped = true;
}

// Event system stubs
void ScriptExecutor::register_event_handler(const std::string& event_name, const std::vector<std::string>& handler_lines)
{
    EventHandler handler;
    handler.event_name = event_name;
    handler.handler_lines = handler_lines;
    handler.script_line = current_line;
    event_handlers.push_back(handler);
}

bool ScriptExecutor::trigger_event(const std::string& event_name)
{
    // Event triggering not yet implemented
    return false;
}

void ScriptExecutor::clear_event_handlers()
{
    event_handlers.clear();
}

// ── Safety Validation System ──────────────────────────────────────────────

bool ScriptExecutor::validate_command_safety(const std::vector<std::string>& tokens)
{
    if (tokens.empty()) {
        return false;
    }
    
    std::string command = tokens[0];
    
    // Validate teleport commands
    if (command == "teleport" && tokens.size() >= 4) {
        try {
            int actor_id = std::stoi(tokens[1]);
            int x = std::stoi(tokens[2]);
            int y = std::stoi(tokens[3]);
            
            if (!validate_actor_id(actor_id)) {
                Logger::warn(Logger::GAME_RULES, "Invalid actor ID in teleport: " + tokens[1]);
                return false;
            }
            
            if (!validate_teleport_coordinates(x, y)) {
                Logger::warn(Logger::GAME_RULES, "Invalid teleport coordinates: " + tokens[2] + ", " + tokens[3]);
                return false;
            }
        } catch (const std::exception& e) {
            Logger::warn(Logger::GAME_RULES, "Invalid numeric values in teleport command");
            return false;
        }
    }
    
    // Validate actor health commands
    if (command == "actor" && tokens.size() >= 4 && tokens[1] == "health") {
        try {
            int actor_id = std::stoi(tokens[2]);
            int health = std::stoi(tokens[3]);
            
            if (!validate_actor_id(actor_id)) {
                Logger::warn(Logger::GAME_RULES, "Invalid actor ID in health command: " + tokens[2]);
                return false;
            }
            
            if (!validate_health_value(health)) {
                Logger::warn(Logger::GAME_RULES, "Invalid health value: " + tokens[3]);
                return false;
            }
        } catch (const std::exception& e) {
            Logger::warn(Logger::GAME_RULES, "Invalid numeric values in health command");
            return false;
        }
    }
    
    // Validate time set commands
    if (command == "time" && tokens.size() >= 4 && tokens[1] == "set") {
        try {
            int hours = std::stoi(tokens[2]);
            int minutes = std::stoi(tokens[3]);
            
            if (!validate_time_values(hours, minutes)) {
                Logger::warn(Logger::GAME_RULES, "Invalid time values: " + tokens[2] + ":" + tokens[3]);
                return false;
            }
        } catch (const std::exception& e) {
            Logger::warn(Logger::GAME_RULES, "Invalid numeric values in time command");
            return false;
        }
    }
    
    // Validate time speed commands
    if (command == "time" && tokens.size() >= 3 && tokens[1] == "speed") {
        try {
            float speed = std::stof(tokens[2]);
            if (speed < 0.1f || speed > 10.0f) {
                Logger::warn(Logger::GAME_RULES, "Time speed out of safe range (0.1-10.0): " + tokens[2]);
                return false;
            }
        } catch (const std::exception& e) {
            Logger::warn(Logger::GAME_RULES, "Invalid speed value in time command");
            return false;
        }
    }
    
    // Block potentially dangerous commands
    std::vector<std::string> blocked_commands = {"shutdown", "exit", "quit", "reset_all"};
    for (const auto& blocked : blocked_commands) {
        if (command == blocked) {
            Logger::warn(Logger::GAME_RULES, "Blocked dangerous command: " + command);
            return false;
        }
    }
    
    return true;
}

bool ScriptExecutor::validate_teleport_coordinates(int x, int y)
{
    // Valid game world bounds (9 tiles x 6 tiles, each 64px)
    // Map coordinates: 0-576 (9*64) x 0-384 (6*64)
    // Allow some buffer for edge cases
    const int MIN_X = -100;
    const int MAX_X = 5000;
    const int MIN_Y = -100;
    const int MAX_Y = 5000;
    
    if (x < MIN_X || x > MAX_X || y < MIN_Y || y > MAX_Y) {
        return false;
    }
    
    return true;
}

bool ScriptExecutor::validate_actor_id(int actor_id)
{
    // Valid actor IDs: 0-2 (rats), 3 (cat)
    return (actor_id >= 0 && actor_id <= 3);
}

bool ScriptExecutor::validate_health_value(int health)
{
    // Health should be between 0 and 100
    return (health >= 0 && health <= 100);
}

bool ScriptExecutor::validate_time_values(int hours, int minutes)
{
    // Hours: 0-23, Minutes: 0-59
    return (hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59);
}