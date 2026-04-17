# 3Rats

A 2D top-down game built with SDL2 (C++17). Three rats escape their cage every night while their owners sleep, explore a procedurally generated 5×5 room grid, collect food — and must return before dawn or it's game over.

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

### Phase 7 — Nacht-Zyklus & Game Loop (Kern-Gameplay)

Die Ratten sind nachts aktiv während die Besitzer schlafen. Jede Nacht ist eine Runde — wer nicht rechtzeitig zurück ist, wird entdeckt.

**7.1 Aktive Spielzeit-Fenster**
- Die Uhr (bereits vorhanden) definiert zwei Phasen pro Tag:
  - **Nacht** (z.B. 22:00–06:00) — Ratten dürfen das Käfig-Zimmer verlassen, Türen sind offen
  - **Tag** (06:00–22:00) — Besitzer wach, Ratten müssen im Käfig (map 0) sein
- Beim Übergang von Nacht → Tag: alle Ratten die sich **nicht** in map 0 befinden → **Game Over**
- Beim Übergang von Tag → Nacht: neue Runde beginnt, Welt wird teilweise neu generiert

**7.2 Rückkehr-Mechanik**
- Wenn die Uhr sich dem Morgen nähert (z.B. 05:00) erscheint eine visuelle Warnung — Uhr-Farbe wechselt, Screen-Rand pulsiert rot
- Ab 05:30 läuft ein sichtbarer Countdown
- Ratten müssen durch die Käfig-Tür (map 0 Entrance) zurück — nicht nur im Raum sein
- Alle drei Ratten müssen zurück sein — eine fehlende Ratte = Game Over

**7.3 Game Over & Tages-Auswertung**
- Game Over Screen zeigt: wie lange überlebt, gesammelte Items, besuchte Räume
- Kein harter Reset — optionaler "Retry same night" Modus mit gleichem Seed
- Tages-Zähler (bereits in `Clock` vorhanden) zählt erfolgreich abgeschlossene Nächte

**7.4 Schwierigkeits-Skalierung pro Nacht**
- Jede Nacht wird das Zeitfenster minimal kürzer (z.B. −2 Minuten pro Nacht)
- Enemies werden schneller, Hunger-Decay steigt leicht
- Ab Nacht 5+: manche Türen sind nur bestimmte Zeit offen

**7.5 Tag-Phase (Besitzer wach)**
- Während des Tages läuft die Zeit schnell durch (kein aktives Gameplay, Zeitraffer-Effekt)
- Kurze Zusammenfassung der letzten Nacht auf dem Screen
- Ratten regenerieren Saturation im Käfig während des Tages

---

### Phase 8 — In-Game Console Erweiterung


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

### Phase 9 — Scripting-System

Scripts sind Textdateien (`.3rs`) die aus der Console heraus geladen und ausgeführt werden. Sie können alle Console-Befehle aufrufen und eigene Logik enthalten.

**9.1 Pre-Start Scripts via Kommandozeilen-Argument**
- Scripts können beim Spielstart als Argument übergeben werden: `./3Rats --script setup.3rs`
- Mehrere Scripts möglich: `./3Rats --script worldgen.3rs --script debug.3rs`
- Pre-Start Scripts laufen **vor** der World-Generation — sie können Generation-Parameter setzen bevor `init_topography` aufgerufen wird
- Beispiel-Anwendungsfälle:
  - Seed festlegen: `set seed 12345`
  - Raum-Typen vorschreiben: `set room 0 garden`, `set room 5 maze`
  - Enemy-Spawn in bestimmten Räumen konfigurieren
  - Globale Spielparameter setzen: `set timescale 0.5`, `set hunger_rate 0`
- Zwei Ausführungs-Phasen im `main()`:
  1. **Pre-Generation** — Script läuft vor `init_topography()`; darf Generation-Parameter, Seed, Raum-Typen setzen
  2. **Post-Generation** — optionales zweites Script läuft nach der kompletten Init; darf Entities und Items manipulieren
- Unbekannte Befehle in Pre-Start Scripts werden geloggt und übersprungen (kein Crash)

**9.2 Script-Ordner & externes Schreiben**
- Scripts liegen in `scripts/` — direkt neben `build/` im Projekt-Root, damit sie aus dem laufenden Spiel über `../scripts/` erreichbar sind
- Scripts können mit jedem Texteditor außerhalb des Spiels geschrieben und gespeichert werden
- Dateiendung `.3rs` (3Rats Script)
- Der Ordner wird beim ersten Start automatisch angelegt falls er nicht existiert
- Unterordner sind erlaubt, z.B. `scripts/debug/`, `scripts/scenarios/`

