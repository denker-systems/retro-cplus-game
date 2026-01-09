# ADR-007: Runtime Architecture - Ren 3D Player

**Datum:** 2026-01-09  
**Status:** ✅ Accepterad  
**Beslutsfattare:** Calle + Cascade

---

## Kontext

RetroGame.exe körde det gamla 2D point-and-click spelet medan RetroEditor hade ett helt nytt 3D-system med PhysX, character controller, och kamera. När man tryckte Play i editorn fungerade 3D-simulationen perfekt, men RetroGame.exe startade det gamla spelet.

### Problem
- `/game/states/` innehöll 2D-baserade states (MenuState, PlayState, DialogState, etc.)
- StateManager var designad för 2D point-and-click gameplay
- RetroGame.exe använde gamla GAME_SOURCES i CMakeLists.txt
- Ingen kod-återanvändning mellan EditorPlayMode (3D) och RetroGame (2D)
- Duplicerad logik för physics, input, och rendering

---

## Beslut

Skapa en helt ny **Runtime Architecture** i `src/runtime/` som är en ren 3D player från grunden.

### Ny Arkitektur

```
Editor → Game Data (JSON) → Runtime Player
```

**RetroEditor:** Authoring tool (skapar content)  
**RetroGame:** Runtime player (kör content)

### Folder Structure

```
src/
├── engine/          # RetroCore.lib (shared engine)
├── editor/          # RetroEditor.exe (authoring)
├── runtime/         # RetroGame.exe (NEW - runtime player)
│   ├── main.cpp
│   ├── RuntimeApp.h/cpp
│   ├── RuntimeWorld.h/cpp
│   └── RuntimeRenderer.h/cpp
└── game/            # DEPRECATED (legacy 2D states)
```

### RuntimeApp Responsibilities
- SDL + OpenGL initialization
- PhysicsManager (PhysX 3D)
- World/Scene loading från JSON
- Player spawning vid PlayerStart
- Input handling (WASD + mouse look)
- 3D rendering
- Game loop

---

## Implementation

### Fas 1: Minimal Runtime
- Skapade `RuntimeApp.h/cpp` med SDL + OpenGL + PhysX init
- Entry point `main.cpp`
- Tomt 3D-fönster öppnas

### Fas 2: World Loading
- Skapade `RuntimeWorld.h/cpp` för JSON-laddning
- Laddar från `assets/data/scenes.json` (med fallback till default scene)
- Skapar ground plane för physics (100x1x100 units vid Y=0)

### Fas 3: Input
- WASD movement (rotation-relativ)
- RMB + mouse för look around
- SPACE för jump
- ESC för quit

### Fas 4: Rendering
- Skapade `RuntimeRenderer.h/cpp`
- Embedded GLSL shaders (vertex + fragment)
- Ground grid rendering (20x20m checkerboard)
- Player rendering (grön kub 0.5x1x0.5m)
- Phong lighting (ambient + diffuse + specular)

### Fas 5: Camera Follow
- Kamera följer player smooth med lerp
- Rotation-baserad offset (följer player yaw)
- setTarget() för att alltid titta på player

---

## CMakeLists.txt Ändringar

### RetroGame (Runtime)
```cmake
set(RUNTIME_SOURCES
    src/runtime/main.cpp
    src/runtime/RuntimeApp.cpp
    src/runtime/RuntimeWorld.cpp
    src/runtime/RuntimeRenderer.cpp
    src/editor/viewport/3d/EditorCamera3D.cpp
)

add_executable(RetroGame ${RUNTIME_SOURCES})
target_link_libraries(RetroGame PRIVATE
    RetroCore
    SDL2::SDL2
    OpenGL::GL
    GLEW::GLEW
    unofficial::omniverse-physx-sdk::sdk
)
```

### CMakeGenerator (Build Panel)
Uppdaterad att generera runtime-baserad build:
- `RUNTIME_SOURCES` istället för `GAME_SOURCES`
- Länkar OpenGL + GLEW + PhysX
- Genererar `RuntimeApp`-baserad main.cpp

---

## Konsekvenser

### Positiva ✅
- **Kod-återanvändning:** Samma 3D-system i editor och runtime
- **Enklare:** Ingen StateManager, inga overlay states
- **Modernare:** Ren 3D från grunden, inte 2D-legacy
- **Mindre kod:** ~500 rader runtime vs ~2000 rader legacy states
- **Bättre performance:** Direkt 3D rendering, ingen overhead
- **Editor-driven:** Runtime laddar bara det editorn skapar

### Negativa ⚠️
- **Ingen meny:** RetroGame startar direkt i 3D-världen (kan läggas till senare)
- **Ingen pause UI:** Ingen overlay UI än (kan läggas till senare)
- **Legacy states kvar:** `/game/states/` behövs fortfarande av editor för nu

### Framtida Cleanup
- [ ] Refactor `Game.cpp` att inte behöva StateManager
- [ ] Ta bort `/game/states/` helt
- [ ] Flytta `Game.cpp` till `/editor/` (används bara av editor)

---

## Verifiering

### RetroGame.exe Fungerar ✅
```
✅ SDL + OpenGL 3.3 initiering
✅ PhysX 3D physics (CPU mode)
✅ Game loop 50 FPS stabilt
✅ 3D rendering (ground grid + player kub)
✅ WASD movement (rotation-relativ)
✅ RMB + mouse look
✅ SPACE jump
✅ Ground collision (player landar vid Y=1)
✅ Camera follow (smooth lerp med rotation offset)
```

### Build Panel Fungerar ✅
```
✅ Genererar runtime-baserad CMakeLists.txt
✅ Genererar RuntimeApp main.cpp
✅ Bygger 3D runtime player
✅ Kopierar assets och DLLs
```

---

## Referenser

- `docs/architecture/GAMEBUILD.md` - Komplett implementation plan
- `src/runtime/` - Nya runtime-filer
- Memory: "RetroGame Runtime Architecture"

---

## Alternativ Övervägda

### Alternativ 1: Behåll StateManager, Ersätt States
**Avvisat:** För mycket duplicerad kod från EditorPlayMode

### Alternativ 2: RetroGame = RetroLauncher + Direct Scene Load
**Avvisat:** Launcher är overkill för standalone game

### Alternativ 3: Runtime Player (VALD)
**Motivering:** Minimal kod, maximal återanvändning, editor-driven workflow

---

## Nästa Steg

1. Ladda editor-skapad data från `assets/data/scenes.json`
2. Rendera StaticMeshActors från editorn
3. Lägg till UI system (HUD, health bar)
4. Lägg till audio (bakgrundsmusik, ljudeffekter)
5. Refactor Game.cpp att inte behöva legacy states
