#include "test_framework.h"
#include "Item.h"

void test_item_effects() {
    TEST_SUITE(ItemEffect);

    // Test 1: Constructor
    TEST_CASE(Constructor) {
        ItemEffect effect(ItemType::FOOD, 50.0f, 0.0f, false);
        ASSERT_EQ(ItemType::FOOD, effect.type);
        ASSERT_FLOAT_EQ(50.0f, effect.value);
        ASSERT_FLOAT_EQ(0.0f, effect.duration);
        ASSERT_FALSE(effect.is_passive);
    };
    RUN_TEST(Constructor);

    // Test 2: SpeedBoostEffect
    TEST_CASE(SpeedBoostEffect) {
        ItemEffect effect(ItemType::SPEED_BOOST, 100.0f, 5.0f, false);
        ASSERT_EQ(ItemType::SPEED_BOOST, effect.type);
        ASSERT_FLOAT_EQ(100.0f, effect.value);
        ASSERT_FLOAT_EQ(5.0f, effect.duration);
        ASSERT_FALSE(effect.is_passive);
    };
    RUN_TEST(SpeedBoostEffect);

    // Test 3: ShieldEffect
    TEST_CASE(ShieldEffect) {
        ItemEffect effect(ItemType::SHIELD, 1.0f, 10.0f, false);
        ASSERT_EQ(ItemType::SHIELD, effect.type);
        ASSERT_FLOAT_EQ(1.0f, effect.value);
        ASSERT_FLOAT_EQ(10.0f, effect.duration);
        ASSERT_FALSE(effect.is_passive);
    };
    RUN_TEST(ShieldEffect);
}
