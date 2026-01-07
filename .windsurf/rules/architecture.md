---
trigger: always_on
description: System architecture for game engine
---

# Architecture

> Enterprise game engine architecture

## Project Structure

```
retro-cplus-game/
├── src/
│   ├── engine/         # RetroCore - Static library
│   │   ├── core/       # Object, Actor, Component
│   │   ├── components/ # All components
│   │   ├── systems/    # Game systems
│   │   ├── physics/    # Box2D integration
│   │   ├── graphics/   # Rendering
│   │   ├── audio/      # Audio
│   │   ├── world/      # World, Level, Scene
│   │   └── data/       # Data loading
│   │
│   ├── game/           # RetroGame - Executable
│   │   ├── states/     # Game states
│   │   └── Game.cpp
│   │
│   └── editor/         # RetroEditor - Executable
│       ├── core/       # Editor framework
│       ├── panels/     # ImGui panels
│       └── commands/   # Undo/Redo
│
├── assets/             # Game assets
├── docs/               # Documentation
└── build/              # CMake output
```

---

## Core Hierarchy

```
Object (abstract root)
└── ActorObject (transform, visibility)
    └── ActorObjectExtended (components)
        ├── CharacterActor
        ├── EnvironmentActor
        └── InteractiveActor
```

---

## World Hierarchy

```
World (game root)
└── Level (chapter/area)
    └── Scene (room)
        └── Actors
```

---

## Design Patterns

| Pattern | Usage |
|---------|-------|
| Component | Actor functionality |
| State | Game states |
| Command | Editor undo/redo |
| Singleton | Managers |
| Observer | Events |

---

## Dependencies

| Library | Purpose |
|---------|---------|
| SDL2 | Window, rendering, input |
| SDL2_image | PNG loading |
| SDL2_mixer | Audio |
| Box2D | Physics |
| nlohmann/json | Serialization |
| Dear ImGui | Editor UI |
| imnodes | Node editor |

---

## System Priorities

| Priority | Systems |
|----------|---------|
| 0-10 | Input, Time |
| 11-30 | Physics |
| 31-50 | AI, Gameplay |
| 51-70 | Animation, Audio |
| 71-100 | Rendering |