**9.3 Script-Browser in der Console**
- `scripts` — listet alle `.3rs`-Dateien im `scripts/`-Ordner rekursiv auf, mit Unterordner-Struktur
- `run <dateiname>` — lädt und führt ein Script aus `scripts/` aus, z.B. `run test.3rs` oder `run debug/setup.3rs`
- `run` ohne Argument — öffnet einen Inline-Script-Modus in der Console (mehrzeilig, `end` zum Ausführen)
- Scripts werden beim `run`-Befehl frisch von Disk gelesen — Änderungen im Editor sind sofort wirksam ohne Neustart
- Scripts werden zeilenweise geparst und an denselben Command-Dispatcher wie die Console übergeben (Phase 7.3)

**9.4 Script-Sprache**

*Variablen & Typen*
```
var hp      = 100           -- int
var speed   = 1.5           -- float
var name    = "mango"       -- string
var active  = true          -- bool
```

Variablen können Werte aus dem Spiel lesen:
```
var cur_sat = get saturation rat 0
var cur_map = get current_map
var rat_x   = get pos_x rat 0
```

*Arithmetik & String-Operationen*
```
var x = 10 + 5
var y = $x * 2
var z = $y / 3
var label = "rat" + 0       -- string concat → "rat0"
var half = $cur_sat / 2
```

*Bedingungen*
```
if $cur_sat < 50
    spawn item food
end

if $cur_sat < 20
    spawn item food
else if $cur_sat < 60
    spawn item speed_boost
else
    -- ratte ist satt, nichts tun
end
```

Vergleiche: `<` `>` `<=` `>=` `==` `!=`
Logik: `and` `or` `not`
```
if $cur_sat < 30 and get hp enemy 0 > 0
    set speed rat 0 300
end
```

*Schleifen*
```
-- feste Anzahl
repeat 3
    spawn enemy basic
end

-- mit Zähler-Variable
var i = 0
while $i < 5
    spawn item food
    var i = $i + 1
end

-- endlos (bis break)
loop
    wait 5.0
    if get saturation rat 0 < 40
        spawn item food
    end
    if get current_map == 0
        break
    end
end
```

*Funktionen*
```
func heal_all_rats
    set saturation rat 0 100
    set saturation rat 1 100
    set saturation rat 2 100
end

func spawn_wave count
    var i = 0
    while $i < $count
        spawn enemy basic
        var i = $i + 1
    end
end

-- Aufruf:
call heal_all_rats
call spawn_wave 4
```

*Events / Hooks*
Scripts können auf Spielereignisse reagieren — der `ScriptExecutor` ruft den jeweiligen Block automatisch auf:
```
on rat_hurt 0               -- wenn Rat 0 Schaden nimmt
    if get saturation rat 0 < 25
        spawn item food
    end
end

on enter_room 5             -- wenn Spieler Raum 5 betritt
    spawn enemy fast
    set timescale 0.8
end

on night_start              -- beim Übergang Tag → Nacht
    call heal_all_rats
    set time 22
end

on curfew_warning           -- wenn 05:00 Uhr erreicht wird
    set timescale 1.5       -- Zeit schneller laufen lassen
end

on item_pickup food         -- wenn irgendein Rat Food aufhebt
    log "food collected"
end

on enemy_death              -- wenn ein Enemy stirbt
    spawn item speed_boost
end
```

Verfügbare Events: `rat_hurt <id>`, `rat_death <id>`, `enter_room <map_id>`, `leave_room <map_id>`, `item_pickup <type>`, `enemy_death`, `night_start`, `night_end`, `curfew_warning`, `game_over`

*Logging & Debug-Ausgabe*
```
log "script gestartet"
log "saturation ist: " + $cur_sat
log_warn "ratte fast verhungert"
log_err "unerwarteter zustand"
```
Ausgabe erscheint in der In-Game Console und im Logger-System (`logs/gameplay/acteur.log`)

*Warten & Timing*
```
wait 2.0            -- 2 Sekunden Spielzeit warten
wait_until 05 30    -- warten bis Spieluhr 05:30 erreicht
wait_frames 10      -- genau 10 Game-Loop-Frames warten
```

*Include & Modularität*
```
include "utils/helpers.3rs"     -- anderen Script einbinden
include "scenarios/hard.3rs"
```

*Kommentare*
```
-- einzeiliger Kommentar
```

