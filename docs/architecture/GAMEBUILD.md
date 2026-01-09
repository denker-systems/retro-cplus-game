# RetroGame Build Architecture

> Ren 3D Runtime Player - Laddar och kör editor-skapad content

**Datum:** 2026-01-09  
**Status:** Design & Implementation Pågår

---

## 🎯 Vision

RetroGame.exe är en **runtime player** som laddar och kör det editorn skapar. Ingen legacy 2D-kod, inga states, bara ren 3D från grunden.

```
┌─────────────────────────────────────────────────────────────┐
│                    RETRO ENGINE ECOSYSTEM                    │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────────┐         ┌──────────────────┐         │
│  │  RetroEditor     │ Skapar  │  Game Data       │         │
│  │  (Authoring)     │────────▶│  (JSON/Assets)   │         │
│  └──────────────────┘         └──────────────────┘         │
│         │                              │                    │
│         │ Play Mode                    │ Loads              │
│         ▼                              ▼                    │
│  ┌──────────────────┐         ┌──────────────────┐         │
│  │ EditorPlayMode   │         │  RetroGame       │         │
│  │ (Test in Editor) │         │  (Standalone)    │         │
│  └──────────────────┘         └──────────────────┘         │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 📦 Vad RetroGame Behöver

### Data (från Editor)
```
assets/data/
├── scenes.json         # World/Level/Scene hierarki
├── actors.json         # Actor templates och placements
├── physics.json        # Physics settings
└── config.json         # Game config (start scene, etc.)
```

### Engine Systems (från RetroCore.lib)
- ✅ PhysicsManager (PhysX 3D)
- ✅ World/Level/Scene hierarki
- ✅ Actor system (Player3DActor, StaticMeshActor, etc.)
- ✅ Component system
- ✅ Input handling
- ✅ Rendering (OpenGL)

### INTE Behövs
- ❌ ImGui (editor UI)
- ❌ EditorState
- ❌ Editor panels
- ❌ Undo/Redo
- ❌ Asset browser
- ❌ Gizmos
- ❌ StateManager (legacy)
- ❌ 2D states (MenuState, PlayState, etc.)

---

## 🏗️ Arkitektur

### Folder Structure

```
src/
├── engine/          # RetroCore.lib (shared engine)
│   ├── core/
│   ├── actors/
│   ├── components/
│   ├── physics/
│   └── world/
│
├── editor/          # RetroEditor.exe (authoring tool)
│   ├── core/
│   ├── panels/
│   └── viewport/
│
├── runtime/         # RetroGame.exe (NEW - runtime player)
│   ├── main.cpp
│   ├── RuntimeApp.h/cpp
│   ├── RuntimeWorld.h/cpp
│   └── RuntimeConfig.h/cpp
│
└── game/            # DEPRECATED (legacy 2D states)
    └── states_legacy/
```

### RuntimeApp Class

```cpp
class RuntimeApp {
public:
    bool init();
    void run();
    void shutdown();
    
private:
    // Core systems
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;
    PhysicsManager m_physics;
    
    // Game world
    std::unique_ptr<World> m_world;
    Scene* m_activeScene = nullptr;
    
    // Player
    std::unique_ptr<Player3DActor> m_player;
    std::unique_ptr<EditorCamera3D> m_camera;
    
    // Input
    Input m_input;
    
    // Game loop
    void handleInput();
    void update(float dt);
    void render();
    
