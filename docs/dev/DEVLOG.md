# DEVLOG - Retro Adventure

Kronologisk logg av alla ändringar sedan projektets start.

**Format:** `[HASH]` type(scope): beskrivning

---

## 2026-01-05

### Kväll (Sen)

- `e28d059` feat(editor): add FileBrowser and enhance HierarchyPanel for Actor system
  - **Editor:** FileBrowser.h/cpp för native file dialogs
  - **Editor:** HierarchyPanel utökad med ActorObjectExtended support
  - **Engine:** Scene.h utökad med Actor management metoder
  - **Editor:** PropertyEditorUtils förbättrad för better editing
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `1a473eb` refactor(core): remove Node system, migrate to ActorObject
  - **BREAKING:** Node.h, Node2D.h, Node.cpp, Node2D.cpp RADERADE helt
  - **Arkitektur:** Object base class (abstract root)
  - **Arkitektur:** ActorObject base class (replaces Node2D)
  - **Nya Actors:** CharacterActor, EnvironmentActor, SpriteActor
  - **Migrering:** Camera2D, PhysicsBody, Layer, KinematicBody ? ActorObject
  - **Migrering:** SceneGraphPanel: renderNodeTree ? renderActorTree
  - **Migrering:** ViewportPanel: renderSceneNode borttagen
  - **Components:** SpriteComponent, AnimationComponent förbättrade
  - **Build:** Alla Node-relaterade filer kommenterade ut i CMakeLists.txt
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `8404603` refactor(architecture): implement World?Level?Scene hierarchy with WorldContainer base
  - **Arkitektur:** WorldContainer abstract base class - gemensam funktionalitet för World/Level/Scene
  - **Arkitektur:** World : public WorldContainer - hanterar Levels och scene stack
  - **Arkitektur:** Level : public WorldContainer - ny klass som mellanled, hanterar Scenes
  - **Arkitektur:** Scene : public WorldContainer - INGEN Node-koppling, ren Actor-container
  - **Breaking Change:** Scene ärver inte längre från Node - all Node-funktionalitet borttagen
  - **Pattern:** Template Method Pattern i WorldContainer med pure virtual update()/render()
  - **Migration:** RoomToSceneConverter skapar ActorObjectExtended istället för Nodes
  - **Migration:** ViewportPanel hybridlösning - renderar via RoomData tills SpriteComponents klara
  - **Migration:** Disabled LayerEditorPanel (kräver omskrivning för Actor-system)
  - **Migration:** SceneGraphPanel disabled Node tree rendering, visar Actor count istället
  - **Editor:** WorldViewPanel för World/Level navigation
  - **Editor:** LevelViewPanel för Level/Scene navigation
  - **Build:** WorldContainer.h (header-only), Level.cpp/h, RoomToSceneConverter.cpp/h
  - **Cleanup:** NodeExample.cpp borttagen (oanvänd)
  - **Cleanup:** getting-started.md borttagen (obsolete guide)
  - **Problems Solved:** 4 stora migrations-problem (se session rapport)
  - **Statistik:** 90+ filer ändrade, 1,215 insertions, 531 deletions, 7 nya filer
  - **UE5 Alignment:** World=UWorld, Level=ULevel, Scene=Sub-level, Actor=AActor
  - Session: [2026-01-05](sessions/2026-01-05.md)

