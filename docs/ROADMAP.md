# Retro Adventure - Development Roadmap

## Faser

```
┌─────────────────────────────────────────────────────────────────────┐
│ Fas 1: Core       │ Fas 2: Systems  │ Fas 3: Advanced│ Fas 4: Content│
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ▓▓░░░░░░░░░░│
│ 100%              │ 100%            │ 100%           │ 15%          │
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
- [x] **Scene System** - Migrated from Room to Scene system
- [x] **SceneManager** - Replaced RoomManager
- [x] **Scene Consolidation** - Unified Scene class in engine::Scene (2026-01-06)

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

### 3.7 ImGui Editor ✅ (2026-01-05)
- [x] Dear ImGui integration med docking
- [x] Panel-baserad arkitektur (IEditorPanel)
- [x] HierarchyPanel - Objektträd
- [x] PropertiesPanel - Inspector
- [x] ViewportPanel - Rum-preview med zoom
- [x] AssetBrowserPanel - Grid/List view, filter, search
- [x] ConsolePanel - Logg med timestamps
- [x] **Drag-and-drop editing** - Hotspots, player spawn, walk area
- [x] **Safe save workflow** - Automatisk backup, validering
- [x] **Removed hot reload** - Restart-workflow istället (enklare, stabilare)
- [x] Docking layout - Flyttbara paneler
- [x] Windowed mode (1600x900 max, resizable)

### 3.8 Depth Scaling ✅
- [x] WalkArea med scaleTop/scaleBottom
- [x] PlayerCharacter::renderScaled()
- [x] Animation/SpriteSheet scaled rendering
- [x] Perspektiv-effekt baserat på Y-position

**Estimerad tid: 4-5 sessioner**

---

## Fas 4: Node Scene Graph & World System ✅ KOMPLETT

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Fas 4A: Core Nodes   │ Fas 4B: World System │ Fas 4C: Physics │ Fas 4D: Editor│
│ ████████████████████ │ ████████████████████ │ ████████████████████ │ ████████████████████│
│ 100%                 │ 100%                 │ 100%            │ 100%         │
└─────────────────────────────────────────────────────────────────────────────┘
```

## Fas 5: Actor-Based Architecture Migration ✅ STARTAT