    // Initialization
    bool initSDL();
    bool initOpenGL();
    bool initPhysics();
    bool loadWorld();
    void spawnPlayer();
};
```

---

## 📋 Implementation Plan

### Fas 1: Minimal Runtime ⏱️ 1-2 timmar

**Mål:** Få RetroGame.exe att starta och visa ett tomt 3D-fönster.

**Steg:**
1. Skapa `src/runtime/` folder
2. Skapa `RuntimeApp.h/cpp` (minimal SDL + OpenGL init)
3. Skapa `main.cpp` (entry point)
4. Uppdatera CMakeLists.txt (ny RUNTIME_SOURCES)
5. Bygg och verifiera att fönster öppnas

**Kod:**
```cpp
// src/runtime/main.cpp
int main(int argc, char* argv[]) {
    RuntimeApp app;
    
    if (!app.init()) {
        return -1;
    }
    
    app.run();  // Game loop
    app.shutdown();
    
    return 0;
}
```

---

### Fas 2: Ladda Editor Data ⏱️ 2-3 timmar

**Mål:** Ladda scene från `assets/data/scenes.json` som editorn skapade.

**Steg:**
1. Skapa `RuntimeWorld.h/cpp`
2. Implementera JSON-laddning (återanvänd DataLoader)
3. Skapa actors från JSON-data
4. Initiera physics för alla actors

**Kod:**
```cpp
// RuntimeWorld.cpp
bool RuntimeWorld::loadFromJSON(const std::string& path) {
    // Ladda scenes.json
    nlohmann::json j = loadJSON(path);
    
    // Skapa World → Level → Scene hierarki
    m_world = std::make_unique<World>();
    
    for (auto& levelData : j["levels"]) {
        auto level = std::make_unique<Level>();
        
        for (auto& sceneData : levelData["scenes"]) {
            auto scene = createSceneFromJSON(sceneData);
            level->addScene(std::move(scene));
        }
        
        m_world->addLevel(std::move(level));
    }
    
    return true;
}
```

---

### Fas 3: Player & Input ⏱️ 1-2 timmar

**Mål:** Spawna player och få WASD + mouse look att fungera.

**Steg:**
1. Implementera player spawning
2. Koppla input till player movement
3. Koppla kamera till player
4. Testa WASD + mouse look

**Kod:**
```cpp
// RuntimeApp::spawnPlayer()
void RuntimeApp::spawnPlayer() {
    // Hitta PlayerStart i scene
    auto* playerStart = m_activeScene->findActor<PlayerStartActor>();
    if (!playerStart) {
        LOG_ERROR("No PlayerStart found in scene!");
        return;
    }
    
    // Spawna Player3DActor
    m_player = std::make_unique<Player3DActor>();
    m_player->setPosition(playerStart->getPosition3D());
    m_player->initializeController(&m_physics);
    
    // Setup camera
    m_camera = std::make_unique<EditorCamera3D>();
    m_camera->setTarget(m_player.get());
    m_camera->setFollowSpeed(5.0f);
    m_camera->setOffset(glm::vec3(0, 3, 5));
}
```

---

### Fas 4: Rendering ⏱️ 2-3 timmar

**Mål:** Rendera 3D-världen med samma rendering som editorn.

**Steg:**
1. Setup OpenGL context (samma som editor)
2. Implementera basic 3D rendering
3. Rendera alla StaticMeshActors
4. Rendera player (grön kub)

**Kod:**
```cpp
// RuntimeApp::render()
void RuntimeApp::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Update camera matrices
    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 proj = m_camera->getProjectionMatrix();
    
    // Render scene
    for (auto& actor : m_activeScene->getActors()) {
        if (auto* mesh = dynamic_cast<StaticMeshActor*>(actor.get())) {
            mesh->render(view, proj);
        }
    }
    
    // Render player
    if (m_player) {
        m_player->render(view, proj);
    }
    
    SDL_GL_SwapWindow(m_window);
}
```

---

### Fas 5: Physics Simulation ⏱️ 1 timme

**Mål:** Få PhysX att köra och uppdatera actors.

**Steg:**
1. Initiera PhysicsManager
2. Uppdatera physics i game loop
3. Synka actors från physics
4. Verifiera att gravity och collisions fungerar

**Kod:**
```cpp
// RuntimeApp::update()
void RuntimeApp::update(float dt) {
    // Update physics
    m_physics.step(dt);
    
    // Update player
    m_player->update(dt);
    
    // Update camera
    m_camera->update(dt);
    
    // Sync actors from physics
    for (auto& actor : m_activeScene->getActors()) {
        if (auto* mesh = dynamic_cast<StaticMeshActor*>(actor.get())) {
            if (mesh->isPhysicsEnabled()) {
                mesh->syncFromPhysics();
            }
        }
    }
}
```

---

## 🎯 Slutresultat

### RetroGame.exe kommer att:

1. ✅ Starta direkt i 3D-världen (ingen meny)
2. ✅ Ladda scene från `assets/data/scenes.json`
3. ✅ Spawna player vid PlayerStart
4. ✅ Köra PhysX physics simulation
5. ✅ WASD + mouse look för player control
6. ✅ Rendera 3D-världen med OpenGL
7. ✅ ESC för att avsluta

### Skillnad mot Editor

| Feature | RetroEditor | RetroGame |
|---------|-------------|-----------|
| **UI** | ImGui panels | Ingen UI (bara 3D) |
| **Mode** | Edit + Play | Bara Play |
| **Input** | Editor tools + Play | Bara Play |
| **Data** | Kan redigera | Bara läsa |
| **Startup** | Launcher → Editor | Direkt till spel |
| **Window** | 1600x900 | Fullscreen/Windowed |
| **Dependencies** | ImGui, ImGuizmo, etc. | Bara SDL + OpenGL |

---

## 🔧 CMakeLists.txt

```cmake
# ============================================================================
# RETRO GAME - Runtime Player (NEW)
# ============================================================================
set(RUNTIME_SOURCES
    src/runtime/main.cpp
    src/runtime/RuntimeApp.cpp
    src/runtime/RuntimeWorld.cpp
    src/runtime/RuntimeConfig.cpp
)