### Kväll
- `4783d89` feat(engine): complete Fas 4 Node Scene Graph system with advanced features
  - **Arkitektur:** Nya mappar src/engine/physics/, src/editor/panels/ med 7 nya filer
  - **Build:** SpatialGrid, SceneGraphPanel, LayerEditorPanel, TileMapEditorPanel i CMakeLists.txt
  - **Physics:** SpatialGrid för spatial partitioning och broad-phase collision detection
  - **SceneGraphPanel:** Drag-and-drop re-parenting med ownership transfer (unique_ptr)
  - **LayerEditorPanel:** Parallax preview med scroll slider och visuell representation
  - **TileMapEditorPanel:** Interactive grid painting med mouse input och auto-tiling
  - **Features:** Auto-tiling algorithm baserat på neighbor detection (4 cardinal directions)
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `f260c3b` feat(engine): implement Node Scene Graph system with physics
  - **Arkitektur:** Ny mappstruktur `src/engine/core/`, `nodes/`, `world/`, `physics/`
  - **Build:** 16 nya klasser i CMakeLists.txt
  - **Core System:** Vec2, Node (hierarki, z-index, lifecycle), Node2D (transform)
  - **Visual Nodes:** Sprite, AnimatedSprite, TileMapLayer, ParallaxLayer, Label
  - **World Management:** Camera2D (follow, zoom, shake), Scene, World (transitions), LayerManager, SceneLoader (JSON)
  - **Physics:** CollisionShape (AABB/Circle/Polygon), PhysicsBody, KinematicBody (moveAndSlide)
  - **Features:** Scene transitions, JSON serialization, smooth collision response
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `f073beb` feat(editor): migrate to imnodes for node graph system
  - **Arkitektur:** Ny `src/editor/graphs/` struktur med base classes
  - **imnodes:** Ersatte imgui-node-editor med imnodes (lokal kopia i vendor/)
  - **Base classes:** IGraphNode, INodeGraphPanel, Link för återanvändbar kod
  - **DialogGraphPanel:** Dialog-noder med tree layout
  - **QuestGraphPanel:** Quest header + objective-noder
  - **BehaviorGraphPanel:** NPC-beteende med dialog/room-kopplingar
  - **Tree Layout:** Horisontell layout baserad på djup
  - **NPC Selection:** Fixat val i HierarchyPanel + PropertyEditor
  - **Dock Layout:** Alla graph panels som tabs i center
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `0196f2f` feat(editor): implement OOP property editors with breadcrumb navigation
  - **Architecture:** Created IPropertyEditor interface + 6 concrete editors
  - **Property Editors:** Room, Hotspot, Dialog, Quest, Item, NPC med full CRUD
  - **Breadcrumb Navigation:** Hierarkisk navigation (Rooms > The Rusty Anchor > Bartender)
  - **Selection Management:** Fixed multi-selection bug, added deselect on re-click
  - **Dropdown Improvements:** ID references now use dropdowns (NPCs, Items, Rooms, Dialogs)
  - **Helper Functions:** PropertyEditorUtils::IdCombo för återanvändbar dropdown-logik
  - **Data Loading:** Fixed EditorContext.loadFromDataLoader() i EditorState::enter()
  - **UX:** Validation, dirty tracking, save/revert per editor
  - **Files:** 14 nya filer i src/editor/properties/, uppdaterade PropertiesPanel + HierarchyPanel
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `5c4fd4d` docs(workflows): enforce complete change analysis in all workflows
  - Uppdaterade git-commit, update-docs, end-session workflows
  - Obligatorisk `git diff --stat` och ändringsanalys
  - Krav på dokumentation av arkitektur, build och features
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `2c38bfa` docs: add complete architecture documentation for engine/game/editor split
  - ADR 005: Separate Engine, Game and Editor
  - Dokumenterade modulär projektstruktur (300+ filer)
  - Dokumenterade build-system
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `9f11b6b` docs: update DEVLOG, ROADMAP, CHANGELOG and session report
  - Dokumenterade drag-and-drop och hot reload removal
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `cf1940c` feat(editor): implement drag-and-drop editing with safe save workflow
  - **Arkitektur:** Separerade `src/` i `engine/`, `game/`, `editor/`
  - **Build:** RetroCore (static lib), RetroGame (exe), RetroEditor (exe)
  - **Drag-and-drop:** Hotspots, player spawn och walk area i ViewportPanel
  - **MSVC fix:** C2597 bug (ImVec2 ? float parametrar)
  - **Hot reload borttaget:** Restart-workflow istället (enklare, stabilare)
  - **Safe save:** Automatisk backup (.bak), validering mot tom data
  - Session: [2026-01-05](sessions/2026-01-05.md)

- `41c68e1` docs: add robust documentation system with CHANGELOG, ADR, and restructured folders
  - CHANGELOG.md (Keep a Changelog standard)
  - ADR i docs/dev/decisions/
  - Flyttat sessions till docs/dev/sessions/
  - Git-policy rules och synkade workflows

