# 3Rats Release Strategy

This document outlines the version numbering and release groupings for 3Rats development phases.

## Version Numbering

**Format**: `MAJOR.MINOR.PATCH`

- **MAJOR**: Complete game systems (0.x during development, 1.0 at feature-complete)
- **MINOR**: Functional feature groups that enhance gameplay
- **PATCH**: Bug fixes, minor improvements, individual phase completions

## Release Groupings

### v0.1.x - Foundation System
**Status**: In Development

#### v0.1.1 - Item Type Foundation ✅
**Released**: [Current]
- ItemType enum with FOOD, SPEED_BOOST, SHIELD, DAMAGE_ORB
- ItemEffect struct with type, value, duration, is_passive fields
- Updated Item class with effect and consumable properties
- Food class integration with effect system

**Changes**:
- `Item.h`: Added ItemType enum and ItemEffect struct
- `Item.cpp`: New getter/setter methods for effects and consumable flag
- `Food.cpp`: Uses ItemEffect system instead of hardcoded nutrition

**Impact**: Provides foundation for all future item functionality, but no visible gameplay changes yet.

#### v0.1.2 - Effect Application ✅
**Released**: [Current]
- Implement effect.apply(rat) in Acteur use_item() method
- Add active_effects vector to Acteur with timer system
- Frame-by-frame effect processing and value recalculation

**Changes**:
- `Acteur.h`: Added ActiveEffect struct and active_effects vector
- `Acteur.h`: Added apply_effect() and process_effects() method declarations
- `Acteur.cpp`: Implemented apply_effect() for passive (instant) and active (duration-based) effects
- `Acteur.cpp`: Implemented process_effects() for frame-by-frame effect timer management
- `Acteur.cpp`: Refactored use_item() to use ItemEffect system instead of hardcoded saturation=100
- `Acteur.cpp`: Added process_effects(delta) call in Update() method

**Impact**:
- Food items now properly apply their ItemEffect values (+nutrition saturation)
- Speed boosts, shields, and damage orbs can now have timed durations
- Effect expiration is logged and tracked per-rat
- Foundation ready for stat modifications (speed boost multipliers, shield damage reduction)

#### v0.1.3 - Inventory Integration ✅
**Released**: [Current]
- Connect Inventory class to Acteur replacing item_hold_id system
- Implement radius-based item pickup system

**Changes**:
- `Inventory.h`: Expanded with full inventory management API (add, remove, get, has, is_full, clear)
- `Inventory.h`: Added PICKUP_RADIUS constant (48px) and max_slots member
- `Inventory.cpp`: Implemented all methods with comprehensive Logger integration
- `Controller.h`: Added Inventory member, included Inventory.h header
- `Acteur.cpp:176`: Updated follow_goal() to use distance-based pickup (sqrt check <= 48px)
- `Acteur.cpp:176`: Replaced item_hold_id with inventory.add_item() on pickup
- `Acteur.cpp:627`: Refactored use_item() to use inventory.get_first_item() and remove_first_item()
- `Acteur.cpp:466`: Updated hold_item_in_mouth() to retrieve item from inventory
- `Acteur.cpp:524`: Updated debug_give_item() to use inventory.add_item() with bounds checking
- `Controller.cpp:151`: Updated place_item() to use inventory.remove_first_item()

**Impact**:
- Rats no longer need exact position match to pickup items (48px radius)
- Inventory system fully functional with 3-slot capacity per rat
- All item operations logged to logs/gameplay/items.log with detailed context
- Legacy item_hold_id system replaced with proper inventory management
- Foundation ready for multi-item inventory UI (v0.3.x)

---

### v0.2.x - Complete Item System
**Target**: Q1 Development

#### v0.2.0 - Enemy Integration
- Add drop_table vector to Enemy class
- Implement Enemy HP system with hp and max_hp fields
- Create Enemy on_death() method with item drop spawning
- Expand Map item_type encoding beyond 0/1 values
- Add item type variants to Garden room generation

