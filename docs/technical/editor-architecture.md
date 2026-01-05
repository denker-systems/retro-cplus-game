# Editor Architecture

## Översikt

Projektet har nu tre separata build targets:

```
RetroCore.lib      # Delad engine-kod (static library)
RetroGame.exe      # Spel med in-game editor (F1)
RetroEditor.exe    # Standalone editor (startar direkt i EditorState)
RetroAdventure.exe # Legacy (bakåtkompatibilitet)
```

## Dependency-struktur

### Core (alltid inkluderat)
- SDL2, SDL2_image, SDL2_mixer, SDL2_ttf
- nlohmann-json

### Editor (valfritt via vcpkg features)
- **Dear ImGui (docking)** - UI framework
- **ImGuizmo** - Transform gizmos
- **imnodes** - Node editor för dialog/quests
- **ImPlot** - Debug/profiling grafer
- **nativefiledialog-extended** - OS-native fil-dialoger

### Dev (valfritt)
- **spdlog** - Logging
- **efsw** - File watching för hot reload

## vcpkg Manifest Mode

Projektet använder `vcpkg.json` för dependency-hantering:

```bash
# Installera endast core
vcpkg install

# Installera med editor-features
vcpkg install --x-feature=editor

# Installera med dev-tools
vcpkg install --x-feature=dev
```

## CMake Targets

### RetroCore (STATIC library)
Innehåller all delad kod:
- Systems (Dialog, Inventory, Quest, AI, Save, etc.)
- Graphics (TextureManager, FontManager, Animation)
- Audio (AudioManager)
- Entities (Entity, Character, Player, NPC)
- Data (DataLoader, TiledImporter)

### RetroGame
- Länkar RetroCore
- Innehåller Game.cpp, states/, editor/
- Startar med MenuState

### RetroEditor
- Länkar RetroCore + editor-libs
- Startar direkt i EditorState
- Har tillgång till ImGui om installerat

## Compile Definitions

CMake definierar automatiskt:
- `HAS_IMGUI=1` - Om Dear ImGui finns
- `HAS_IMGUIZMO=1` - Om ImGuizmo finns
- `HAS_IMNODES=1` - Om imnodes finns

Använd i kod:
```cpp
#ifdef HAS_IMGUI
    // ImGui-specifik kod
#endif
```

## Framtida Förbättringar

1. **Hot Reload** - Använd efsw för att ladda om JSON/assets
2. **JSON Schema** - Validera data vid laddning
3. **Node Editor** - Dialog/quest flows med imnodes
4. **Asset Browser** - ImGui-baserad asset preview
5. **Profiling** - Tracy integration för performance

## Build Commands

```powershell
# Alla targets
cmake --build build --config Release

# Specifikt target
cmake --build build --config Release --target RetroEditor
```
