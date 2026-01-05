# Retro Adventure (C++ / SDL2)

Ett point-and-click adventure-spel för Windows, byggt med C++17 och SDL2.

## Features

- **State Management** - MenuState, PlayState, PauseState, DialogState
- **Room System** - Rum med hotspots, NPCs, transitions
- **Dialog System** - Dialogträd med val och villkor
- **Inventory System** - Items, pickup, kombination
- **Quest System** - Objectives med auto-completion
- **AI System** - NPC-beteenden, scheman, waypoints
- **Save/Load** - JSON serialization med flags och counters
- **Cutscene System** - Scripted sequences med fade och text
- **UI Widgets** - Button, Label, Panel, ProgressBar

## Krav

- CMake 3.16+
- SDL2, SDL2_image, SDL2_mixer, SDL2_ttf
- nlohmann/json
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

## Kontroller

| Tangent | Funktion |
|---------|----------|
| WASD / Piltangenter | Rörelse |
| Vänsterklick | Interagera / Gå |
| ESC | Pausmeny |
| I | Inventory |
| J | Quest Log |

## Projektstruktur

```
src/
├── main.cpp           # Entry point
├── Game.cpp/h         # Spelloop och init
├── Room.cpp/h         # Rum med hotspots
├── states/            # Game states (8 st)
├── systems/           # Spelsystem (7 st)
├── entities/          # Entity hierarki
├── graphics/          # Rendering och animation
├── audio/             # Ljud och musik
├── data/              # JSON data-laddning
└── ui/                # Widget-system
```

## Parallellt projekt

Detta är C++/SDL2-versionen av DOS-spelet i `retro-game`.
