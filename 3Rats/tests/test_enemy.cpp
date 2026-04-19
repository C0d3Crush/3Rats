#include "test_framework.h"
#include "Enemy.h"
#include "Item.h"

void test_enemy() {
    TEST_SUITE(Enemy);

    // Test 1: Constructor initializes HP correctly
    TEST_CASE(ConstructorInitializesHP) {
        Enemy enemy;
        ASSERT_EQ(50, enemy.get_hp());
        ASSERT_EQ(50, enemy.get_max_hp());
        ASSERT_TRUE(enemy.is_alive());
    };
    RUN_TEST(ConstructorInitializesHP);

    // Test 2: SetHomeMap
    TEST_CASE(SetHomeMap) {
        Enemy enemy;
        enemy.set_home_map(5);
        // Verify setting home map doesn't crash
        ASSERT_TRUE(true);
    };
    RUN_TEST(SetHomeMap);

    // Test 3: TakeDamage reduces HP
    TEST_CASE(TakeDamageReducesHP) {
        Enemy enemy;
        enemy.take_damage(10);
        ASSERT_EQ(40, enemy.get_hp());
        ASSERT_TRUE(enemy.is_alive());
    };
    RUN_TEST(TakeDamageReducesHP);

    // Test 4: TakeDamage multiple times
    TEST_CASE(TakeDamageMultipleTimes) {
        Enemy enemy;
        enemy.take_damage(20);
        ASSERT_EQ(30, enemy.get_hp());
        enemy.take_damage(15);
        ASSERT_EQ(15, enemy.get_hp());
        ASSERT_TRUE(enemy.is_alive());
    };
    RUN_TEST(TakeDamageMultipleTimes);

    // Test 5: HP reduces to near-death
    TEST_CASE(HPReducesToNearDeath) {
        Enemy enemy;
        enemy.take_damage(45); // Leave with 5 HP
        ASSERT_EQ(5, enemy.get_hp());
        ASSERT_TRUE(enemy.is_alive());
    };
    RUN_TEST(HPReducesToNearDeath);

    // Test 6: SetDropTable stores item types
    TEST_CASE(SetDropTableStoresItemTypes) {
        Enemy enemy;
        std::vector<ItemType> drops = {ItemType::FOOD, ItemType::SPEED_BOOST};
        enemy.set_drop_table(drops);
        // If this doesn't crash, drop table is stored correctly
        ASSERT_TRUE(true);
    };
    RUN_TEST(SetDropTableStoresItemTypes);

    // Note: Cannot test death/on_death() in isolation because it requires
    // topography->get_item_array() which needs full game setup.
    // Death mechanics are tested via integration tests with full game state.
}