### Eftermiddag
- `401647c` feat(editor): ImGui panel-based editor architecture
  - Dear ImGui integration med docking support
  - 5 nya paneler: Hierarchy, Properties, Viewport, AssetBrowser, Console
  - EditorContext för shared panel state
  - Windowed mode för editor (1600x900 max)

- `5bdfd02` feat(build): separate editor into standalone executable
  - RetroCore static library
  - RetroEditor standalone exe

### Förmiddag
- `cbc0ce3` docs: update session report with evening work
- `22b59b4` refactor: extract EditorState into smaller components
  - VisualRoomEditor, RoomDataManager, EditorTabRenderer, TiledIntegration
  - EditorState.cpp: 1567  600 rader (-61%)
- `b97942e` fix: enable dragging player spawn marker in editor
- `17f2bf0` feat: add player spawn editing and NPC hotspot positioning
- `1f69fa6` fix: use room walk area instead of hardcoded values

### Morgon
- `a692225` docs: add comprehensive project documentation
- `29a2744` docs: update documentation for depth scaling and editor features
- `a37ab7c` fix: adjust walkArea, depth scaling and font sizes
- `6f4c5f4` feat: add perspective depth scaling for characters
  - WalkArea med scaleTop/scaleBottom
  - PlayerCharacter::renderScaled()
- `832a399` fix: save hotspots to rooms.json

---

## 2026-01-04

### Kväll - Editor & Visual Tools
- `a782a4b` feat: add mute toggle and visual editor resize/walk area
- `f9dfa41` feat(editor): complete visual hotspot editor with drag-and-drop
- `6530af2` feat(editor): add modular editor architecture
- `3da956b` feat(editor): add visual room editor
- `36b725c` fix(rendering): load and render room layers correctly
- `f5bc119` feat(editor): complete room editor with full functionality
- `1b74459` feat(editor): implement room editor UI
- `8b07728` feat(content): add tavern-inside.png as tavern background
- `32c06d4` feat(editor): make Tiled workflow optional
- `c3c397f` feat(rendering): add multi-layer rendering with depth
- `0b29b0b` feat(editor): add Import/Export Tiled buttons
- `7421493` feat(data): add TiledImporter for room editing workflow
- `867e524` fix(editor): load game data when entering EditorState
- `1854c1d` feat(editor): add EditorState accessible from main menu

### Eftermiddag - Advanced Systems (Fas 3)
- `35e8ba3` feat(systems): add IWorldQuery/IWorldMutator interfaces
- `88ecb02` feat(systems): add WorldState, Conditions, EventBus
- `6171de2` feat(systems): implement Fas C - Polish & Feel features
- `6c1fbf1` feat(systems): implement Fas B.3 - Recap barks system
- `5981329` feat(systems): implement Fas B - Immersion features
- `7b46774` feat(systems): implement Fas A - Anti-Frustration features
- `c33f8d1` docs(design): add LucasArts-inspired extensions roadmap
- `8336771` docs(design): add LucasArts insights applied to our game
- `6cf8e3c` docs(workflow): enhance git-commit workflow

### Förmiddag - Debugging & Logger
- `ef7b6f1` fix(states): prevent PlayState from reloading data on resume
- `a3a5781` feat(debug): add detailed logging to PlayState, RoomManager, DialogSystem
- `ad29f4f` fix(font): add fallback text rendering when font is missing
- `049e73f` fix(logger): create log directory automatically
- `eee6d20` feat(utils): add Logger system with file and console output
- `9999155` feat(input): add E key proximity interaction for hotspots
- `9d62273` content: add extensive test content (7 rooms, 8 NPCs, 20 items, 7 quests, 8 dialogs)

### Morgon - Core Systems (Fas 2 & 3)
- `33b8b29` docs: update session report with Fas 4 progress
- `87eed69` fix(audio): silence missing sound warnings
- `06369b9` feat(state): add SaveLoadState with save slot UI
- `05ed215` docs: update documentation for Fas 3 completion
- `b261ffa` docs: add session report 2026-01-05
- `23136ab` feat(ui): add Widget system with Label, Button, Panel, ProgressBar
- `10ea0d1` feat(cutscene): add CutsceneSystem for scripted sequences
- `8c0bb9a` feat(save): add SaveSystem with JSON serialization
- `5182333` feat(ai): add AISystem with NPC behaviors and schedules
- `00eb620` docs: add session report 2026-01-04