# Windows icon resources
if(WIN32)
    list(APPEND RUNTIME_SOURCES src/game/RetroGame.rc)
endif()

add_executable(RetroGame ${RUNTIME_SOURCES})
target_link_libraries(RetroGame PRIVATE
    RetroCore
    SDL2::SDL2
    SDL2::SDL2main
    OpenGL::GL
    GLEW::GLEW
    unofficial::omniverse-physx-sdk::sdk
)

# Copy assets
add_custom_command(TARGET RetroGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:RetroGame>/assets
)
```

---

## 📊 Tidsestimat

| Fas | Uppgift | Tid |
|-----|---------|-----|
| 1 | Minimal Runtime | 1-2h |
| 2 | Ladda Editor Data | 2-3h |
| 3 | Player & Input | 1-2h |
| 4 | Rendering | 2-3h |
| 5 | Physics | 1h |
| **TOTALT** | | **7-11h** |

---

## 🗑️ Legacy Code Cleanup

### /game Folder - DEPRECATED

**Markera för borttagning:**
```
src/game/
├── states/              # DEPRECATED - 2D states
│   ├── MenuState        # Ersätts av launcher
│   ├── PlayState        # Ersätts av RuntimeApp
│   ├── DialogState      # Behövs ej i 3D
│   ├── InventoryState   # Behövs ej i 3D
│   ├── PauseState       # Behövs ej i 3D
│   └── ...
└── Game.cpp/h           # Behålls för nu (används av editor)
```

**Åtgärd:**
1. Flytta `src/game/states/` → `src/game/states_legacy/`
2. Ta bort från CMakeLists.txt GAME_SOURCES
3. Dokumentera i ADR
4. Ta bort helt i framtida cleanup-fas

---

## 📝 Nästa Steg

1. **Skapa memory** av denna plan ✅
2. **Skapa GAMEBUILD.md** ✅
3. **Implementera Fas 1** - Minimal Runtime
4. **Testa och verifiera** varje fas
5. **Dokumentera** i DEVLOG

---

## 🔗 Relaterade Dokument

- `docs/architecture/ARCHITECTURE.md` - Övergripande arkitektur
- `docs/architecture/3d-hierarchy-design.md` - 3D-system design
- `docs/dev/DEVLOG.md` - Utvecklingslogg
- `docs/CHANGELOG.md` - Release notes
