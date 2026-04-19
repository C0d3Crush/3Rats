#include "test_framework.h"

// Forward declarations
void test_inventory();
void test_item_effects();
void test_enemy();
void test_map();

int main() {
    std::cout << "Running 3Rats Test Suite\n" << std::endl;

    test_inventory();
    test_item_effects();
    test_enemy();
    test_map();

    TestFramework::print_summary();
    return TestFramework::get_exit_code();
}
