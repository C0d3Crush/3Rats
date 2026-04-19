#include "test_framework.h"
#include "HealthBar.h"

void test_healthbar() {
    TEST_SUITE(HealthBar);

    // Test 1: Constructor initializes to full health
    TEST_CASE(ConstructorDefault) {
        HealthBar bar;
        ASSERT_EQ(100, bar.get_current());
        ASSERT_EQ(100, bar.get_max());
        ASSERT_FLOAT_EQ(1.0f, bar.get_percentage());
    };
    RUN_TEST(ConstructorDefault);

    // Test 2: Custom dimensions constructor
    TEST_CASE(ConstructorCustomDimensions) {
        HealthBar bar(64, 8);
        ASSERT_EQ(100, bar.get_current());
        ASSERT_EQ(100, bar.get_max());
    };
    RUN_TEST(ConstructorCustomDimensions);

    // Test 3: Set values updates current and max
    TEST_CASE(SetValues) {
        HealthBar bar;
        bar.set_values(50, 100);
        ASSERT_EQ(50, bar.get_current());
        ASSERT_EQ(100, bar.get_max());
        ASSERT_FLOAT_EQ(0.5f, bar.get_percentage());
    };
    RUN_TEST(SetValues);

    // Test 4: Set current clamps to max
    TEST_CASE(SetCurrentClampsToMax) {
        HealthBar bar;
        bar.set_values(50, 100);
        bar.set_current(150);  // Try to exceed max
        ASSERT_EQ(100, bar.get_current());
    };
    RUN_TEST(SetCurrentClampsToMax);

    // Test 5: Set current clamps to zero
    TEST_CASE(SetCurrentClampsToZero) {
        HealthBar bar;
        bar.set_current(-10);  // Negative value
        ASSERT_EQ(0, bar.get_current());
    };
    RUN_TEST(SetCurrentClampsToZero);

    // Test 6: Percentage calculation at 100%
    TEST_CASE(PercentageAt100) {
        HealthBar bar;
        bar.set_values(100, 100);
        ASSERT_FLOAT_EQ(1.0f, bar.get_percentage());
    };
    RUN_TEST(PercentageAt100);

    // Test 7: Percentage calculation at 50%
    TEST_CASE(PercentageAt50) {
        HealthBar bar;
        bar.set_values(50, 100);
        ASSERT_FLOAT_EQ(0.5f, bar.get_percentage());
    };
    RUN_TEST(PercentageAt50);

    // Test 8: Percentage calculation at 0%
    TEST_CASE(PercentageAt0) {
        HealthBar bar;
        bar.set_values(0, 100);
        ASSERT_FLOAT_EQ(0.0f, bar.get_percentage());
    };
    RUN_TEST(PercentageAt0);

    // Test 9: Percentage calculation at 25%
    TEST_CASE(PercentageAt25) {
        HealthBar bar;
        bar.set_values(25, 100);
        ASSERT_FLOAT_EQ(0.25f, bar.get_percentage());
    };
    RUN_TEST(PercentageAt25);

    // Test 10: Percentage calculation at 75%
    TEST_CASE(PercentageAt75) {
        HealthBar bar;
        bar.set_values(75, 100);
        ASSERT_FLOAT_EQ(0.75f, bar.get_percentage());
    };
    RUN_TEST(PercentageAt75);

    // Test 11: Division by zero protection
    TEST_CASE(ZeroMaxValueProtection) {
        HealthBar bar;
        bar.set_values(50, 0);  // Invalid max value
        ASSERT_EQ(1, bar.get_max());  // max clamped to 1
        ASSERT_EQ(0, bar.get_current());  // current clamped to min(50, 0) = 0
        ASSERT_FLOAT_EQ(0.0f, bar.get_percentage());  // 0/1 = 0%
    };
    RUN_TEST(ZeroMaxValueProtection);

    // Test 12: Position setting
    TEST_CASE(SetPosition) {
        HealthBar bar;
        bar.set_position(100, 200);
        // Position is set (no getter, but verify it doesn't crash)
        ASSERT_TRUE(true);
    };
    RUN_TEST(SetPosition);
}
