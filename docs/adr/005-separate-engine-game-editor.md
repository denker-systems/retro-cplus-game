# ADR 005: Separera Engine, Game och Editor

**Datum:** 2026-01-05  
**Status:** Accepterad  
**Beslutsfattare:** Utvecklingsteam

## Kontext

Projektet hade tidigare en flat struktur där all kod låg i `src/`. Detta gjorde det svårt att:
- Återanvända engine-kod mellan game och editor
- Bygga separata executables
- Hålla isär ansvar och dependencies
- Skala projektet när det växer

## Beslut

**Dela upp projektet i tre separata moduler:**

```
src/
├── engine/          # Delad engine-kod (static lib)
│   ├── audio/
│   ├── data/
│   ├── entities/
│   ├── graphics/
│   ├── input/
│   ├── systems/
│   ├── ui/
│   └── utils/
├── game/            # Spel-specifik kod (exe)
│   ├── states/
│   ├── Game.cpp/h
│   └── main.cpp
└── editor/          # Editor-specifik kod (exe)
    ├── panels/
    ├── components/
    ├── commands/
    ├── widgets/
    ├── EditorState.cpp/h
    └── main.cpp
```

## Build-struktur

### RetroCore (Static Library)
- **Innehåll:** All engine-kod
- **Dependencies:** SDL2, SDL2_image, SDL2_mixer, SDL2_ttf, nlohmann_json
- **Används av:** RetroGame och RetroEditor

### RetroGame (Executable)
- **Innehåll:** Game states, main loop
- **Dependencies:** RetroCore
- **Conditional:** `HAS_EDITOR=0` (ingen editor-access)

### RetroEditor (Executable)
- **Innehåll:** ImGui panels, editor logic
- **Dependencies:** RetroCore, ImGui
- **Conditional:** `HAS_IMGUI=1`

## Konsekvenser

### Positiva
- ✅ **Tydlig separation** - Engine vs Game vs Editor
- ✅ **Återanvändbar kod** - Engine delas mellan båda
- ✅ **Oberoende builds** - Kan bygga game utan editor
- ✅ **Skalbarhet** - Lättare att lägga till nya features
- ✅ **Mindre executables** - Game behöver inte ImGui

### Negativa
- ❌ **Mer komplex build** - Tre targets istället för en
- ❌ **Längre build-tid** - Static lib måste byggas först

### Mitigering
- CMake hanterar dependencies automatiskt
- Incremental builds är snabba efter första bygget

## Implementation

### Flyttade filer

**Till `engine/`:**
- All kod från `src/` som är delad (rendering, audio, systems, etc.)
- `Input.cpp/h` → `engine/input/`
- Alla systems → `engine/systems/`
- Alla entities → `engine/entities/`

**Till `game/`:**
- `Game.cpp/h` (main game loop)
- `states/` (MenuState, PlayState, etc.)
- `main.cpp` (game entry point)

**Till `editor/`:**
- `EditorState.cpp/h` (flyttad från `src/states/`)
- `panels/` (ImGui panels)
- `components/` (VisualRoomEditor, RoomDataManager, etc.)
- Editor-specifik main.cpp

### CMakeLists.txt

```cmake
# RetroCore - Static library
add_library(RetroCore STATIC ${CORE_SOURCES})
target_link_libraries(RetroCore PUBLIC SDL2 SDL2_image ...)

# RetroGame - Game executable
add_executable(RetroGame ${GAME_SOURCES})
target_link_libraries(RetroGame PRIVATE RetroCore)

# RetroEditor - Editor executable
add_executable(RetroEditor ${EDITOR_SOURCES})
target_link_libraries(RetroEditor PRIVATE RetroCore imgui::imgui)
target_compile_definitions(RetroEditor PRIVATE HAS_IMGUI=1)
```

## Alternativ som övervägdes

1. **Monolitisk struktur** - Behålla allt i `src/`
   - ❌ Svårt att skala
   - ❌ Editor-dependencies i game

2. **DLL-baserad engine** - Shared library istället för static
   - ❌ Mer komplext deployment
   - ❌ Performance overhead

3. **Header-only engine** - Ingen lib alls
   - ❌ Långsam compile-tid
   - ❌ Svårt att underhålla

## Relaterade beslut

- [ADR 004: Separate Game and Editor Builds](004-separate-builds.md)
- [ADR 003: ImGui Editor Architecture](003-imgui-editor.md)

## Verifiering

```powershell
# Bygg alla targets
cmake --build . --config Release

# Resultat:
# - build/Release/RetroCore.lib
# - build/Release/RetroGame.exe
# - build/Release/RetroEditor.exe
```

Alla tre targets bygger korrekt och game/editor delar samma engine-kod.