### Tidig Morgon - Fas 1 & 2 Foundation
- `b166e1b` feat(data): add JSON-based data loading system with nlohmann/json
- `24e34ff` feat(quests): add Quest System with objectives and quest log
- `1fe9f34` feat(inventory): add Inventory UI overlay
- `6590f56` feat(rooms): add RoomManager, room transitions, and NPC class
- `ab94bfd` feat(graphics): implement DPI-aware font rendering
- `b418585` feat(video): add resolution settings with auto-detection
- `3c8ffe0` feat: add Inventory System and fullscreen mode
- `ea1c89b` feat(dialog): implement Dialog System with choices
- `e8603a3` feat(player): add sprite sheet and animation support
- `87a62cd` feat(room): implement Room System with hotspots
- `d6c2112` feat: add FontManager and fix state change crash
- `9118f70` feat(states): add PauseState - Fas 1 complete!
- `379355f` refactor(entities): implement Entity hierarchy with OOP
- `e28bb11` docs: add documentation structure and rules
- `3432e17` docs: add workspace rules and update sdl2-patterns trigger
- `fe1dc52` feat: add gamepad support, options menu, and OOP architecture docs
- `340f33a` feat(audio): add menu music with fade out on exit
- `7603933` docs: update ROADMAP - Fas 1 now 75%, Audio done
- `2466194` feat(audio): add AudioManager with music and SFX support
- `031acc2` docs: update ROADMAP and ARCHITECTURE progress
- `94d250e` feat(graphics): add TextureManager, SpriteSheet, Animation
- `e411fa3` feat: initial C++ SDL2 project with StateManager
  - **PROJEKTSTART**
  - SDL2, SDL2_image, SDL2_mixer setup
  - CMake + vcpkg integration
  - Game loop, StateManager, MenuState, PlayState

---

## Statistik

| Datum | Commits | Highlights |
|-------|---------|------------|
| 2026-01-04 | 63 | Fas 1-3 komplett, Editor, Test content |
| 2026-01-05 | 13 | ImGui editor, Dokumentationssystem |
| **Totalt** | **76** | |

---

## Fas-progress

| Fas | Status | Datum |
|-----|--------|-------|
| Fas 1: Core |  100% | 2026-01-04 |
| Fas 2: Systems |  100% | 2026-01-04 |
| Fas 3: Advanced |  100% | 2026-01-04 |
| Fas 4: Content |  15% | Pågående |



## 2026-01-06

- cc4e398 feat(engine): complete Room to Scene migration
  - **Arkitektur:** Added Scene.h/cpp and SceneManager.h/cpp to replace Room system
  - **Arkitektur:** All systems now use SceneManager instead of RoomManager
  - **Actors:** Added actors/NPC.h/cpp with getX/getY/setSpeed/getSpeed methods
  - **Actors:** Extended CharacterActor with setSpeed/getSpeed methods
  - **Cleanup:** Removed legacy entities/NPC.h
  - **Systems:** Updated PlayState, AISystem, SaveSystem, ConditionSystem, CutsceneSystem, WorldQuery, DebugEditor
  - **Data:** GameDataLoader now loads scenes to SceneManager only (removed double loading)
  - **Build:** Updated CMakeLists.txt to exclude entities/NPC.h
  - Session: [2026-01-06](sessions/2026-01-06.md)

- c83203 refactor(scene): consolidate Scene classes and remove RoomToSceneConverter
  - **Arkitektur:** Konsoliderade tvÃ¥ Scene-klasser till en engine::Scene i src/engine/world/
  - **Arkitektur:** Tog bort RoomToSceneConverter - anvÃ¤nds Scene::createFromData istÃ¤llet
  - **Feature:** Background actor med SpriteComponent skapas i createFromData
  - **Feature:** Legacy API bevarad fÃ¶r backward compatibility
  - **Fixed:** Uppdaterade alla includes frÃ¥n engine/Scene.h till engine/world/Scene.h
  - **Fixed:** Bakgrund laddas nu i editorn via loadBackground()
  - Session: [2026-01-06](sessions/2026-01-06.md)
