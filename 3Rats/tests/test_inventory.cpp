#include "test_framework.h"
#include "Inventory.h"

void test_inventory() {
    TEST_SUITE(Inventory);

    // Test 1: AddItem
    TEST_CASE(AddItem) {
        Inventory inv;
        ASSERT_TRUE(inv.add_item(0));
        ASSERT_EQ(0, inv.get_first_item());
        ASSERT_TRUE(inv.has_item());
        ASSERT_EQ(1, inv.get_item_count());
    };
    RUN_TEST(AddItem);

    // Test 2: FullInventory
    TEST_CASE(FullInventory) {
        Inventory inv;
        ASSERT_TRUE(inv.add_item(0));
        ASSERT_TRUE(inv.add_item(1));
        ASSERT_TRUE(inv.add_item(2));
        ASSERT_FALSE(inv.add_item(3)); // Should fail - inventory full (3 slots)
        ASSERT_TRUE(inv.is_full());
        ASSERT_EQ(3, inv.get_item_count());
    };
    RUN_TEST(FullInventory);

    // Test 3: RemoveItem
    TEST_CASE(RemoveItem) {
        Inventory inv;
        inv.add_item(5);
        ASSERT_EQ(5, inv.get_first_item());
        ASSERT_EQ(5, inv.remove_first_item());
        ASSERT_FALSE(inv.has_item());
        ASSERT_EQ(0, inv.get_item_count());
    };
    RUN_TEST(RemoveItem);

    // Test 4: GetFirstItem
    TEST_CASE(GetFirstItem) {
        Inventory inv;
        inv.add_item(10);
        inv.add_item(20);
        inv.add_item(30);
        ASSERT_EQ(10, inv.get_first_item());
        inv.remove_first_item();
        ASSERT_EQ(20, inv.get_first_item());
        inv.remove_first_item();
        ASSERT_EQ(30, inv.get_first_item());
    };
    RUN_TEST(GetFirstItem);
}
