#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <string>
#include <cmath>

// Simple test framework - lightweight replacement for GoogleTest
namespace TestFramework {
    extern int test_count;
    extern int passed_count;
    extern int failed_count;
    extern std::string current_suite;
    extern std::string current_test;

    void start_suite(const std::string& suite_name);
    void start_test(const std::string& test_name);
    void pass();
    void fail(const std::string& message, const char* file, int line);
    void print_summary();
    int get_exit_code();
}

// Assertion macros
#define TEST_SUITE(suite_name) TestFramework::start_suite(#suite_name)

#define TEST_CASE(test_name) \
    TestFramework::start_test(#test_name); \
    auto test_##test_name = [&]()

#define RUN_TEST(test_name) \
    test_##test_name(); \
    TestFramework::pass()

#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        TestFramework::fail("Expected true, got false: " #condition, __FILE__, __LINE__); \
        return; \
    }

#define ASSERT_FALSE(condition) \
    if (condition) { \
        TestFramework::fail("Expected false, got true: " #condition, __FILE__, __LINE__); \
        return; \
    }

#define ASSERT_EQ(expected, actual) \
    if ((expected) != (actual)) { \
        TestFramework::fail("Expected " #expected " == " #actual, __FILE__, __LINE__); \
        return; \
    }

#define ASSERT_FLOAT_EQ(expected, actual) \
    if (std::abs((expected) - (actual)) > 0.0001f) { \
        TestFramework::fail("Expected " #expected " ~= " #actual, __FILE__, __LINE__); \
        return; \
    }

#endif // TEST_FRAMEWORK_H
