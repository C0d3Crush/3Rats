#include "test_framework.h"
#include "Map.h"

void test_map() {
    TEST_SUITE(Map);

    // Test 1: Map constructor doesn't crash
    TEST_CASE(ConstructorBasic) {
        Map map;
        // Basic smoke test
        ASSERT_TRUE(true);
    };
    RUN_TEST(ConstructorBasic);

    // Test 2: Map ID can be set and retrieved
    TEST_CASE(SetAndGetMapID) {
        Map map;
        map.set_map_id(42);
        ASSERT_EQ(42, map.get_map_id());
    };
    RUN_TEST(SetAndGetMapID);

    // Note: Cannot test set_type() or item generation in isolation because:
    // - set_type() triggers map generation (generate_maze/garden/cage)
    // - Generation requires Random pointer and full Topography setup
    // - Item distribution testing requires integration tests with game state
    // Map generation is validated via Logger output and in-game testing.
}
