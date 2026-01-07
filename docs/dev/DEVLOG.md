# DEVLOG - Retro Adventure

Kronologisk logg av alla ändringar sedan projektets start.

**Format:** `[HASH]` type(scope): beskrivning

---

## 2026-01-07

### Tidig Morgon

- `6c831ec` feat(physics): implement physics editor system with collision boxes
  - **Data:** Added CollisionBoxData struct for static level geometry (ground, walls, platforms, hazards)
  - **Data:** Added collisionBoxes vector to SceneData with JSON serialization
  - **Editor:** Added physics properties UI to NPC and Hotspot property editors
  - **Editor:** Added gateway functionality to hotspots for scene/level/world transitions
  - **Editor:** Added physics visualization in Scene View with color-coded colliders
  - **Editor:** Added Physics toggle checkbox in Viewport toolbar
  - **Components:** Created TriggerComponent for collision callbacks (onEnter/onExit/onStay)
  - **Components:** Created PhysicsLoader utility for runtime physics data loading
  - **Examples:** Added collision boxes to Tavern scene (floor, walls, one-way platform)
  - **Visualization:** Green=ground, Red=wall, Blue=platform, Cyan=trigger, Bright red=hazard
  - **Build:** Added new physics files to CMakeLists.txt
  - Session: [2026-01-07](sessions/2026-01-07.md)

## 2026-01-06

### Sen Kväll

- `df9fc7e` feat(physics): implement Box2D physics system with Adventure/Platformer modes
  - **Physics:** Added PhysicsWorld2D wrapper for Box2D v3.1.1
  - **Physics:** Created RigidBody2DComponent and Collider2DComponent
  - **Physics:** Implemented CharacterController2D for platformer movement
  - **Physics:** Integrated physics in WorldContainer (World/Level/Scene)
  - **Physics:** Added pixel-to-meter conversions (32 pixels = 1 meter)
  - **Editor:** Created GameSettings singleton for game mode configuration
  - **Editor:** Added GameSettingsPanel for Adventure/Platformer mode selection
  - **Editor:** Physics settings panel with gravity, jump force, speeds
  - **Gameplay:** Adventure mode = point-and-click with depth scaling
  - **Gameplay:** Platformer mode = physics-based movement with jump/run
  - **Build:** Added Box2D and GLM dependencies via vcpkg
  - Session: [2026-01-06](sessions/2026-01-06.md)

- `2967cee` feat(physics): add Box2D and GLM libraries with physics roadmap
  - **Dependencies:** Box2D v3.1.1 för 2D physics simulation
  - **Dependencies:** GLM v1.0.2 för math (vektorer, matriser)
  - **Docs:** PHYSICS_ROADMAP.md med OOP-arkitektur plan
  - **Build:** CMakeLists.txt och vcpkg.json uppdaterade
  - Session: [2026-01-06](sessions/2026-01-06.md)

- `37c44b5` feat(editor): implement World/Level/Scene hierarchy with actor support
  - **Hierarchy:** Top-down World → Level → Scene med full persistence
  - **World System:** world.json laddas/sparas med EditorWorldManager
  - **Actor Support:** ActorData på alla nivåer (World, Level, Scene)
  - **Sync System:** Konsekvent synkronisering: engine → editor → JSON
  - **Interaction:** 1-klick=select/drag, 2-klick=enter på alla nivåer
  - **Grid Persistence:** Level grid positions sparas till world.json
  - **Load Flow:** world.json → WorldData → engine::World med actors
  - **Save Flow:** syncLevelsToWorldData() → world.json → scenes.json
  - **Files:** EditorWorldManager, EditorDataTypes, ViewportPanel_World
  - Session: [2026-01-06](sessions/2026-01-06.md)

### Kväll

- `7df904f` refactor(editor): complete folder restructuring and fix rendering issues
  - **Arkitektur:** Flyttade 80+ filer från flat src/editor/ till hierarkisk struktur
  - **Structure:** Skapade core/, managers/, panels/*/, properties/*/, legacy/, ui/, input/, data/, commands/, tools/, widgets/
  - **Build:** Uppdaterade CMakeLists.txt med alla nya sökvägar
  - **Fixes:** Fixade ImGui dockspace att använda faktisk fönsterstorlek istället för hårdkodade värden
  - **Fixes:** Lade till SDL_RenderClear() för att förhindra fullscreen artifacts
  - **Fixes:** Fixade alla include paths efter omstruktureringen
  - **Resultat:** Editorn är nu fullt fungerande med organiserad kodbas
  - **Statistik:** 171 filer ändrade, 488 insertions, 1665 deletions
  - Session: [2026-01-06](sessions/2026-01-06.md)

- `fd76906` refactor(editor): separate EditorState into manager classes
  - **Arkitektur:** EditorState.cpp reducerad från 998 till 211 rader (-79%)
  - **Separation of Concerns:** Extraherade funktionalitet till manager-klasser
  - **EditorPanelManager:** Skapar och hanterar alla editor panels (~150 rader)
  - **EditorWorldManager:** World/Level/Scene setup från RoomData (~120 rader)
  - **EditorEventDispatcher:** SDL event handling och shortcuts (~80 rader)
  - **ViewportPanel:** Uppdelad i 6 filer (Core, World, Level, Scene, Actors, Input)
  - **PlaceActorsPanel:** Ny panel för drag-and-drop av actor templates (UE5-style)
  - **EditorInputHandler:** Centraliserad input med Windows shortcuts
  - **Shortcuts:** Del (delete actor), Ctrl+D (duplicate), Ctrl+Z/Y (undo/redo), Ctrl+S (save)
  - **WorldContainer:** Lade till removeActor() metod
  - **Build:** CMakeLists.txt uppdaterad med 9 nya filer
  - **Statistik:** 92 filer ändrade, 4478 insertions, 1557 deletions
  - Session: [2026-01-06](sessions/2026-01-06.md)

## 2026-01-05

### Kväll (Sen)

- `e28d059` feat(editor): add FileBrowser and enhance HierarchyPanel for Actor system