Vollständiges Beispiel — Nacht-Setup Script (`scripts/scenarios/hard_night.3rs`):
```
-- Schwere Nacht: kurzes Zeitfenster, viele Enemies, wenig Food
include "utils/clear_room.3rs"

set time 22
set hunger_rate 2.0
set timescale 1.2

var map = 1
while $map < 25
    if get room_type $map == "garden"
        spawn enemy fast $map
        spawn enemy basic $map
    end
    var map = $map + 1
end

on curfew_warning
    set timescale 2.0
    log_warn "nur noch 30 minuten!"
end

on night_end
    call heal_all_rats
    log "nacht geschafft"
end
```

**9.5 Script-Executor**
- Klasse `ScriptExecutor` parst eine Script-Datei in eine Liste von `ScriptStatement`
- Statements werden im Game-Loop schrittweise abgearbeitet (nicht blockierend — `wait` wird über einen Timer gelöst)
- `ScriptExecutor` hält eine Referenz auf den Command-Dispatcher der Console
- Fehler (unbekannter Befehl, falsche Argumente) werden in der Console ausgegeben mit Zeilen-Nummer

**9.6 Script-Manager**
- `ScriptManager` hält eine Queue aktiver Scripts — mehrere Scripts können gleichzeitig laufen
- Scripts können andere Scripts aufrufen: `run other.3rs`
- `stop` — bricht das aktuell laufende Script ab
- `list scripts` — zeigt alle laufenden Scripts mit Status in der Console an

---

### Reihenfolge

```
Phase 1.1 → 1.2 → 1.3         Item-Effekte, Food fertigstellen
Phase 2.1 → 2.2               Inventory
Phase 3.1 → 3.2               Enemy Drops + HP
Phase 4.1 → 4.2               World-Drops
Phase 5.1 → 5.2 → 5.3 → 5.4  HP-Bars, Damage Numbers, Settings
Phase 6.1 → 6.2                              Multi-Enemy & Waves
Phase 7.1 → 7.2 → 7.3 → 7.4 → 7.5          Nacht-Zyklus & Kern-Gameplay Loop
Phase 8.1 → 8.2 → 8.3                       Console-Erweiterung & Parser-Refactor
Phase 9.1 → 9.2 → 9.3 → 9.4 → 9.5 → 9.6   Scripting-System
Phase 10.1 → 10.2 → 10.3 → 10.4            Test-Suite
```

Jede Phase ist eigenständig testbar und baut auf der vorherigen auf.

---

### Phase 10 — Test-Suite

**Framework: GoogleTest** via CMake `FetchContent` — kein manuelles Installieren nötig, läuft im selben Build-System wie das Spiel. Tests liegen in `tests/` und werden als separates Binary `3Rats_tests` gebaut.

**10.1 Unit Tests**
- `Random` — Würfelergebnisse bleiben im erwarteten Bereich; gleicher Seed erzeugt identische Sequenz
- `Acteur` Saturation — `reduce_saturation` geht nie unter 0, Wert bleibt immer 0–100
- `Door` Platzierung — Türen landen nie auf Ecken, immer auf der korrekten Wand für ihre Seite
- `Controller` Kollision — blockierte Richtungen korrekt aus Tile-Positionen berechnet
- `Map` Daten — `save_data` / `get_tile` Indexberechnung korrekt für alle Gitterpositionen

**10.2 Generierungs-Tests**
- Maze — alle Türen des Raumes sind durch Gänge verbunden, kein Raum generiert mit hängendem `rec_pos`
- Garden — Innenbereich hat keine Wand-Tiles, Türen korrekt gesetzt
- Cage — generiert ohne Absturz, Boden-Tiles korrekt
- 1-Door Maze — terminiert immer (Regression für alten Infinite-Loop Bug)
- `rec_pos` — terminiert innerhalb einer maximalen Iterationsanzahl für jeden validen Input

**10.3 Integrations-Tests**
- Gleicher Seed → identisches Welt-Layout bei jedem Start
- `Topography` — alle 25 Räume von Raum 0 aus erreichbar; Verbindungs-Bitmasks sind symmetrisch (wenn A→B dann B→A)
- Tür-Übergang — Ratte landet auf der korrekten Seite des Nachbar-Raumes
- Item Pickup → Use → Saturation wiederhergestellt
- Enemy Schaden — Ratten-Saturation sinkt nach N Sekunden Kontakt

**10.4 Simulations-Tests**
- AI-Ratten finden ein erreichbares Item innerhalb von X simulierten Sekunden (kein Infinite Loop in `make_goal`)
- Alle 25 Räume über Türen von Raum 0 erreichbar
- Enemy steckt nach N Frames nicht dauerhaft in einer Wand fest
- Enemy spawnt immer in einem Garden-Raum — nie in Maze oder Cage (Regression)

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
