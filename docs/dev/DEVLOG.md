# DEVLOG - Retro Adventure

Kronologisk logg av alla ändringar sedan projektets start.

**Format:** `[HASH]` type(scope): beskrivning

---

## 2026-01-10

### Kväll (16:20-17:11) - 2D till 3D Migration Fas 6-7

- `91eb4b8` fix(runtime): load scenes.json directly when world.json missing
  - **Ändrade filer:** RuntimeWorld.cpp
  - **Fix:** Fallback till scenes.json när world.json saknas
  - **Verifierat:** NPCs laddas korrekt i runtime

- `fef007c` feat(editor): add NPC3DActor support to EditorPlayMode
  - **Ändrade filer:** EditorPlayMode.cpp
  - **Feature:** NPC3DActor uppdateras i play mode
  - **Verifierat:** NPCs rör sig och loggar positioner

- `cae79c8` feat(editor): implement 2D projection mode in EditorCamera3D
  - **Ändrade filer:** EditorCamera3D.h/cpp
  - **Feature:** Ortografisk projektion för 2D-läge
  - **API:** setProjectionMode(), setOrthoSize()

- `ad55d94` feat(editor,physics): complete 2D projection mode implementation
  - **Ändrade filer:** ViewportToolbar.cpp/h, ViewportPanel.cpp
  - **Feature:** 2D/3D toggle-knappar i toolbar
  - **Verifierat:** Knappar byter kameraprojektion

- `b208f60` feat(actors,components): complete Fas 4-5 - mark legacy 2D as DEPRECATED
  - **Ändrade filer:** CMakeLists.txt, Game.cpp/h
  - **Breaking:** StateManager borttagen från Game.cpp
  - **Förberedelse:** Legacy 2D-kod markerad för borttagning

- `fe99bfd` refactor(physics)!: remove Box2D and 2D physics - unified 3D architecture
  - **BREAKING CHANGE:** Box2D och alla 2D physics-komponenter borttagna
  - **Borttagna filer:**
    - `src/engine/physics/box2d/` (5 filer)
    - `RigidBody2DComponent.h/.cpp`
    - `Collider2DComponent.h/.cpp`
    - `CharacterController2D.h/.cpp`
  - **Ändrade filer:** PhysicsManager.h, EditorPlayMode.h/cpp, ViewportActorRenderer.cpp, WorldContainer.h
  - **Statistik:** 271 filer, +546/-2449 rader
  - **Arkitektur:** Motorn använder nu endast PhysX för all physics
  - **2D-stöd:** Implementerat via ortografisk projektion i 3D-rymden

### Session Sammanfattning
- **Commits:** 6 st
- **Huvudsakligt arbete:** Slutförde 2D till 3D migration (Fas 6-7)
- **Resultat:** Box2D helt borttagen, endast PhysX används nu
- **Status:** Alla builds fungerar (RetroCore, RetroEditor, RetroGame, RetroLauncher)

---

## 2026-01-09

### Natt (00:50-03:05)

- `74f2c01` feat(editor,actors): add ActorDetailsPanel and PlayerConfigActor
  - **ActorDetailsPanel:** Detaljerad actor editor i separat panel
    - Transform editor (position, rotation, yaw, scale)
    - Component editors (CharacterController3D, Camera, Sprite, RigidBody3D)
    - Add Component dropdown
    - Auto-open vid actor selection
  - **PlayerConfigActor:** Global player-konfiguration med kamera
    - CameraComponent automatiskt tillagd
    - Camera offset settings (X, Y, Z)
    - Camera follow speed
    - Default-skapad i Tavern scene
  - **Camera Visualization:** Cyan kub för PlayerConfig, gul kub för camera preview
  - **Nya filer:** ActorDetailsPanel.h/cpp, PlayerConfigActor.h/cpp
  - **Statistik:** 55 filer, +1,768/-526 rader
  - **Verifierat:** Actor Details öppnas vid selection, camera settings redigerbara

- `efb2585` chore(build): add build artifacts and temp files
  - **Build artifacts:** .obj, .exe, .lib filer
  - **Statistik:** 91 filer, +422/-1,813 rader

