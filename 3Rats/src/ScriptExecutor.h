#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <functional>

class Console;

enum class ScriptBlockType {
    IF,
    WHILE,
    EVENT_HANDLER
};

struct ScriptBlock {
    ScriptBlockType type;
    std::string condition;
    std::vector<std::string> body_lines;
    int start_line;
    bool is_active;
    
    ScriptBlock(ScriptBlockType t, const std::string& cond, int line)
        : type(t), condition(cond), start_line(line), is_active(false) {}
};

struct EventHandler {
    std::string event_name;
    std::vector<std::string> handler_lines;
    int script_line;
};

class ScriptExecutor {
private:
    std::unordered_map<std::string, std::string> variables;
    std::vector<EventHandler> event_handlers;
    std::stack<ScriptBlock> control_stack;
    Console* console_ref;
    
    int current_line;
    std::string current_file;
    bool execution_stopped;
    int max_loop_iterations;
    
    // Parsing and execution
    std::vector<std::string> tokenize_line(const std::string& line);
    std::string substitute_variables(const std::string& text);
    bool evaluate_condition(const std::string& condition);
    
    // Command handlers
    void handle_var_command(const std::vector<std::string>& tokens);
    void handle_set_command(const std::vector<std::string>& tokens);
    void handle_log_command(const std::vector<std::string>& tokens);
    void handle_if_command(const std::vector<std::string>& tokens);
    void handle_while_command(const std::vector<std::string>& tokens);
    void handle_end_command(const std::vector<std::string>& tokens);
    void handle_include_command(const std::vector<std::string>& tokens);
    void handle_on_command(const std::vector<std::string>& tokens);
    void handle_get_function(const std::vector<std::string>& tokens, std::string& result);
    
    // Console command delegation
    void execute_console_command(const std::vector<std::string>& tokens);
    
    // Safety validation
    bool validate_command_safety(const std::vector<std::string>& tokens);
    bool validate_teleport_coordinates(int x, int y);
    bool validate_actor_id(int actor_id);
    bool validate_health_value(int health);
    bool validate_time_values(int hours, int minutes);
    
    // Control flow management
    void enter_block(ScriptBlockType type, const std::string& condition);
    void exit_block();
    bool should_execute_line();
    
    // Utility functions
    bool is_script_command(const std::string& command);
    std::string trim(const std::string& str);
    bool is_number(const std::string& str);
    
public:
    ScriptExecutor(Console* console);
    ~ScriptExecutor();
    
    // Main execution interface
    bool execute_line(const std::string& line);
    bool execute_script_content(const std::vector<std::string>& lines, const std::string& filename);
    
    // Variable management
    void set_variable(const std::string& name, const std::string& value);
    std::string get_variable(const std::string& name);
    void clear_variables();
    std::unordered_map<std::string, std::string> get_all_variables() const;
    
    // Event system
    void register_event_handler(const std::string& event_name, const std::vector<std::string>& handler_lines);
    bool trigger_event(const std::string& event_name);
    void clear_event_handlers();
    
    // Execution control
    void stop_execution() { execution_stopped = true; }
    void reset_execution_state();
    bool is_execution_stopped() const { return execution_stopped; }
    
    // Error handling
    std::string get_current_context() const;
    void log_error(const std::string& message);
};