**Milestone**: Complete overhaul of item mechanics from pickup to usage

---

### v0.3.x - UI & Visual Systems
**Target**: Q2 Development

#### v0.3.0 - Health & Damage UI
- Implement HP bars for rats with color interpolation
- Add HP bars for enemies with damage-triggered visibility
- Create floating damage numbers system with DamageNumberManager
- Implement GameSettings struct and pause menu settings
- Complete Sound class with volume and music controls

#### v0.3.1 - Multi-Enemy System
- Implement enemy array system with MAX_ENEMIES
- Add wave system and spawn timers with WaveManager

**Milestone**: Rich visual feedback and multiple enemy encounters

---

### v0.4.x - Core Gameplay Loop
**Target**: Q3 Development

#### v0.4.0 - Night/Day Cycle
- Implement night/day cycle with active time windows
- Add return-to-cage mechanics with visual warnings
- Create game over screen and daily evaluation system
- Implement difficulty scaling per night
- Add day phase time-lapse and rat regeneration

**Milestone**: Complete game loop with win/lose conditions - "Playable Game"

---

### v0.5.x - Developer Tools
**Target**: Q4 Development

#### v0.5.0 - Enhanced Console
- Replace food spawn with generic 'spawn item <type>' command
- Add comprehensive console commands for spawn, actors, world, time
- Refactor console parser to use tokenized commands and command map

#### v0.5.1 - Scripting System
- Implement pre-start script execution via command line arguments
- Create script browser and execution system in console
- Design and implement .3rs script language with variables, conditions, loops
- Add script functions, event hooks, and timing controls
- Create ScriptExecutor and ScriptManager classes

**Milestone**: Full modding and debugging capabilities

---

### v0.6.x - Advanced Systems
**Target**: 2025 H1

#### v0.6.0 - Moon & Calendar System
- Implement Calendar class with day/month/year tracking
- Add moon phase system with 8 phases over 29-day cycles
- Implement moon phase gameplay effects on enemies, hunger, spawns
- Add console commands for date and moon phase manipulation

#### v0.6.1 - Infinite World
- Design ChunkManager with chunk coordinates and infinite world system
- Refactor Topography for chunk-based world generation
- Implement on-demand chunk generation with border detection
- Add chunk memory management with LRU eviction and serialization
- Integrate chunk system with console commands and scripts

**Milestone**: Procedural infinite world with dynamic difficulty

---

### v0.7.x - Quality Assurance
**Target**: 2025 H2

#### v0.7.0 - Test Suite
- Set up GoogleTest framework via CMake FetchContent
- Write unit tests for Random, Acteur, Door, Controller, Map classes
- Create generation tests for maze, garden, cage room types
- Implement integration tests for world connectivity and gameplay mechanics
- Add simulation tests for AI behavior and enemy interactions

**Milestone**: Comprehensive testing and stability

---

### v1.0.0 - Feature Complete Release
**Target**: 2025 End

Complete 3Rats game with:
- Full item and effect system
- Rich UI and visual feedback
- Complete day/night gameplay loop
- Scripting and modding support
- Infinite procedural world
- Moon phase mechanics
- Comprehensive test coverage

---

## Release Notes Template

### v0.1.1 - Item Type Foundation

**New Features**:
- Added ItemType enum system (FOOD, SPEED_BOOST, SHIELD, DAMAGE_ORB)
- Introduced ItemEffect struct for flexible item properties
- Enhanced Item class with effect and consumable systems

**Technical Changes**:
- Refactored Food class to use new effect framework
- Added getter/setter methods for item properties
- Prepared foundation for future inventory and effect application

**Developer Notes**:
- This release establishes the architecture for all future item functionality
- No immediate gameplay changes visible to players
- Next release will implement effect application to rats

**Files Modified**: `Item.h`, `Item.cpp`, `Food.cpp`