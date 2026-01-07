# Changelog

Alla viktiga ändringar i detta projekt dokumenteras här.

Formatet följer [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
och projektet använder [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Changed
- **Editor folder structure** - Complete reorganization from flat to hierarchical layout
- **EditorState** refaktorerad från 998 till 211 rader (-79%) med separation of concerns
- **ViewportPanel** uppdelad i 6 modulära filer (Core, World, Level, Scene, Actors, Input)
- **World interaction** - Consistent 1-click=select/drag, 2-click=enter on all hierarchy levels

### Added
- **World/Level/Scene hierarchy** - Top-down persistence with world.json loading/saving
- **ActorData support** - Actors can be placed on World, Level, and Scene levels
- **EditorWorldManager** - Manages world.json loading, saving, and engine synchronization
- **Level drag functionality** - Levels can be dragged in World spatial view with grid snapping
- **Grid position persistence** - Level positions saved to world.json, scenes to scenes.json

### Added
- **Hierarchical editor structure** - core/, managers/, panels/*/, properties/*/, legacy/, ui/, input/, data/, commands/, tools/, widgets/
- **EditorPanelManager** - Centraliserad panel creation och management
- **EditorWorldManager** - World/Level/Scene setup från RoomData
- **EditorEventDispatcher** - SDL event handling och shortcuts
- **PlaceActorsPanel** - UE5-style drag-and-drop för actor templates
- **EditorInputHandler** - Windows shortcuts (Del, Ctrl+Z/Y/D/S)
- **WorldContainer.removeActor()** - Metod för att ta bort actors från containers

### Fixed
- **ImGui dockspace** - Uses actual window size instead of hardcoded values
- **Fullscreen artifacts** - Added SDL_RenderClear() to prevent visual artifacts
- **Include paths** - Fixed all includes after folder restructuring

### Added
- **Scene** and **SceneManager** - New scene system replacing Room system
- **actors/NPC** - New NPC class with getX/getY/setSpeed/getSpeed methods
- **Hotspot.h** - Centralized hotspot definitions (HotspotType, Hotspot, WalkArea, RoomLayer)
- **FileBrowser** - Native file dialog integration för editorn
- **ActorObjectExtended support** - HierarchyPanel utökad för Actor system
- **Actor management methods** - Scene.h utökad med Actor-funktionalitet
- **Object** abstract root base class för alla engine-objekt
- **ActorObject** concrete actor base class (ersätter Node2D)
- **CharacterActor**, **EnvironmentActor**, **SpriteActor** specialiserade actors
- **WorldContainer** abstract base class för World/Level/Scene hierarki med template method pattern
- **Level** class som mellanled mellan World och Scene, hanterar scene transitions
- **WorldViewPanel** och **LevelViewPanel** för hierarki-navigation i editorn
- **RoomToSceneConverter** för att konvertera RoomData till Actor-baserade Scenes
- **Hybrid rendering** i ViewportPanel - renderar via RoomData tills SpriteComponents är klara
- **Node Scene Graph System** - OOP-baserad scene graph (inspirerat av Godot/Cocos2d-x)
- **Vec2** - 2D vektor-matematik med operators, lerp, rotation, distance
- **Node** - Base class med parent/child hierarki, z-index, lifecycle callbacks
- **Node2D** - 2D transform (position, rotation, scale) med global/local koordinater
- **Sprite** - Textur-rendering med tint, flip, source rect, origin
- **AnimatedSprite** - Animation state machine med callbacks, frame control
- **TileMapLayer** - Grid-baserad tilemap med collision detection
- **ParallaxLayer** - Parallax scrolling med repeat X/Y
- **Label** - Text rendering med alignment och color
- **Camera2D** - Viewport med follow, zoom, limits, screen shake, koordinat-konvertering
- **Scene** - Container för game levels med camera management
- **World** - Multi-scene manager med transitions och scene stack
- **LayerManager** - Layer system (Background, Main, Foreground, UI)
- **SceneLoader** - JSON scene serialization och loading
- **CollisionShape** - AABB, Circle, Polygon collision shapes
- **PhysicsBody** - Physics med velocity, acceleration, friction, mass
- **KinematicBody** - moveAndSlide() för smooth collision response
- **SceneGraphPanel** - Hierarkisk node tree viewer med context menu och drag-and-drop re-parenting
- **LayerEditorPanel** - Layer management med properties, add/remove och parallax preview
- **TileMapEditorPanel** - TileMap editor med palette, interactive painting, auto-tiling
- **SpatialGrid** - Grid-baserad spatial partitioning för effektiv collision detection
- **Drag-and-Drop Re-parenting** - Node ownership transfer i scene graph
- **Interactive Grid Painting** - Click-and-drag tile painting i TileMapEditor
- **Auto-tiling Algorithm** - Neighbor-based tile variant selection
- **Parallax Preview** - Live scrolling preview för background layers
- **imnodes Node Graph System** - Ersatte imgui-node-editor med imnodes
- **Graph Base Classes** - IGraphNode, INodeGraphPanel, Link för återanvändbar arkitektur
- **DialogGraphPanel** - Visualiserar dialog-träd med noder och kopplingar
- **QuestGraphPanel** - Quest header + objective-noder i sekvens
- **BehaviorGraphPanel** - NPC-beteende med dialog/room-kopplingar
- **Tree Layout Algorithm** - Automatisk horisontell layout baserad på djup
- **OOP Property Editor System** - IPropertyEditor interface med Strategy pattern
- **6 Property Editors** - Room, Hotspot, Dialog, Quest, Item, NPC med full CRUD
- **Breadcrumb Navigation** - Hierarkisk navigation i PropertiesPanel (Rooms > Room > Hotspot)
- **Dropdown för ID-referenser** - NPCs, Items, Rooms, Dialogs använder dropdowns istället för text input
- **PropertyEditorUtils::IdCombo** - Återanvändbar helper för ID-dropdowns
- **Validation & Dirty Tracking** - Varje editor validerar data och spårar ändringar
- **Save/Revert per Editor** - Toolbar med Save/Revert buttons i PropertiesPanel
- **Deselect Functionality** - Klicka på samma item för att deselect
- **Modulär arkitektur** - Separerade `src/` i `engine/`, `game/`, `editor/`
- **RetroCore static library** - Delad engine-kod mellan game och editor
- **Separata executables** - RetroGame.exe och RetroEditor.exe
- **Drag-and-drop editing** i ViewportPanel för hotspots, player spawn och walk area
- **Automatisk backup** (.bak-fil) vid sparning
- **Säkerhetsvalidering** förhindrar sparning av tom data
- ImGui-baserad editor med docking support
- 5 editor-paneler: Hierarchy, Properties, Viewport, AssetBrowser, Console
- Komplett dokumentationsstruktur med CHANGELOG, ADR, DEVLOG
- Git-policy rules och synkade workflows

### Changed
- **Scene consolidation** - Two Scene classes consolidated into single engine::Scene
- **RoomToSceneConverter removal** - Replaced with Scene::createFromData factory method
- **Background actor rendering** - Background actors now use SpriteComponent for rendering
- **Room system → Scene system** - All systems now use SceneManager instead of RoomManager
- **GameDataLoader** - Now loads scenes only to SceneManager (removed double loading)
- **PlayState** - Updated to use SceneManager for scene transitions
- **All systems** - AISystem, SaveSystem, ConditionSystem, CutsceneSystem, WorldQuery, DebugEditor updated for SceneManager
- **Legacy entities/NPC.h** - Removed and replaced with actors/NPC.h
- **Camera2D** - Migrerad från Node2D till ActorObject
- **PhysicsBody** - Migrerad från Node2D till ActorObject
- **Layer** - Migrerad från Node2D till ActorObject
- **KinematicBody** - translate() ersatt med setPosition()
- **SceneGraphPanel** - renderNodeTree → renderActorTree
- **CoreRedirects.h** - Rensat från Node typedefs
- **Engine Architecture** - Ny `src/engine/core/`, `nodes/`, `world/` struktur
- **Node Editor Library** - Bytte från imgui-node-editor till imnodes (enklare, ingen "dangling lines")
- **Graph Panel Structure** - Ny `src/editor/graphs/` med dialog/, quest/, npc/ subfolders
- **Default Dock Layout** - Viewport, Dialog Graph, Quest Graph, NPC Behavior som tabs
- **Selection Management** - Endast ett item kan vara selected åt gången
- **HierarchyPanel** - Rensar andra selections när nytt item väljs
- **PropertiesPanel** - Använder Strategy pattern för att välja rätt editor
- **Projektstruktur** - Flyttade 300+ filer till nya mappar (engine/game/editor)
- **Build-system** - Tre CMake targets: RetroCore (lib), RetroGame (exe), RetroEditor (exe)
- Editor öppnas nu i windowed mode (max 1600x900)
- Spelet öppnas i fullscreen
- **Hot reload borttaget** - Använd restart-workflow istället (enklare, stabilare)

### Removed
- **Legacy Scene files** - Removed src/engine/Scene.h and src/engine/Scene.cpp
- **RoomToSceneConverter** - No longer needed, functionality moved to Scene::createFromData
- **entities/NPC.h** - Legacy NPC class removed and replaced with actors/NPC.h
- **Node.h/Node.cpp** - Legacy Node system helt borttaget
- **Node2D.h/Node2D.cpp** - Legacy Node2D system helt borttaget
- **renderSceneNode** - ViewportPanel Node rendering borttagen

### Fixed
- **Background loading in editor** - Fixed missing background textures by using loadBackground()
- **Include paths** - Updated all Scene includes from engine/Scene.h to engine/world/Scene.h
- **NPC Selection** - HierarchyPanel sätter nu selectedNPCId korrekt
- **NPC Property Editor** - Kopplade NPCPropertyEditor till PropertiesPanel
- **Multi-selection bug** - Kunde ha både Room och Dialog selected samtidigt
- **"No dialog selected" bug** - EditorContext.loadFromDataLoader() anropades inte
- MSVC C2597 compiler bug (ImVec2 → float parametrar i ViewportPanel)

---

## [0.3.0] - 2026-01-05

### Added
- **AI System** - NPC-beteenden med Idle, Patrol, Wander, Follow, GoTo
- **Save System** - JSON serialization med multiple save slots
- **Cutscene System** - Sekventiella actions med fade och text
- **UI Widget System** - Button, Label, Panel, ProgressBar
- **ImGui Editor** - Professionell docking-baserad editor
- **Player Spawn System** - Konfigurerbar spawn per rum
- **NPC Hotspot Positioning** - NPCs spawnar vid sina hotspots

### Changed
- **Scene consolidation** - Two Scene classes consolidated into single engine::Scene
- **RoomToSceneConverter removal** - Replaced with Scene::createFromData factory method
- **Background actor rendering** - Background actors now use SpriteComponent for rendering
- EditorState refaktorerad till modulära komponenter (-61% kod)
- Editor använder nu ImGui istället för custom UI

### Fixed
- **Background loading in editor** - Fixed missing background textures by using loadBackground()
- **Include paths** - Updated all Scene includes from engine/Scene.h to engine/world/Scene.h
- Walk area fungerar nu korrekt i spelet
- Hotspots sparas till JSON

---

## [0.2.0] - 2026-01-04

### Added
- **Room System** - Multi-layer rendering, hotspots, exits
- **Dialog System** - Dialogträd med val och villkor
- **Inventory System** - Items, pickup, kombination
- **Quest System** - Objectives med auto-completion
- **Visual Editor** - Drag-and-drop hotspot editing
- **Depth Scaling** - Perspektiv-skalning baserat på Y-position
- **Mute Toggle** - M-tangent för global mute

### Changed
- **Scene consolidation** - Two Scene classes consolidated into single engine::Scene
- **RoomToSceneConverter removal** - Replaced with Scene::createFromData factory method
- **Background actor rendering** - Background actors now use SpriteComponent for rendering
- Tiled workflow nu valfri (Manual/Tiled toggle)

### Fixed
- **Background loading in editor** - Fixed missing background textures by using loadBackground()
- **Include paths** - Updated all Scene includes from engine/Scene.h to engine/world/Scene.h
- Room layers laddas och renderas korrekt
- Font-storlekar justerade för läsbarhet

---

## [0.1.0] - 2026-01-03

### Added
- **Core Foundation** - SDL2 setup med CMake + vcpkg
- **State Management** - StateManager med push/pop/change
- **Graphics** - TextureManager, SpriteSheet, Animation
- **Audio** - AudioManager med musik och SFX
- **Input** - Keyboard och gamepad support
- **Entity System** - Entity hierarki med OOP
- 9 game states: Menu, Play, Pause, Dialog, Inventory, QuestLog, Options, SaveLoad, Editor

---

## Version Format

- **MAJOR** - Inkompatibla ändringar
- **MINOR** - Ny funktionalitet (bakåtkompatibel)
- **PATCH** - Buggfixar (bakåtkompatibel)

## Session References

Detaljerade session-rapporter finns i [docs/dev/sessions/](dev/sessions/)
