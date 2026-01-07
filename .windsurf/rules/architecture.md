---
trigger: always_on
description: System architecture and folder structure for Retro Engine
---

# Architecture

> C++17 SDL2 Game Engine med skalbar arkitektur

## Overview

Tre-delad arkitektur för separation of concerns:

| Target | Typ | Innehåll |
|--------|-----|----------|
| **RetroCore** | Static lib | Engine: rendering, physics, audio, world |
| **RetroGame** | Executable | Spellogik, states, content |
| **RetroEditor** | Executable | ImGui editor, asset pipeline |

---

## Folder Structure

```
retro-cplus-game/
 CMakeLists.txt              # Root build config
 src/
    engine/                 # RetroCore - Engine library
       core/               # Object, Actor, Component base classes
       components/         # All components (Sprite, Physics, etc.)
       systems/            # Game systems (Audio, Input)
       physics/            # Box2D integration
       graphics/           # Rendering, textures, animation
       audio/              # AudioManager, music, SFX
       world/              # World, Level, Scene hierarchy
       data/               # Data loaders, serialization
   
    game/                   # RetroGame - Game executable
       states/             # Game states (Menu, Play, Dialog)
       Game.cpp            # Entry point
       main.cpp            # Main function
   
    editor/                 # RetroEditor - Editor executable
        core/               # Editor framework
        panels/             # ImGui panels
        commands/           # Undo/Redo commands
        managers/           # Editor systems

 assets/
    sprites/                # Character sprites, items
    backgrounds/            # Room backgrounds
    sounds/                 # Sound effects (.wav)
    music/                  # Background music (.ogg)
    data/                   # JSON data files

 docs/
    ROADMAP.md
    CHANGELOG.md
    dev/                    # Session reports, DEVLOG

 build/                      # CMake output
```

---

## Key Classes

### Core Hierarchy
```
Object (abstract root)
 ActorObject (transform, visibility)
     ActorObjectExtended (components)
         CharacterActor (player, NPC)
         EnvironmentActor (props, tiles)
         InteractiveActor (hotspots)
```

### World Hierarchy
```
World (game root)
 Level (chapter/area)
     Scene (room/level)
         Actors
```

### Component System
```cpp
// Add components to actors
auto* sprite = actor->addComponent<SpriteComponent>();
auto* physics = actor->addComponent<RigidBody2DComponent>();
```

---

## Design Patterns

| Pattern | Användning |
|---------|------------|
| **Component** | Actor functionality via composition |
| **State** | Game states (Menu, Play, Pause) |
| **Command** | Editor undo/redo |
| **Singleton** | Managers (Texture, Audio) |
| **Observer** | Event system |

---

## Dependencies

| Library | Version | Användning |
|---------|---------|------------|
| SDL2 | 2.x | Window, rendering, input |
| SDL2_image | 2.x | PNG texture loading |
| SDL2_mixer | 2.x | Audio playback |
| Box2D | 3.x | Physics simulation |
| nlohmann/json | 3.x | JSON serialization |
| Dear ImGui | docking | Editor UI |
| imnodes | - | Node editor graphs |
