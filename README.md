# 3Rats

A 2D top-down game built with SDL2 (C++17). Three rats navigate a procedurally generated 5×5 room grid, collect food to survive, and avoid enemies.

---

## Roadmap: Item & Enemy System

### Phase 1 — Item-Typen & Effekte (Fundament)

**1.1 `ItemType` Enum / Effekt-Struct**
- Enum `ItemType`: `FOOD`, `SPEED_BOOST`, `SHIELD`, `DAMAGE_ORB`, ...
- Struct `ItemEffect { ItemType type; float value; float duration; bool is_passive; }`
- `Item` bekommt `ItemEffect effect` und `bool is_consumable`

**1.2 `Food` fertigstellen**
- `Food` (bereits stub) erbt von `Item`, hat `nutrition_value`
- `use_item()` in `Acteur` ruft `effect.apply(rat)` auf — kein hard-coded `saturation = 100` mehr

**1.3 Passive Effekte auf `Acteur`**
- `Acteur` bekommt `std::vector<ActiveEffect> active_effects` (je mit Timer)
- Jedes Frame: Timer runterzählen, abgelaufene Effekte entfernen, Werte (speed, defense) neu berechnen
- Beispiel-Effekte: Speed +50 %, Schadens-Resistenz, Saturation-Regen

---

### Phase 2 — Inventory-System

**2.1 `Inventory` an `Acteur` anschließen**
- `Inventory` (bereits stub) mit Slots verbinden
- `Acteur` hält `Inventory inventory` statt `item_hold_id` + `holds_item`
- Passive Items bleiben aktiv solange sie im Inventory sind

**2.2 Item-Pickup Refactor**
- Pickup per Radius statt Tile-exakt
- `Acteur::try_pickup(Item* items, int count)` — nächstes Item im Radius aufheben

---

### Phase 3 — Enemy Drops & HP

**3.1 Drop-Tabelle**
- `Enemy` bekommt `std::vector<ItemDrop> drop_table`
- Jeder Eintrag hat `ItemType type` + `float chance`
- Bei Tod: `Enemy::on_death()` rolled durch Tabelle und legt Items in die Welt

**3.2 Enemy HP-System**
- `Enemy` bekommt `int hp`, `int max_hp`
- Ratten können dem Enemy Schaden zufügen (z.B. durch Item-Use)
- Bei Tod: Verschwinden + Drop-Spawn

---

### Phase 4 — World-Drops & Map-Integration

**4.1 Item-Typen in Map-Daten kodieren**
- `Map::data[h][w].second` (item_type) über `0/1` hinaus ausbauen — Typ direkt kodieren
- `set_textures()` weist Textur und Effekt nach Typ zu

**4.2 Item-Varianten im Garden-Raum**
- Garden-Räume können neben Pilz auch Speed-Boost, Shield-Orb etc. spawnen
- Wahrscheinlichkeit pro Typ in `generate_garden()` parametrierbar

---

### Phase 5 — Mehrere Enemies & Skalierung

**5.1 Enemy-Array**
- `Enemy enemy_array[MAX_ENEMIES]` mit dynamischem Count
- Jeder Enemy hat eigenen `home_map_id` und eigene Drop-Tabelle
- Mehrere Enemy-Typen (schnell/langsam, viel/wenig Schaden) über Konfig-Struct oder Subklassen

**5.2 Wave-System / Spawn-Timer**
- Pro Raum: nach X Sekunden spawnt ein neuer Enemy nach
- Neuer `WaveManager` (oder in `Topography`) tracked welche Räume noch Enemies bekommen

---

### Reihenfolge

```
Phase 1.1 → 1.2 → 1.3   Item-Effekte, Food fertigstellen
Phase 2.1 → 2.2          Inventory
Phase 3.1 → 3.2          Enemy Drops + HP
Phase 4.1 → 4.2          World-Drops
Phase 5.1 → 5.2          Multi-Enemy & Waves
```

Jede Phase ist eigenständig testbar und baut auf der vorherigen auf.

---

## Build & Run

```bash
cd 3Rats/build
cmake ..        # einmalig
make -j$(nproc)
./3Rats
```

**Dependencies:** SDL2, SDL2_image, SDL2_ttf, SDL2_mixer — C++17 erforderlich.

Die ausführbare Datei muss aus `build/` gestartet werden, damit die Asset-Pfade (`../maze_textures/` etc.) korrekt aufgelöst werden.
