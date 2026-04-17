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

### Phase 5 — UI: HP-Bars, Damage Numbers & Settings

**5.1 HP-Bar für Ratten**
- Saturation-Wert (0–100) wird als Balken über dem Rat angezeigt
- Farbe interpoliert von grün (voll) → rot (leer)
- Balken wird ausgeblendet wenn Saturation = 100 (voll)

**5.2 HP-Bar für Enemies**
- Gleiche Logik wie 5.1, über dem Enemy angezeigt
- Wird eingeblendet sobald der Enemy Schaden nimmt, wieder ausgeblendet wenn HP = max

**5.3 Floating Damage Numbers**
- Bei jedem Schadens-Tick erscheint die Zahl in rot über dem getroffenen Charakter
- Zahl schwebt nach oben und blendet über ~0.8 s aus (Position + Alpha animiert)
- Klasse `DamageNumber { float x, y, alpha; int value; float lifetime; }`
- `DamageNumberManager` hält eine Liste aktiver Zahlen, updated und rendert sie

**5.4 Settings-System & Pause-Menü**
- Struct `GameSettings { bool show_damage_numbers; bool music_enabled; float music_volume; ... }` — erweiterbar für spätere Optionen
- `Pause`-Klasse (bereits vorhanden) bekommt ein Settings-Untermenü
- Toggle "Damage Numbers: ON/OFF" im Pause-Menü steuerbar
- Toggle "Music: ON/OFF" sowie ein Lautstärke-Slider (0–100) im Pause-Menü
- Lautstärke-Änderung wird live an SDL_mixer (`Mix_VolumeMusic`) weitergegeben
- `Sound`-Klasse (bereits stub) fertigstellen: `set_volume(float)`, `toggle_music()`
- Settings werden zur Laufzeit an `DamageNumberManager`, HP-Bar-Renderer und `Sound` weitergegeben

---

### Phase 6 — Mehrere Enemies & Skalierung

**6.1 Enemy-Array**
- `Enemy enemy_array[MAX_ENEMIES]` mit dynamischem Count
- Jeder Enemy hat eigenen `home_map_id` und eigene Drop-Tabelle
- Mehrere Enemy-Typen (schnell/langsam, viel/wenig Schaden) über Konfig-Struct oder Subklassen

**6.2 Wave-System / Spawn-Timer**
- Pro Raum: nach X Sekunden spawnt ein neuer Enemy nach
- Neuer `WaveManager` (oder in `Topography`) tracked welche Räume noch Enemies bekommen

---

### Phase 7 — In-Game Console Erweiterung

**7.1 Generischer `spawn item`-Befehl**
- Bestehenden Food-Spawn-Befehl ersetzen durch: `spawn item <type>` — spawnt jedes Item nach `ItemType`
- Beispiele: `spawn item food`, `spawn item speed_boost`, `spawn item shield`
- Item wird an der Position von Rat 0 in die Welt gesetzt
- Unbekannte Typen geben eine Fehlermeldung in der Console aus

**7.2 Weitere Console-Befehle**

*Spawn*
- `spawn item <type>` — spawnt Item an Position von Rat 0
- `spawn enemy <type> <map_id>` — spawnt einen bestimmten Enemy-Typ in einem bestimmten Raum; ohne `map_id` wird der aktuelle Raum verwendet
- `fill items` — spawnt alle Item-Typen im aktuellen Raum
- `give all` — gibt Rat 0 jeden Item-Typ direkt

*Actors*
- `set saturation <rat> <value>` — setzt Saturation eines Rats (0–100)
- `set speed <rat> <value>` — überschreibt Move-Speed eines Rats temporär
- `set hp enemy <value>` — setzt HP des Enemies im aktuellen Raum
- `kill enemy` — Enemy im aktuellen Raum sofort töten
- `freeze enemy` — Enemy-Bewegung einfrieren / wieder aktivieren
- `god <rat>` — kein Hunger-Decay, unendliche Saturation für diesen Rat

*Entity-Inspector (gilt für Rats, Enemies, Cat und alle zukünftigen Entitäten)*
- `select <entity_type> <id>` — wählt eine Entität als aktives Ziel aus, z.B. `select rat 0`, `select enemy 2`, `select cat 0`
- `get <attribute>` — gibt einen Attributwert der ausgewählten Entität aus, z.B. `get speed`, `get hp`, `get pos`
- `set <attribute> <value>` — setzt einen Attributwert der ausgewählten Entität, z.B. `set speed 300`, `set hp 50`, `set saturation 80`
- `list entities` — gibt alle vorhandenen Entitäten mit ID, Typ und aktuellem Raum aus
- `info` — gibt alle Attribute der aktuell ausgewählten Entität auf einmal aus

Implementierung: alle Entitäten (Rats, Enemies, Cat) implementieren ein gemeinsames Interface `IInspectable { virtual std::string get_attr(const std::string&); virtual void set_attr(const std::string&, const std::string&); }`. Die Console hält einen `IInspectable*`-Zeiger auf die aktuell ausgewählte Entität.

*World*
- `tp <map_id>` — teleportiert alle Rats in den angegebenen Raum
- `regen <map_id>` — einzelnen Raum neu generieren
- `set room <map_id> <type>` — Raum-Typ (maze/garden/cage) zur Laufzeit ändern
- `clear items` — alle Items im aktuellen Raum entfernen
- `reveal` — Topologie aller 25 Räume in der Console ausgeben

*Zeit & Simulation*
- `set time <hour>` — Spielzeit direkt setzen
- `timescale <factor>` — Spielgeschwindigkeit skalieren (0.5 = slow-mo, 2.0 = schnell)
- `pause hunger` — Hunger-Decay global einfrieren / wieder aktivieren
- `noclip <rat>` — Tile-Collision für diesen Rat deaktivieren

*Info & Logging*
- `info rat <n>` — alle Werte von Rat n ausgeben (pos, saturation, inventory, goal)
- `info map` — aktuellen Raum-Typ, Verbindungen und Door-Positionen ausgeben
- `fps <cap>` — FPS-Cap setzen
- `seed` — aktuellen World-Seed ausgeben
- `log level <debug|info|warn|err>` — Logger-Level zur Laufzeit ändern
- `help` — listet alle verfügbaren Befehle in der Console auf

**7.3 Console-Parser refactoren**
- Befehle als tokenisierte Strings parsen (`command arg1 arg2 ...`) statt Einzelzeichen-Vergleiche
- `Console` registriert Befehle als Map: `std::unordered_map<std::string, CommandFn>`
- Neue Befehle können ohne Änderung am Parser-Core hinzugefügt werden

---

### Reihenfolge

```
Phase 1.1 → 1.2 → 1.3         Item-Effekte, Food fertigstellen
Phase 2.1 → 2.2               Inventory
Phase 3.1 → 3.2               Enemy Drops + HP
Phase 4.1 → 4.2               World-Drops
Phase 5.1 → 5.2 → 5.3 → 5.4  HP-Bars, Damage Numbers, Settings
Phase 6.1 → 6.2               Multi-Enemy & Waves
Phase 7.1 → 7.2 → 7.3        Console-Erweiterung & Parser-Refactor
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
