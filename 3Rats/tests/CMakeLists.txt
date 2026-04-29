cmake_minimum_required(VERSION 3.14)

# Test executable
add_executable(3Rats_tests
    test_framework.cpp
    test_main.cpp
    test_inventory.cpp
    test_item_effects.cpp
    test_enemy.cpp
    test_map.cpp
    test_healthbar.cpp
    test_wavemanager.cpp
    test_timemanager.cpp
)

target_include_directories(3Rats_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(3Rats_tests PRIVATE
    3RatsLib
)

add_test(NAME 3Rats_tests COMMAND 3Rats_tests)
