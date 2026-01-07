# Retro Engine

> Enterprise C++17 game engine med SDL2, Box2D och Dear ImGui

[![Build](https://img.shields.io/badge/build-passing-brightgreen)]()
[![C++17](https://img.shields.io/badge/C++-17-blue)]()
[![SDL2](https://img.shields.io/badge/SDL2-2.x-orange)]()
[![Box2D](https://img.shields.io/badge/Box2D-3.1.1-purple)]()

## Overview

Retro Engine är en UE5-inspirerad Actor/Component-baserad spelmotor för 2D adventure och platformer-spel. Stödjer både point-and-click adventure och physics-baserad platformer gameplay.

## Features

### Engine (RetroCore)
- **Actor System** - UE5-inspirerad Object → ActorObject → ActorObjectExtended hierarki
- **Component System** - SpriteComponent, AnimationComponent, RigidBody2DComponent
- **World Hierarchy** - World → Level → Scene container architecture
- **Box2D Physics** - Full 2D physics med CharacterController2D
- **Game Modes** - Adventure (point-and-click) och Platformer (physics-based)
- **State Management** - MenuState, PlayState, PauseState, DialogState, etc.
- **Dialog System** - Dialogträd med val och villkor
- **Inventory System** - Items, pickup, kombination
- **Quest System** - Objectives med auto-completion
- **AI System** - NPC-beteenden, scheman, waypoints
- **Save/Load** - JSON serialization

### Editor (RetroEditor)
- **Dear ImGui** - Professionell docking-baserad layout
- **Hierarchy Panel** - World/Level/Scene/Actor navigation
- **Properties Panel** - OOP-baserade property editors
- **Viewport Panel** - Spatial view med zoom och grid
- **Physics Editor** - Collision boxes och triggers
- **Place Actors Panel** - UE5-style drag-and-drop
- **Undo/Redo** - Command pattern

### Game (RetroGame)
- **Point-and-Click** - Adventure gameplay
- **Platformer** - Physics-based gameplay
- **Cutscenes** - Scripted sequences
- **UI Widgets** - Button, Label, Panel, ProgressBar

## Architecture

```
Object (abstract)
└── ActorObject
    └── ActorObjectExtended (+ components)
        ├── CharacterActor
        │   ├── PlayerActor
        │   └── NPCActor
        ├── EnvironmentActor
        │   ├── TileMapActor
        │   └── ParallaxActor
        ├── InteractiveActor
        └── SpriteActor

World (game root)
└── Level (chapter/area)
    └── Scene (room)
        └── Actors
```

## Requirements

- **CMake** 3.16+
- **C++17** compiler (MSVC 2019+ or Clang)
- **vcpkg** for dependencies

### Dependencies (via vcpkg)

| Package | Version | Purpose |
|---------|---------|---------|
| SDL2 | 2.x | Window, rendering, input |
| SDL2_image | 2.x | PNG loading |
| SDL2_mixer | 2.x | Audio |
| Box2D | 3.1.1 | 2D Physics |
| GLM | 1.0.2 | Math library |
| nlohmann-json | 3.x | JSON serialization |
| imgui[docking] | 1.x | Editor UI |
| imnodes | 0.x | Node graphs |

## Quick Start

### Install Dependencies

```powershell
# vcpkg (om inte installerat)
git clone https://github.com/Microsoft/vcpkg.git C:\vcpkg
cd C:\vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install

# Dependencies
vcpkg install sdl2:x64-windows
vcpkg install sdl2-image:x64-windows
vcpkg install sdl2-mixer:x64-windows
vcpkg install box2d:x64-windows
vcpkg install glm:x64-windows
vcpkg install nlohmann-json:x64-windows
vcpkg install imgui[docking-experimental]:x64-windows
vcpkg install imnodes:x64-windows
```

### Build

```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### Run

```powershell
# Game
.\Release\RetroGame.exe

# Editor
.\Release\RetroEditor.exe
```

## Controls

| Input | Action |
|-------|--------|
| **Left Click** | Move / Interact |
| **WASD** | Movement (platformer) |
| **Space** | Jump (platformer) |
| **ESC** | Pause menu |
| **I** | Inventory |
| **J** | Quest log |
| **M** | Mute toggle |

## Project Structure

```
retro-cplus-game/
├── src/
│   ├── engine/              # RetroCore static library
│   │   ├── core/            # Object, Actor, Component
│   │   ├── actors/          # CharacterActor, SpriteActor, etc.
│   │   ├── components/      # SpriteComponent, AnimationComponent
│   │   ├── world/           # World, Level, Scene
│   │   ├── physics/         # Box2D integration
│   │   ├── graphics/        # TextureManager, rendering
│   │   ├── audio/           # AudioManager
│   │   └── data/            # DataLoader, JSON
│   │
│   ├── game/                # RetroGame executable
│   │   └── states/          # MenuState, PlayState, etc.
│   │
│   └── editor/              # RetroEditor executable
│       ├── core/            # EditorContext, EditorState
│       ├── managers/        # PanelManager, WorldManager
│       ├── panels/          # ImGui panels
│       └── commands/        # Undo/Redo
│
├── assets/                  # Game assets
│   ├── data/                # JSON data files
│   ├── sprites/             # Character sprites
│   ├── backgrounds/         # Scene backgrounds
│   └── audio/               # Music and SFX
│
├── docs/                    # Documentation
│   ├── CHANGELOG.md
│   ├── ROADMAP.md
│   └── dev/                 # Development logs
│
└── .windsurf/               # AI workflows
    ├── rules/               # Coding standards
    └── workflows/           # Development workflows
```

## Documentation

- [CHANGELOG](docs/CHANGELOG.md) - Release notes
- [ROADMAP](docs/ROADMAP.md) - Development plan
- [Architecture](docs/architecture/) - Technical docs

## Workflows

Projektet använder enterprise-grade workflows för utveckling:

| Workflow | Description |
|----------|-------------|
| `/init-session` | Starta utvecklingssession |
| `/end-session` | Avsluta med dokumentation |
| `/git-commit` | Conventional commits |
| `/build` | Build och felsökning |
| `/debug` | Debugging-process |
| `/new-feature` | Feature-utveckling |
| `/generate-pr` | Pull Request-skapande |

## License

Proprietary - Denker Systems
