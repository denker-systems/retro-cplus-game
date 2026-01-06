# Retro Adventure (C++ / SDL2)

Ett point-and-click adventure-spel för Windows, byggt med C++17 och SDL2.

## Features

### Spel
- **State Management** - MenuState, PlayState, PauseState, DialogState
- **Scene System** - Scener med hotspots, NPCs, transitions (ersätter Room system)
- **Dialog System** - Dialogträd med val och villkor
- **Inventory System** - Items, pickup, kombination
- **Quest System** - Objectives med auto-completion
- **AI System** - NPC-beteenden, scheman, waypoints
- **Save/Load** - JSON serialization med flags och counters
- **Cutscene System** - Scripted sequences med fade och text
- **UI Widgets** - Button, Label, Panel, ProgressBar
- **Depth Scaling** - Perspektiv-skalning av karaktärer baserat på Y-position

### Editor (Dear ImGui)
- **Professionellt UI** - Docking-baserad layout med flyttbara paneler
- **Hierarchy Panel** - Träd med rum, dialoger, quests, items
- **Properties Panel** - Inspector för valt objekt
- **Viewport Panel** - Rum-preview med zoom och overlays
- **Asset Browser** - Grid/List view, filter, search
- **Console Panel** - Logg med timestamps och log levels
- **Visuell redigering** - Drag-and-drop för hotspots och walk areas
- **Data Serialization** - EditorDataManager för World/Scene serialization
- **Background Actor System** - SpriteComponent-baserad bakgrundsrendering

## Krav

- CMake 3.16+
- SDL2, SDL2_image, SDL2_mixer, SDL2_ttf
- nlohmann/json
- Dear ImGui (docking branch)
- Visual Studio 2019+ eller MinGW

## Installation av SDL2 (vcpkg)

```powershell
# Installera vcpkg om det inte finns
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Installera SDL2
.\vcpkg install sdl2:x64-windows
.\vcpkg install sdl2-image:x64-windows
.\vcpkg install sdl2-mixer:x64-windows

# Integrera med CMake
.\vcpkg integrate install
```

## Bygga

```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-path]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### Köra Spelet
```powershell
.\build\Release\RetroAdventure.exe
```

### Köra Editorn
```powershell
.\build\Release\RetroEditor.exe
```

## Kontroller

| Tangent | Funktion |
|---------|----------|
| WASD / Piltangenter | Rörelse |
| Vänsterklick | Interagera / Gå |
| ESC | Pausmeny |
| I | Inventory |
| J | Quest Log |
| M | Mute/Unmute ljud |
| F1 | Öppna Editor |

## Projektstruktur

```
src/
├── main.cpp           # Entry point (spel)
├── Game.cpp/h         # Spelloop och init
├── Room.cpp/h         # Rum med hotspots
├── states/            # Game states (9 st)
├── systems/           # Spelsystem (7 st)
├── entities/          # Entity hierarki
├── graphics/          # Rendering och animation
├── audio/             # Ljud och musik
├── data/              # JSON data-laddning
├── ui/                # Widget-system
├── editor/            # ImGui-baserad editor
│   └── panels/        # HierarchyPanel, PropertiesPanel, etc.
└── vendor/            # Vendored ImGui backends
```

## Parallellt projekt

Detta är C++/SDL2-versionen av DOS-spelet i `retro-game`.
