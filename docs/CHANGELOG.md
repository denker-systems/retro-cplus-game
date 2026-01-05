# Changelog

Alla viktiga ändringar i detta projekt dokumenteras här.

Formatet följer [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
och projektet använder [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### Added
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

### Fixed
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
- EditorState refaktorerad till modulära komponenter (-61% kod)
- Editor använder nu ImGui istället för custom UI

### Fixed
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
- Tiled workflow nu valfri (Manual/Tiled toggle)

### Fixed
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
