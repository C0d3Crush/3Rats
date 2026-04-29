# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

The build directory is `build/` and the executable must be run from there so that relative asset paths (`../maze_textures/`, etc.) resolve correctly.

```bash
# Configure (first time)
cd build && cmake ..

# Build
cd build && make -j$(nproc)

# Run (must be from build/ directory)
cd build && ./3Rats

# Run with scripts
cd build && ./3Rats --script debug/god_mode
cd build && ./3Rats -s scenarios/demo_showcase
cd build && ./3Rats --script=test/startup_test --script=utils/clear_all

# Build and run tests (C++ unit tests)
cd build && make 3Rats_tests && ./tests/3Rats_tests

# Safety validation test script
cd build && ./3Rats -s test/safety_validation
```

**Tests**: C++ unit tests are configured via CMake with a custom TestFramework. Build target `3Rats_tests` in the `/tests` directory. Tests currently have compilation issues with missing TestFramework methods but the framework structure exists.

**Dependencies**: SDL2, SDL2_image, SDL2_ttf, SDL2_mixer. Requires C++17.

## Architecture

**3Rats** is a 2D SDL2 game where three rats navigate procedurally generated maps. The entire game loop lives in `src/main.cpp`.

### Rendering hierarchy

All renderable objects inherit from `Body`, which holds an `SDL_Texture*`, `crop_rect` (source rect), and `position_rect` (destination rect). The two-step init pattern used everywhere is:
1. `set_surface(renderer)` — stores the renderer pointer and loads a placeholder texture
2. `set_texture(path)` — loads the actual texture; updates `crop_rect.w/h` but NOT `position_rect.w/h`
3. `set_cords(x, y)` — copies `crop_rect.w/h` into `position_rect.w/h` and sets position

**Order matters**: `set_cords` must be called *after* the intended texture is loaded, otherwise `position_rect` will have the placeholder's dimensions.

### Map / level system

- `Topography` owns the world layout: a 5×5 grid of connected `Map` rooms. It generates the inter-room topology and holds the shared `tile_array` and `item_array` pointers that all maps reference.
- `Map` inherits `Level_Structure` and stores a 6×9 grid of `(tile_type, item_type)` pairs in `data[6][9]`. It procedurally generates room contents (`generate_maze`, `generate_garden`, `generate_cage`) and assigns textures to tiles via `set_textures()`.
- `Tile` and `Item` both inherit `Body`. Tiles are flagged with `is_exit`, `is_entrance`, `is_hole`.
- All maps share the **same** `tile_array[54]` and `item_array[54]` arrays (54 = 9×6). Only the current map's tiles are visible at a time.

### Game constants

- Window: 600×420px
- Tile size: 64×64px
- Grid: 9 wide × 6 tall = 54 tiles per map (also the shared `tile_array` / `item_array` size)
- 25 maps total (5×5 topology grid), 3 rats, 1 cat entity

### Acteur (player/NPC)

`Acteur` inherits both `Body` (rendering/position) and `Controller` (movement logic). `Controller` handles collision detection against the tile grid, pathfinding toward items (`follow_goal`), and door transitions. Three `Acteur` instances are the rats (controller numbers 0–2); a fourth is the cat entity (controller number 3).

The `mode` variable in `main()` toggles between `0` (player controls rat 0 via WASD) and `1` (all rats run AI). `Space` cycles between modes at runtime.

Each rat has a `saturation` value (0–100) that decays over time via `hungerTimer` / `nextHungerTick`. Use `reduce_saturation(int amount)` to apply damage from external sources (e.g. `Enemy`); it clamps to 0.

### Enemy

`Enemy` inherits only `Body` (not `Controller`). It has its own chase AI and wall collision, separate from the `Acteur`/`Controller` system.

