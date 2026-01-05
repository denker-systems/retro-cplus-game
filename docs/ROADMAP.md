# Retro Adventure - Development Roadmap

## Faser

```
┌─────────────────────────────────────────────────────────────────────┐
│ Fas 1: Core       │ Fas 2: Systems  │ Fas 3: Advanced│ Fas 4: Content│
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ░░░░░░░░░░░░│
│ 100%              │ 100%            │ 100%           │ 0%           │
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
- [x] FontManager (SDL_ttf)

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

## Fas 2: Game Systems ✅ (Klar)

### 2.1 Room System ✅
- [x] Room klass med bakgrund
- [x] Walk area och kollision
- [x] Hotspots (interaktiva områden)
- [x] RoomManager med rumsövergångar
- [x] Fade transitions
- [x] JSON room definitions
- [x] NPC-hantering per rum

### 2.2 Entity System ✅
- [x] Entity basklass
- [x] Character basklass
- [x] PlayerCharacter med point-and-click
- [x] NPC klass

### 2.3 Dialog System ✅
- [x] DialogNode/DialogTree struktur
- [x] JSON-laddning via DataLoader
- [x] Dialog UI (textbox)
- [x] Valmöjligheter
- [x] DialogState overlay

### 2.4 Inventory System ✅
- [x] InventorySystem singleton
- [x] Item pickup/remove
- [x] InventoryState UI overlay
- [x] Item combination
- [x] JSON item definitions

### 2.5 Quest System ✅
- [x] Quest/Objective strukturer
- [x] ObjectiveTypes: Talk, Collect, Deliver, GoTo, Examine
- [x] QuestLogState UI overlay
- [x] Auto-completion detection
- [x] JSON quest definitions

**Estimerad tid: 4-5 sessioner**

---

## Fas 3: Advanced Features ✅ (Klar)

### 3.1 AI System ✅
- [x] AISystem singleton
- [x] BehaviorTypes: Idle, Patrol, Wander, Follow, GoTo
- [x] Waypoint-baserad patrol
- [x] Tidbaserade schedules
- [x] Game time med konfigurerbar skala
- [x] Automatisk NPC-registrering

### 3.2 Save/Load System ✅
- [x] SaveData struktur
- [x] JSON serialization (nlohmann/json)
- [x] Multiple save slots
- [x] Flags och counters
- [x] Speltid-tracking

### 3.3 Menu System ✅
- [x] MenuState (huvudmeny)
- [x] OptionsState (volym, fullscreen)
- [x] PauseState (overlay)
- [x] Global mute toggle (M-tangent)
- [ ] SaveLoadState (save/load meny)

### 3.4 UI System ✅
- [x] Widget basklass
- [x] Button med hover/click states
- [x] Label med fonts
- [x] Panel (container)
- [x] ProgressBar
- [x] Mute status indikator

### 3.5 Cutscene System ✅
- [x] CutsceneSystem singleton
- [x] Sekventiella actions
- [x] Text overlay med speaker
- [x] FadeIn/FadeOut
- [x] Factory methods för enkel skapning

### 3.6 Editor System ✅
- [x] EditorState (F1 för att öppna)
- [x] Visuell hotspot-redigering (drag, resize)
- [x] Walk area editing med handles
- [x] Depth scaling redigering (+/- tangenter)
- [x] JSON save/load av ändringar
- [x] Command pattern för undo/redo

### 3.7 Depth Scaling ✅
- [x] WalkArea med scaleTop/scaleBottom
- [x] PlayerCharacter::renderScaled()
- [x] Animation/SpriteSheet scaled rendering
- [x] Perspektiv-effekt baserat på Y-position

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

## Nästa session: Fas 4 - Content & Polish

### Fokus:
1. Sprite/textur assets
2. Bakgrundsbilder för rum
3. Ljudeffekter och musik
4. Komplett spelinnehåll

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
| M3 | Rum med spelare | ✅ |
| M4 | Dialog med NPC | ✅ |
| M5 | Inventory fungerar | ✅ |
| M6 | Quest system | ✅ |
| M7 | Save/Load | ✅ |
| M8 | Playable demo | ⬜ |
| M9 | Full game | ⬜ |
