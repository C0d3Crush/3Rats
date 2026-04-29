#include "test_framework.h"
#include "TimeManager.h"

void test_timemanager() {
    TestFramework::start_test("TimeManager");
    
    // Test initialization
    {
        TimeManager tm;
        TestFramework::assert_equal(tm.get_day_number(), 1, "Initial day number should be 1");
        TestFramework::assert_equal(tm.get_hours(), 16, "Initial hour should be 16 (4 PM)");
        TestFramework::assert_equal(tm.get_minutes(), 30, "Initial minute should be 30");
        TestFramework::assert_equal((int)tm.get_phase(), (int)TimePhase::DAY, "Initial phase should be DAY");
        TestFramework::assert_equal((int)tm.get_game_state(), (int)GameState::ACTIVE, "Initial state should be ACTIVE");
    }
    
    // Test time setting
    {
        TimeManager tm;
        tm.set_time(22, 0);  // Set to night time
        TestFramework::assert_equal(tm.get_hours(), 22, "Time should be set to 22");
        TestFramework::assert_equal(tm.get_minutes(), 0, "Minutes should be set to 0");
        TestFramework::assert_equal((int)tm.get_phase(), (int)TimePhase::NIGHT, "Phase should be NIGHT at 22:00");
    }
    
    // Test phase transitions
    {
        TimeManager tm;
        
        // Test dusk phase
        tm.set_time(20, 0);
        TestFramework::assert_equal((int)tm.get_phase(), (int)TimePhase::DUSK, "Should be DUSK at 20:00");
        
        // Test night phase
        tm.set_time(23, 0);
        TestFramework::assert_equal((int)tm.get_phase(), (int)TimePhase::NIGHT, "Should be NIGHT at 23:00");
        
        // Test dawn phase
        tm.set_time(6, 0);
        TestFramework::assert_equal((int)tm.get_phase(), (int)TimePhase::DAWN, "Should be DAWN at 06:00");
        
        // Test day phase
        tm.set_time(17, 0);
        TestFramework::assert_equal((int)tm.get_phase(), (int)TimePhase::DAY, "Should be DAY at 17:00");
    }
    
    // Test difficulty scaling
    {
        TimeManager tm;
        TestFramework::assert_float_equal(tm.get_difficulty_multiplier(), 1.0f, 0.01f, "Day 1 difficulty should be 1.0");
        
        tm.advance_to_next_day();
        TestFramework::assert_equal(tm.get_day_number(), 2, "Should advance to day 2");
        TestFramework::assert_float_equal(tm.get_difficulty_multiplier(), 1.2f, 0.01f, "Day 2 difficulty should be 1.2");
        
        tm.advance_to_next_day();
        TestFramework::assert_float_equal(tm.get_difficulty_multiplier(), 1.4f, 0.01f, "Day 3 difficulty should be 1.4");
    }
    
    // Test enemy scaling
    {
        TimeManager tm;
        TestFramework::assert_equal(tm.get_max_enemies_for_night(), 3, "Day 1 should have 3 max enemies");
        
        tm.advance_to_next_day();
        TestFramework::assert_equal(tm.get_max_enemies_for_night(), 5, "Day 2 should have 5 max enemies");
        
        tm.advance_to_next_day();
        TestFramework::assert_equal(tm.get_max_enemies_for_night(), 7, "Day 3 should have 7 max enemies");
    }
    
    // Test cage safety mechanics
    {
        TimeManager tm;
        tm.set_time(20, 0);  // Dusk
        tm.set_rats_in_cage(false);
        
        // Simulate warning check
        TestFramework::assert_equal(tm.are_rats_safe(), false, "Rats should not be safe outside cage");
        TestFramework::assert_equal(tm.is_night_time(), false, "Should not be night time yet at dusk");
        
        tm.set_rats_in_cage(true);
        TestFramework::assert_equal(tm.are_rats_safe(), true, "Rats should be safe in cage");
    }
    
    // Test time string formatting
    {
        TimeManager tm;
        tm.set_time(9, 5);
        TestFramework::assert_equal(tm.get_time_string(), "09:05", "Time string should be properly formatted");
        
        tm.set_time(23, 59);
        TestFramework::assert_equal(tm.get_time_string(), "23:59", "Time string should handle large numbers");
    }
    
    // Test reset functionality
    {
        TimeManager tm;
        tm.set_time(22, 0);
        tm.advance_to_next_day();
        tm.advance_to_next_day();
        
        tm.reset_game();
        TestFramework::assert_equal(tm.get_day_number(), 1, "Reset should return to day 1");
        TestFramework::assert_equal(tm.get_hours(), 16, "Reset should return to starting time");
        TestFramework::assert_equal((int)tm.get_phase(), (int)TimePhase::DAY, "Reset should return to day phase");
    }
    
    TestFramework::end_test();
}