- Spawns in a randomly selected garden room (`map_type == 1`) found after `init_topography`
- Only active when `topography->get_current_map_id() == home_map_id`; moves off-screen otherwise
- Chases rat 0 using normalised float-position movement (stores `float pos_x/pos_y` to avoid int-truncation drift at lower speeds)
- Wall collision uses the same circle-overlap approach as `Acteur::intersectsWithBody`
- Deals `DAMAGE_AMOUNT = 5` saturation damage per `DAMAGE_INTERVAL = 1.0f` second to any rat within `CONTACT_RADIUS = 48px`

`Map::get_type()` returns the stored `map_type` (0=maze, 1=garden, 2=cage) set during `set_type()`. This is the correct way to find garden rooms for enemy spawning.

### Debug / developer systems

- **`Console`** — in-game debug terminal. `handle_event` captures keystrokes when open; `draw()` renders the overlay. Holds pointers to all game arrays so commands can mutate game state directly.
- **`Metrics`** — overlay showing FPS, memory usage, and per-rat saturation. Toggled with `Tab`.

### Logger

`Logger` is a static class that writes structured logs to `logs/` (created relative to the working directory, i.e. `build/logs/`). Channels organized by category to prevent log file bloat:

**System:**
- `SYS` → `logs/system/init.log` — startup, core systems
- `SDL_INIT` → `logs/system/sdl_init.log` — SDL initialization, window creation
- `SDL_RENDER` → `logs/system/rendering.log` — frame rendering, texture loading  
- `SDL_EVENT` → `logs/system/events.log` — input events, window events
- `GAME_TIMING` → `logs/system/timing.log` — frame rate, game loop timing
- `ASSETS` → `logs/system/assets.log` — texture loading, font loading, resources

**Scripting (Subcategorized v0.5.1):**
- `SCRIPT` → `logs/scripting/script.log` — script discovery, general events
- `SCRIPT_EXEC` → `logs/scripting/execution.log` — script execution flow, start/stop
- `SCRIPT_CMD` → `logs/scripting/commands.log` — console commands executed by scripts  
- `SCRIPT_VAR` → `logs/scripting/variables.log` — variable assignments and operations

**Generation:**  
- `TOPOLOGY` → `logs/generation/topology.log` — world topology generation
- `MAP` → `logs/generation/map.log` — individual map generation

**Gameplay:**
- `ACTEUR` → `logs/gameplay/acteur.log` — player/NPC behavior  
- `ITEMS` → `logs/gameplay/items.log` — item interactions
- `GAME_STATE` → `logs/gameplay/game_state.log` — win/lose conditions, state changes
- `GAME_RULES` → `logs/gameplay/rules.log` — rule violations, safety checks

Four levels: DEBUG, INFO, WARN, ERR. The `errors.log` file mirrors all WARN and ERR messages for quick debugging. Each channel creates its own directory structure to keep logs organized by category.

**IMPORTANT DEVELOPMENT GUIDELINE**: Always use the Logger system for testing and implementation tracking. Log significant events, state changes, and debug information to maintain visibility into system behavior. Scripts automatically log both to console output and to the Logger system for comprehensive tracking.

### In-game keybindings (not configurable)