- `50320de` feat(engine,editor): add 3D character system with PhysX controller
  - **3D Character System:** Komplett implementation
    - PlayerStartActor: Spawn-punkt för 3D-spelare
    - Character3DActor: Bas-klass för 3D-karaktärer
    - Player3DActor: Spelbar karaktär med WASD + mouse look
    - CharacterController3DComponent: PhysX character controller wrapper
  - **Features:**
    - WASD-rörelse rotation-relativ (W = framåt i spelarens riktning)
    - RMB + mus för player rotation (yaw)
    - SPACE för hopp
    - Grounded-detektion
    - Kamera följer player automatiskt (smooth lerp)
    - Player renderas som grön kub med rotation
  - **PhysX Integration:**
    - PxControllerManager för character controllers
    - PxCapsuleController för player collision
    - eSCENE_QUERY_SHAPE flag för ground detection
    - Ground plane vid Y=0
  - **Editor Integration:**
    - EditorPlayMode spawnar player vid PlayerStart
    - Viewport3DPanel renderar player med rotation
    - Default PlayerStart i Tavern scene
    - Camera follow i Viewport3DPanel::update()
    - m_playMode vidarebefordras: ViewportPanel → Base3DRenderer → Viewport3DPanel
  - **Nya filer:** 6 st (Character3DActor, Player3DActor, PlayerStartActor, CharacterController3DComponent)
  - **Statistik:** 18 filer, +1,343/-86 rader
  - **Verifierat:** WASD fungerar, mouse look fungerar, kamera följer, hopp fungerar, grounded detection fungerar
- Session: [2026-01-09](sessions/2026-01-09.md)

---

## 2026-01-08

### Kväll (22:50-23:28)

- `7e44dee` refactor(viewport): redesign with OOP inheritance architecture
  - **Arkitektur:** Komplett OOP inheritance hierarki
    - BaseViewRenderer → Base3DRenderer/Base2DRenderer
    - 3D renderers: World3D, Level3D, Scene3D
    - 2D renderers: World2D → Level2D → Scene2D (inheritance chain)
  - **Nya filer:** 20 st i organiserad mappstruktur (core/, renderers/3d/, renderers/2d/, ui/, input/, 3d/)
  - **Borttagna:** 18 gamla viewport filer (7,703 rader)
  - **Statistik:** 219 filer, +4,517/-6,286 rader, netto -1,769 (-27%)
  - **Features:** Play/Pause/Stop buttons, EditorPlayMode integration, fysik simulation
  - **Verifierat:** 3D/2D rendering, navigation, selection, play mode fungerar
- `443cb1c` docs: update CHANGELOG and ROADMAP for viewport refactoring
  - **CHANGELOG:** Dokumenterade viewport OOP architecture
  - **ROADMAP:** Uppdaterade Fas 4 till 20%
  - **Ny fil:** docs/architecture/viewport-oop-inheritance.md
- Session: [2026-01-08](sessions/2026-01-08.md)

### Eftermiddag

- `a8b52dd` docs: add PR description for 3D viewport feature
- `6fa30bd` chore: configure Git LFS for large binary files
  - **Git LFS:** Konfigurerat för *.pdb, *.dll, *.exe, *.lib
  - **Löser:** GitHub 100MB filstorleksbegränsning
  - **.gitattributes:** Ny fil med LFS tracking
- `0b00c23` docs(workflow): add critical rule - always commit build files
  - **Workflow:** Uppdaterad git-commit.md med build-fil regel
  - **Memory:** Skapad för att alltid commita build-filer
- `7c89611` feat(editor): add GLTextureManager and input refactoring plan
  - **input_checklist.md:** Detaljerad plan för input-centralisering
  - **EditorInputController:** Planerad arkitektur dokumenterad

---

## 2026-01-07

### Kväll

- `4bf200d` feat(ai): add AI World Builder tools for levels and quests
  - **AI Tools:** 37 total registered tools for comprehensive world building
  - **Level/World:** list_levels, create_level, add_scene_to_level, set_start_scene, get_world_info
  - **Quest:** list_quests, get_quest, create_quest, add_quest_objective, link_quest_to_npc
  - **Dialog:** list_dialogs, get_dialog, create_dialog, add_dialog_node
  - **Hotspot:** list_hotspots, create_hotspot, modify_hotspot, delete_hotspot
  - **Item:** list_items, get_item, create_item, modify_item
  - **Context:** get_editor_context, select_scene, select_actor
  - **Command:** execute_command, list_commands
  - **System Prompt:** Updated with world-building guidelines for consistent storytelling
  - **DataLoader:** Added saveNPCs() and saveQuests() methods
  - **Editor:** Added CommandPanel for command palette

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