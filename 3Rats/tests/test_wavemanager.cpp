#include "test_framework.h"
#include "WaveManager.h"
#include "Enemy.h"
#include "Map.h"

void test_wavemanager() {
    TEST_SUITE(WaveManager);

    // Test 1: Constructor initializes correctly
    TEST_CASE(Constructor) {
        WaveManager wave_manager;
        ASSERT_EQ(1, wave_manager.get_current_wave());
        ASSERT_EQ(0, wave_manager.get_enemies_spawned());
    };
    RUN_TEST(Constructor);

    // Test 2: Reset functionality
    TEST_CASE(Reset) {
        WaveManager wave_manager;
        wave_manager.reset();
        ASSERT_EQ(1, wave_manager.get_current_wave());
        ASSERT_EQ(0, wave_manager.get_enemies_spawned());
    };
    RUN_TEST(Reset);

    // Test 3: Enemy multi-enemy management
    TEST_CASE(EnemyActivation) {
        Enemy enemy;
        
        // Test initial state
        ASSERT_FALSE(enemy.get_is_active());
        ASSERT_TRUE(enemy.is_alive());
        
        // Test activation
        enemy.activate();
        ASSERT_TRUE(enemy.get_is_active());
        ASSERT_TRUE(enemy.is_alive());
        ASSERT_EQ(enemy.get_max_hp(), enemy.get_hp());
        
        // Test deactivation
        enemy.deactivate();
        ASSERT_FALSE(enemy.get_is_active());
    };
    RUN_TEST(EnemyActivation);
}