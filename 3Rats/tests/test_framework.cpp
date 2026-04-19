#include "test_framework.h"

namespace TestFramework {
    int test_count = 0;
    int passed_count = 0;
    int failed_count = 0;
    std::string current_suite = "";
    std::string current_test = "";

    void start_suite(const std::string& suite_name) {
        current_suite = suite_name;
    }

    void start_test(const std::string& test_name) {
        current_test = test_name;
    }

    void pass() {
        test_count++;
        passed_count++;
        std::cout << "[PASS] " << current_suite << "." << current_test << std::endl;
    }

    void fail(const std::string& message, const char* file, int line) {
        test_count++;
        failed_count++;
        std::cout << "[FAIL] " << current_suite << "." << current_test
                  << " (" << file << ":" << line << ")" << std::endl
                  << "       " << message << std::endl;
    }

    void print_summary() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Test Summary:" << std::endl;
        std::cout << "  Total:  " << test_count << std::endl;
        std::cout << "  Passed: " << passed_count << std::endl;
        std::cout << "  Failed: " << failed_count << std::endl;
        std::cout << "========================================" << std::endl;
    }

    int get_exit_code() {
        return (failed_count == 0) ? 0 : 1;
    }
}