```
┌─────────────────────────────────────────────────────────────────────────────┐
│ Fas 5A: World Hierarchy │ Fas 5B: Actor System │ Fas 5C: Components │ Fas 5D: Migration│
│ ████████████████████    │ ████████████████████ │ ████████████████████ │ ████████████████████│
│ 100%                    │ 100%                 │ 100%              │ 100%             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Arkitektur:** UE5-inspirerad Actor/Component-baserad design, migrerar bort från Node-systemet

### 5A: World Container Hierarchy ✅ KLART (2026-01-05)

#### 5A.1 WorldContainer Base Class ✅
- [x] Abstract base class för World/Level/Scene
- [x] Template Method Pattern för update()/render()
- [x] Gemensam actor-management
- [x] Gemensam name-management
- [x] Pure virtual interface

#### 5A.2 World Class Refactoring ✅
- [x] World : public WorldContainer
- [x] Hanterar Levels
- [x] Scene stack för overlays
- [x] Level transitions

#### 5A.3 Level Class Creation ✅
- [x] Level : public WorldContainer (NY KLASS)
- [x] Mellanled mellan World och Scene
- [x] Hanterar Scenes
- [x] Scene transitions
- [x] Lifecycle hooks (onLevelEnter/Exit)

#### 5A.4 Scene Refactoring ✅
- [x] Scene : public WorldContainer
- [x] BREAKING: Tog bort Node inheritance
- [x] Ren Actor-container
- [x] Camera via CameraComponent
- [x] Lifecycle hooks (onSceneEnter/Exit/Pause/Resume)

#### 5A.5 Migration Tools ✅
- [x] RoomToSceneConverter - RoomData → Actors
- [x] Hybrid rendering via RoomData i ViewportPanel
- [x] WorldViewPanel för navigation
- [x] LevelViewPanel för navigation
- [x] Disabled LayerEditorPanel (kräver Actor-omskrivning)

**Status:** ✅ KOMPLETT - World→Level→Scene hierarki implementerad

### 5B: Actor Base Classes ✅ KLART (2026-01-05)

#### 5B.1 Object & ActorObject Foundation ✅
- [x] `Object` - Root base class för alla objekt
- [x] `ActorObject` - Base för alla actors (utan Node-arv)
- [x] Transform (position, rotation, scale)
- [x] Visibility & lifecycle hooks
- [x] Compatibility med legacy Node2D (getGlobalPosition, isVisible)

#### 5B.2 Specialized Actor Classes ✅
- [x] `CharacterActor` - Base för Player/NPCs
  - [x] `PlayerActor` - Spelarkaraktär
  - [x] `NPCActor` - NPC karaktärer
- [x] `EnvironmentActor` - Base för miljöobjekt
  - [x] `TileMapActor` - Tile maps
  - [x] `ParallaxActor` - Parallax layers
  - [x] `PropActor` - Statiska props
- [x] `SpriteActor` - Actor med SpriteComponent rendering

#### 5B.3 Legacy Compatibility ✅
- [x] VisualActor, InteractiveActor, ItemActor uppdaterade
- [x] Legacy Nodes (Sprite, Label, etc) använder ActorObject via Node2D
- [x] CoreRedirects uppdaterad (Object/ActorObject är riktiga klasser)

**Status:** ✅ KOMPLETT - Actor class hierarchy implementerad

### 5C: Component System ✅ KLART (2026-01-05)

#### 5C.1 SpriteComponent - Fullständig Implementation ✅
- [x] Texture management (setTexture, source rect)
- [x] Origin/pivot point för rotation/scale
- [x] Flip (horizontal/vertical)
- [x] Tint & opacity support
- [x] Scale support från owner actor
- [x] Full SDL_RenderCopyEx rendering

#### 5C.2 AnimationComponent - Fullständig Implementation ✅
- [x] SDL_Rect frame-baserad animation
- [x] Animation management (add, has, play, stop, pause)
- [x] Speed control (playback speed multiplier)
- [x] Loop vs one-shot animations
- [x] Frame access (getCurrentFrameRect, frame index)
- [x] Restart control för play()

#### 5C.3 SpriteActor Integration ✅
- [x] initializeSprite() laddar texturer via TextureManager
- [x] Auto-query texture size
- [x] Auto-setup source rect
- [x] initializeAnimation() för animation support

**Status:** ✅ KOMPLETT - Component-baserad rendering funktionell

### 5D: Full Migration ✅ KLART (2026-01-05)

#### 5D.1 RoomToSceneConverter → SpriteActors ✅
- [x] Background konverteras till SpriteActor med texture loading
- [x] Hotspots konverteras till InteractiveActor
- [x] Player spawn & walk area som PropActors

#### 5D.2 ViewportPanel → Actor Rendering ✅
- [x] renderSceneActors() implementation
- [x] SpriteComponent rendering via ImGui
- [x] Debug visualization för actors utan sprites
- [x] Ta bort RoomData hybrid-rendering

#### 5D.3 Legacy Node System Deprecation ✅
- [x] Node.h markerad som DEPRECATED med #pragma message
- [x] Node2D.h markerad som DEPRECATED med #pragma message
- [x] CoreRedirects.h uppdaterad med deprecation warnings
- [x] ViewportPanel includes organiserade (legacy nodes sist)
- [x] LEGACY_NODES.md dokumentation skapad

#### 5D.4 Actor Hierarchy Consolidation ✅
- [x] VisualActor → ActorObjectExtended
- [x] InteractiveActor → ActorObjectExtended
- [x] ItemActor → ActorObjectExtended

#### 5D.5 Node System Removal ✅
- [x] Ta bort Node.h, Node2D.h, Node.cpp, Node2D.cpp HELT
- [x] Ta bort renderSceneNode() från ViewportPanel
- [x] Migrera Camera2D, PhysicsBody, Layer, KinematicBody → ActorObject
- [x] SceneGraphPanel: renderNodeTree → renderActorTree
- [x] CoreRedirects.h: Rensa bort Node typedefs

**Status:** ✅ 100% - Node system helt borttaget, Actor-baserad arkitektur komplett

**Arkitektur:** OOP Scene Graph (inspirerat av Godot, Cocos2d-x, SFML)

### 4A: Core Node System 🔴 HÖG PRIORITET

**Mål:** Skapa Node-baserad hierarki för alla spelobjekt

#### 4A.1 Base Node Classes ✅
- [x] `Node` - Basklass med transform, hierarki, lifecycle
- [x] `Node2D` - 2D-specifik node med position, rotation, scale
- [x] `Vec2` struct för matematiska operationer
- [x] Parent/child relationer med transform-inheritance
- [x] Z-index för renderingsordning
- [x] `onEnter()`, `onExit()`, `update()`, `render()` callbacks

#### 4A.2 Visual Nodes ✅
- [x] `Sprite` - Textur-rendering med tint, flip, src rect
- [x] `AnimatedSprite` - Animation state machine
- [x] `Label` - Text-rendering med FontManager
- [x] `TileMapLayer` - Grid-baserad tile rendering
- [x] `ParallaxLayer` - Parallax scrolling support

#### 4A.3 Camera System ✅
- [x] `Camera2D` - Viewport med follow, zoom, limits
- [x] Smooth follow med damping
- [x] Screen shake support
- [x] World-to-screen / screen-to-world koordinater

**Estimerad tid: 2-3 sessioner**

---

### 4B: World & Scene Management 🟡 MEDIUM PRIORITET

**Mål:** Scen-baserad världshantering

#### 4B.1 Scene System ✅
- [x] `Scene` - Container för alla nodes i en level
- [x] `World` - Manages multiple scenes
- [x] Scene transitions (fade, slide, etc.)
- [x] Active scene stack för overlays

#### 4B.2 Layer Management ✅
- [x] Background layers (parallax)
- [x] Main game layer
- [x] Foreground/overlay layers
- [x] UI layer (screen-space)

#### 4B.3 Scene Serialization ✅
- [x] JSON scene format
- [x] SceneLoader för att ladda från fil
- [x] Integrera med befintliga rooms.json (optional - kan göras vid behov)

**Estimerad tid: 2 sessioner**

---

### 4C: Physics & Collision 🟡 MEDIUM PRIORITET

**Mål:** Robust kollision och enkel fysik

#### 4C.1 Collision Shapes ✅
- [x] `CollisionShape` basklass
- [x] AABB (axis-aligned bounding box)
- [x] Circle collision
- [x] Polygon collision (grundläggande)

#### 4C.2 Physics Bodies ✅
- [x] `PhysicsBody` - Base med velocity, acceleration, friction
- [x] `KinematicBody` - Kontrollerad rörelse (spelare, NPCs)
- [x] `moveAndSlide()` för smooth collision response
- [x] Collision callbacks (onCollisionEnter, onCollisionStay, onCollisionExit)

#### 4C.3 Collision Detection ✅
- [x] Shape overlap detection (AABB vs AABB, Circle vs Circle, AABB vs Circle)
- [x] Spatial partitioning (grid-baserad med SpatialGrid)
- [x] Broad phase / narrow phase optimization (via SpatialGrid)

**Estimerad tid: 2-3 sessioner**

---

### 4D: Editor Integration 🟢 SENARE

**Mål:** Visuell redigering av nya node-typer

#### 4D.1 Visual Layer Editor ✅
- [x] LayerEditorPanel med layer list
- [x] Layer properties (type, z-order, opacity)
- [x] Add/remove layers dialog
- [x] Parallax preview med scroll slider och visuell representation
- [ ] Drag-and-drop layers i ViewportPanel (kan implementeras vid behov)

#### 4D.2 TileMap Editor ✅
- [x] TileMapEditorPanel med tile palette
- [x] Paint/erase mode toggle
- [x] Collision mode toggle
- [x] Interactive grid painting med mouse
- [x] Auto-tiling baserat på grannar (neighbor-based)

#### 4D.3 Scene Graph Panel ✅
- [x] Hierarkisk vy av alla nodes
- [x] Node context menu (rename, duplicate, delete)
- [x] Active/Visible toggles
- [x] Drag-and-drop re-parenting med ownership transfer
- [x] Node properties inspector (via PropertiesPanel)

**Estimerad tid: 3-4 sessioner**

---

### Ny Mappstruktur

```
src/engine/
├── core/                    # NYA
│   ├── Node.h/cpp
│   ├── Node2D.h/cpp
│   └── Vec2.h
│
├── nodes/                   # NYA
│   ├── Sprite.h/cpp
│   ├── AnimatedSprite.h/cpp
│   ├── TileMapLayer.h/cpp
│   ├── ParallaxLayer.h/cpp
│   └── Label.h/cpp
│
├── physics/                 # NYA
│   ├── CollisionShape.h/cpp
│   ├── PhysicsBody.h/cpp
│   └── CollisionWorld.h/cpp
│
├── world/                   # NYA
│   ├── Scene.h/cpp
│   ├── World.h/cpp
│   ├── Camera2D.h/cpp
│   └── SceneLoader.h/cpp
│
└── [befintliga mappar...]
```

---

## Fas 5: Editor Development & Content Creation

### 4A: Property Editing (Grundläggande Redigering) 🔴 PÅGÅENDE

**Mål:** Alla data-typer kan redigeras via Properties-panelen

#### 4A.1 Room Properties Editor 🔴 Hög prioritet
- [ ] InputText för room namn
- [ ] InputText för background path
- [ ] InputFloat2 för player spawn position
- [ ] Slider för walk area bounds
- [ ] Slider för depth scale (top/bottom)
- [ ] Button: "Add Hotspot"
- [ ] Button: "Delete Room" (med bekräftelse)

#### 4A.2 Hotspot Editor 🔴 Hög prioritet
- [ ] Dropdown för hotspot type (Exit, NPC, Item, Examine)
- [ ] InputText för id, name
- [ ] InputInt4 för position/size (x, y, w, h)
- [ ] InputText för targetRoom (om Exit)
- [ ] InputText för dialogId (om NPC)
- [ ] InputText för itemId (om Item)
- [ ] TextArea för examineText
- [ ] Button: "Delete Hotspot"

#### 4A.3 Dialog Node Editor 🔴 Hög prioritet
- [ ] InputText för dialog id
- [ ] InputText för npc name
- [ ] Lista över noder (selectable)
- [ ] InputText för speaker
- [ ] TextArea för node text
- [ ] Lista över choices (add/remove)
- [ ] InputText för choice text
- [ ] InputInt för nextNodeId
- [ ] Button: "Add Node", "Delete Node"
- [ ] Button: "Add Choice", "Delete Choice"

#### 4A.4 Quest Editor 🟡 Medium prioritet
- [ ] InputText för quest id, title
- [ ] TextArea för description
- [ ] Lista över objectives (add/remove)
- [ ] Dropdown för objective type
- [ ] InputText för objective description
- [ ] InputText för target (targetId, targetRoom, etc)
- [ ] Button: "Add Objective", "Delete Objective"

#### 4A.5 Item Editor 🟡 Medium prioritet
- [ ] InputText för item id, name
- [ ] TextArea för description
- [ ] InputText för icon path
- [ ] Checkbox för combinable
- [ ] Lista över combinations (add/remove)
- [ ] Button: "Add Item", "Delete Item"

#### 4A.6 NPC Editor 🟡 Medium prioritet
- [ ] InputText för npc id, name
- [ ] Dropdown för AI behavior type
- [ ] Lista över waypoints (add/remove)
- [ ] Lista över schedules (add/remove)
- [ ] InputText för dialog id
- [ ] Button: "Add NPC", "Delete NPC"

#### 4A.7 Create/Delete Functionality 🔴 Hög prioritet
- [ ] "New Room" button i HierarchyPanel
- [ ] "New Dialog" button i HierarchyPanel
- [ ] "New Quest" button i HierarchyPanel
- [ ] "New Item" button i HierarchyPanel
- [ ] Delete confirmation dialog
- [ ] Validering av dependencies (varna om brutna länkar)

#### 4A.8 Save System 🔴 Hög prioritet
- [ ] EditorContext::saveToFiles() implementation
- [ ] Save alla rooms.json ändringar
- [ ] Save alla dialogs.json ändringar
- [ ] Save alla quests.json ändringar
- [ ] Save alla items.json ändringar
- [ ] Save alla npcs.json ändringar
- [ ] Backup före sparning (.bak)
- [ ] Status message efter save
- [ ] Ctrl+S shortcut

**Estimerad tid: 2-3 sessioner**

---

### 4B: Visual Editors (Avancerade Verktyg) 🟡 PLANERAD

**Mål:** Visuella verktyg för komplexa datatyper

#### 4B.1 Node Graph System 🔴 Hög prioritet (PÅGÅENDE)

**Arkitektur:**
- [ ] vcpkg: imnodes dependency (byta från imgui-node-editor)
- [ ] Uppdatera CMakeLists.txt för imnodes
- [ ] Skapa IGraphNode base class
- [ ] Skapa INodeGraphPanel base class
- [ ] Skapa Link struct

**Dialog Graph:**
- [ ] DialogNode implementation
- [ ] DialogGraphPanel (imnodes-baserad)
- [ ] Automatisk tree layout
- [ ] Visuella kopplingar mellan noder
- [ ] Zoom och pan
- [ ] Sync till/från DialogData

**Quest Graph (framtida):**
- [ ] QuestNode implementation
- [ ] QuestGraphPanel
- [ ] Objective-noder

**NPC Behavior Graph (framtida):**
- [ ] BehaviorNode implementation
- [ ] BehaviorGraphPanel
- [ ] AI behavior trees

#### 4B.2 Room Flowchart 🟡 Medium prioritet
- [ ] Visuell karta över alla rum
- [ ] Visa exits som pilar
- [ ] Click för att öppna rum
- [ ] Overview av hela spelet

#### 4B.3 Quest Chain Editor 🟡 Medium prioritet
- [ ] Visuell quest-sekvens
- [ ] Visa dependencies
- [ ] Trigger-visualisering

#### 4B.4 Animation Preview 🟢 Låg prioritet
- [ ] Förhandsvisning av sprites
- [ ] Animation playback
- [ ] Frame-by-frame stepping

#### 4B.5 Audio Preview 🟢 Låg prioritet
- [ ] Spela upp ljud i editorn
- [ ] Waveform-visualisering
- [ ] Volume test

**Estimerad tid: 3-4 sessioner**

---

### 4C: Workflow & Polish 🟢 FRAMTIDA

**Mål:** Professionell arbetsflödesupplevelse

#### 4C.1 Undo/Redo System 🔴 Hög prioritet
- [ ] Command pattern för alla ändringar
- [ ] Undo stack (Ctrl+Z)
- [ ] Redo stack (Ctrl+Y)
- [ ] History panel (visa alla commands)

#### 4C.2 Hot Reload 🟡 Medium prioritet
- [ ] FileWatcher för JSON-filer
- [ ] Automatisk reload vid ändringar
- [ ] Notification vid reload

#### 4C.3 Validation System 🟡 Medium prioritet
- [ ] Validera alla dialogId-referenser
- [ ] Validera alla targetRoom-referenser
- [ ] Validera alla itemId-referenser
- [ ] Varna för saknade assets
- [ ] Validation panel med errors/warnings

#### 4C.4 Search & Filter 🟡 Medium prioritet
- [ ] Global search (Ctrl+F)
- [ ] Sök i alla data-typer
- [ ] Filter i HierarchyPanel
- [ ] "Find references" för objekt

#### 4C.5 Project Settings 🟢 Låg prioritet
- [ ] Game settings panel
- [ ] Build configuration
- [ ] Export settings

#### 4C.6 Play-in-Editor 🟢 Låg prioritet
- [ ] Starta spelet från editorn
- [ ] Quick test av rum
- [ ] Debug overlay

**Estimerad tid: 2-3 sessioner**

---

### 4D: Game Content (Efter Editor är Klar)

#### 4D.1 Storyline & Design
- [ ] Komplett story outline
- [ ] Character designs
- [ ] Puzzle designs

#### 4D.2 Content Creation
- [ ] Alla rum (5-10 för demo)
- [ ] Alla NPCs med dialoger
- [ ] Alla items
- [ ] Alla quests

#### 4D.3 Art Assets
- [ ] Character sprites
- [ ] NPC sprites
- [ ] Background art
- [ ] UI grafik
- [ ] Item ikoner

#### 4D.4 Audio Assets
- [ ] Background music
- [ ] Ambient sounds
- [ ] UI sounds

#### 4D.5 Polish & Testing
- [ ] Bug fixes
- [ ] Balancing
- [ ] Playtesting

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

## Nästa session: Fas 4A - Property Editing

### Fokus:
1. Gör PropertiesPanel editerbar (InputText, sliders, buttons)
2. Room properties: namn, background, spawn, walk area
3. Hotspot properties: typ, position, länkar
4. Save-funktionalitet för alla ändringar

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
