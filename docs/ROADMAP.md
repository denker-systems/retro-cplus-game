# Retro Adventure - Development Roadmap

## Faser

```
┌─────────────────────────────────────────────────────────────────────┐
│ Fas 1: Core       │ Fas 2: Systems  │ Fas 3: Content │ Fas 4: Polish│
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ░░░░░░░░░░░░░░░ │ ░░░░░░░░░░░░░░ │ ░░░░░░░░░░░░│
│ 100%              │ 0%              │ 0%             │ 0%           │
└─────────────────────────────────────────────────────────────────────┘
```

---

## Fas 1: Core Foundation ✅ (Klar)

### 1.1 Grundläggande arkitektur ✅
- [x] SDL2 projekt setup
- [x] CMake + vcpkg
- [x] Game loop
- [x] Basic rendering
- [x] Input handling (keyboard, mouse, gamepad)

### 1.2 State Management ✅
- [x] IState interface
- [x] StateManager klass
- [x] MenuState
- [x] PlayState
- [x] PauseState
- [x] OptionsState

### 1.3 Resource Management ✅
- [x] TextureManager (singleton)
- [x] SpriteSheet loading (PNG)
- [x] Animation system
- [ ] Font rendering (framtida - SDL_ttf)

### 1.4 Audio Foundation ✅
- [x] AudioManager klass
- [x] Music playback
- [x] Sound effects
- [x] Volume kontroll

### 1.5 Entity System ✅
- [x] Entity base class
- [x] Character base class
- [x] PlayerCharacter

**Estimerad tid: 2-3 sessioner**

---

## Fas 2: Game Systems

### 2.1 Room System
- [ ] Room klass med bakgrund
- [ ] Walk area och kollision
- [ ] Hotspots (interaktiva områden)
- [ ] Exits (rumsövergångar)
- [ ] Room transitions (fade)
- [ ] JSON room definitions

### 2.2 Entity System
- [ ] Entity basklass
- [ ] Player med sprite animation
- [ ] Point-and-click movement
- [ ] NPC basklass
- [ ] Item entitet

### 2.3 Dialog System
- [ ] DialogNode struktur
- [ ] DialogTree parser (JSON)
- [ ] Dialog UI (textbox)
- [ ] Valmöjligheter
- [ ] Villkor och actions
- [ ] Character portraits

### 2.4 Inventory System
- [ ] InventorySystem klass
- [ ] Item pickup
- [ ] Inventory UI
- [ ] Item examination
- [ ] Item combination
- [ ] Use item on hotspot

### 2.5 Quest System
- [ ] Quest struktur
- [ ] Objectives
- [ ] Quest log UI
- [ ] Quest triggers
- [ ] Rewards

**Estimerad tid: 4-5 sessioner**

---

## Fas 3: Advanced Features

### 3.1 AI System
- [ ] AIBehavior interface
- [ ] Scheduled behaviors
- [ ] NPC pathfinding (A*)
- [ ] NPC states (idle, walk, talk)
- [ ] NPC schedules

### 3.2 Save/Load System
- [ ] SaveData struktur
- [ ] JSON serialization
- [ ] Multiple save slots
- [ ] Auto-save
- [ ] Save slot UI

### 3.3 Menu System
- [ ] Main menu
- [ ] Options (volume, fullscreen)
- [ ] Save/Load menu
- [ ] Credits
- [ ] Pause menu

### 3.4 UI System
- [ ] Widget basklass
- [ ] Button med hover/click
- [ ] Label med fonts
- [ ] Panels
- [ ] Scrollable lists

### 3.5 Cutscenes/Video
- [ ] Video playback (FFMPEG eller pre-rendered)
- [ ] Scripted sequences
- [ ] Camera movements
- [ ] Letterboxing

**Estimerad tid: 4-5 sessioner**

---

## Fas 4: Content & Polish

### 4.1 Game Content
- [ ] Komplett storyline
- [ ] Alla rum (5-10 för demo)
- [ ] Alla NPCs med dialoger
- [ ] Alla items
- [ ] Alla quests

### 4.2 Art Assets
- [ ] Character sprites
- [ ] NPC sprites
- [ ] Background art
- [ ] UI grafik
- [ ] Item ikoner
- [ ] Portraits

### 4.3 Audio Assets
- [ ] Background music (per rum)
- [ ] Ambient sounds
- [ ] UI sounds
- [ ] Dialog sounds (optional TTS)

### 4.4 Polish
- [ ] Transitions och animations
- [ ] Particle effects
- [ ] Screen shake
- [ ] Achievement system
- [ ] Statistics tracking

### 4.5 Testing & Release
- [ ] Bug fixes
- [ ] Balancing
- [ ] Localization support
- [ ] Installer
- [ ] Steam integration (optional)

**Estimerad tid: 5-10 sessioner**

---

## Prioriterad implementeringsordning

```
1.  StateManager + MenuState        # Grundläggande navigation
2.  TextureManager                  # Ladda sprites
3.  SpriteSheet + Animation         # Animerade karaktärer
4.  Room (basic)                    # Bakgrund + walk area
5.  Player (point-and-click)        # Klicka för att gå
6.  AudioManager                    # Musik och ljud
7.  Hotspots                        # Interaktiva objekt
8.  Dialog System (basic)           # Prata med NPCs
9.  Inventory (basic)               # Plocka upp items
10. Room exits + transitions        # Byta rum
11. Quest System                    # Mål och progression
12. NPC + AI                        # Levande värld
13. Save/Load                       # Spara progress
14. Full UI system                  # Polerade menyer
15. Content creation                # Spelet!
```

---

## Nästa session: Fas 2 - Room System

### Filer att skapa:
```
src/systems/
├── RoomSystem.h
├── RoomSystem.cpp
```

### Mål:
1. Bakgrundstexturer för rum
2. Walk area (polygon eller rektangel)
3. Hotspot-interaktion (klick)
4. Room exits och transitions

---

## Tekniska beslut

| Beslut | Val | Motivering |
|--------|-----|------------|
| State Management | State Stack | Tillåter pause overlay |
| Resource Loading | Lazy + Cache | Minskar minne |
| Data Format | JSON | Läsbart, flexibelt |
| Fonts | SDL_ttf | Enkel integration |
| Pathfinding | A* | Standard för 2D |
| Scripting | C++ lambdas | Ingen extern dependency |
| Save Format | JSON | Läsbart för debugging |

---

## Milstolpar

| Milstolpe | Beskrivning | Status |
|-----------|-------------|--------|
| M1 | Fönster + rendering | ✅ |
| M2 | State system + meny | ✅ |
| M3 | Rum med spelare | ⏳ |
| M4 | Dialog med NPC | ⬜ |
| M5 | Inventory fungerar | ⬜ |
| M6 | Quest system | ⬜ |
| M7 | Save/Load | ⬜ |
| M8 | Playable demo | ⬜ |
| M9 | Full game | ⬜ |