| Key | Action |
|-----|--------|
| `WASD` | Move rat 0 (player mode) |
| `Tab` | Toggle Metrics overlay |
| `` ` `` | Toggle Console |
| `Space` | Cycle player/AI mode |
| `E` | Set rat 0 to enter door |
| `R` | Teleport all rats to entrance |
| `U` | Use held item (all rats) |
| `P` | Place item (rats 1 & 2) |
| `N` | Clear goal (rats 1 & 2) |
| `O` | Teleport cat to entrance |
| `1` | Swap background to empty.png |

### Asset paths

All assets live one level above the build directory. Paths in code use `../` prefix:
- `../maze_textures/` — tile textures
- `../rat_textures/`, `../npc_textures/` — character sprites
- `../item_textures/` — collectible items
- `../ui_textures/` — HUD elements (clock, fade overlay)
- `../meta_textures/place_holder.png` — fallback texture
- `../fonts/sans.ttf` — UI font

SDL_image must be initialized with `IMG_INIT_PNG` (not `IMG_INIT_JPG`) since all assets are PNGs.

### Key init sequence in `main()`

```
init_tile_array   → creates 54 tiles with placeholder texture + renderer ptr
init_item_array   → creates 54 items with placeholder texture + renderer ptr
init_map_array    → attaches tile/item arrays to all maps; sets grid coords
init_topography   → generates world topology; calls map[0].set_textures()
init_player_array → creates Acteur instances, loads rat sprites
init_entity       → creates cat Acteur (controller number 3), off-screen at (4000,4000)
Enemy init        → scans map_array for garden rooms, picks one randomly, spawns Enemy there
```

The `Enemy` init must come **after** `init_topography` because `map_type` is only set inside `Map::set_type()` which is called during topography setup.

### In-progress / stub systems

- **`Food`** — extends `Item` with nutritional attributes; not yet wired into gameplay
- **`Inventory`** — vector-backed slot system; not yet connected to `Acteur`
- **`Sound`** — SDL_mixer wrapper for background music; not yet called from `main()`

### Scripts (v0.5.1 System)

`scripts/` lives at the project root (one level above `build/`). Files use the `.3rs` extension (3Rats Script). The v0.5.1 scripting system is fully implemented with `ScriptManager` and `ScriptExecutor` classes featuring:

- **Variable substitution** with `$variable` syntax
- **Safety validation** blocking dangerous operations (invalid coordinates, actor IDs, dangerous commands)
- **Non-blocking error handling** (scripts continue after validation failures)
- **Command-line integration** for automated startup script execution

Structure:
```
scripts/
├── scenarios/   — gameplay scenarios (moon_phase_setup.3rs)
├── utils/       — reusable helpers (clear_all.3rs)
├── debug/       — dev shortcuts (god_mode.3rs)
└── test/        — system tests (safety_validation.3rs for testing safety system)
```

**Console Commands:**
- `script list` — Show all discovered scripts
- `script run <name>` — Execute a script (e.g., `script run debug/god_mode`)
- `script vars` — Show current script variables
- `script stop` — Stop running script
- `script clear` — Clear script variables

**Command-Line Arguments:**
- `./3Rats --help` — Show help and available options
- `./3Rats --script <name>` — Run script automatically on startup  
- `./3Rats -s <name>` — Run script automatically on startup (short form)
- `./3Rats --script=<name>` — Run script automatically on startup (inline form)
- Multiple scripts: `./3Rats -s script1 --script script2 -s script3`

**Script Syntax:**
```bash
# Variables and substitution
var health = 100
log "Health is: $health"

# Console commands (validated for safety)
teleport 0 50 50
actor health 0 $health

# Conditionals (basic support)
if $health > 50
    log "Player is healthy"
else
    log "Player needs healing"
end

# Comments
-- This is a comment
```

**Safety Validation:**
The system validates all console commands for safety:
- **Actor IDs**: Must be 0-3 (rats 0-2, cat 3)
- **Coordinates**: Within reasonable bounds (-100 to 5000)  
- **Health values**: 0-100 range
- **Time values**: Hours 0-23, minutes 0-59
- **Time speed**: 0.1-10.0 safe range
- **Blocked commands**: `shutdown`, `exit`, `quit`, `reset_all`

Validation failures are logged to `logs/gameplay/rules.log` as warnings and scripts continue execution.

All script `log` commands appear both in console output and in the categorized scripting logs:
- Script content logs → `logs/scripting/script.log`
- Execution flow → `logs/scripting/execution.log`  
- Console commands → `logs/scripting/commands.log`
- Variable operations → `logs/scripting/variables.log`

This subcategorization prevents log file bloat and enables targeted debugging of specific script aspects.

### `src/legecy/`

Unused old implementations (Bowl, Digit, Frame, Menu, Clock_old). Safe to ignore.
