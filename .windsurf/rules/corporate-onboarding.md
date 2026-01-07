---
description: Corporate Onboarding Guide for New Developers
trigger: always_on
---

# Developer Onboarding Guide

> För nya utvecklare i ett 50+ team

## DAG 1: ENVIRONMENT SETUP

### 1.1 Verktyg att installera
```powershell
# 1. Visual Studio 2022 (Community eller Professional)
# 2. CMake 3.20+
# 3. vcpkg package manager
git clone https://github.com/microsoft/vcpkg C:\vcpkg
.\bootstrap-vcpkg.bat

# 4. Dependencies
vcpkg install sdl2:x64-windows sdl2-image:x64-windows sdl2-mixer:x64-windows
vcpkg install nlohmann-json:x64-windows box2d:x64-windows
vcpkg install imgui[docking-experimental]:x64-windows imnodes:x64-windows
```

### 1.2 Klona repository
```powershell
git clone https://github.com/[org]/retro-cplus-game.git
cd retro-cplus-game
```

### 1.3 Första build
```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
.\Release\RetroGame.exe
```

---

## DAG 2: KODSTRUKTUR

### 2.1 Mappstruktur
```
src/
 engine/              # Core engine (statiskt lib)
    core/            # Object, Actor, Component
    components/      # Alla komponenter
    systems/         # Game systems
    physics/         # Box2D integration
    graphics/        # Rendering
    audio/           # Audio
    world/           # World, Level, Scene

 game/                # Spel-specifik kod (exe)
    states/          # Game states
    Game.cpp         # Entry point

 editor/              # Editor (exe)
     core/            # Editor framework
     panels/          # ImGui panels
     commands/        # Undo/Redo
```

### 2.2 Viktiga klasser att förstå
| Klass | Ansvar |
|-------|--------|
| `Object` | Root base class |
| `ActorObject` | Transform och lifecycle |
| `ActorObjectExtended` | Component support |
| `ActorComponent` | Base för komponenter |
| `Scene` | Actor container |
| `Level` | Scene container |
| `World` | Level container |

### 2.3 Design Patterns
- **Component Pattern** - Composition over inheritance
- **State Pattern** - Game states (Menu, Play, Pause)
- **Command Pattern** - Editor undo/redo
- **Observer Pattern** - Event system

---

## DAG 3: WORKFLOWS

### 3.1 Dagligt arbete
```
1. /init-session     - Starta dagen
2. [arbeta på task]
3. /git-commit       - Committa ändringar
4. /end-session      - Avsluta dagen
```

### 3.2 Ny feature
```
1. /new-feature      - Skapa feature branch
2. [implementera]
3. /update-docs      - Uppdatera dokumentation
4. /generate-pr      - Skapa PR
```

### 3.3 Code review
- Alla PRs kräver minst 1 approval
- Följ code review checklist
- Svara på kommentarer inom 24h

---

## VECKA 1: FÖRSTA CONTRIBUTION

### Steg 1: Hitta en "good first issue"
Se GitHub Issues märkta med `good-first-issue`

### Steg 2: Skapa feature branch
```powershell
git checkout -b feature/XXX-my-first-feature
```

### Steg 3: Implementera
- Följ coding standards
- Dokumentera all ny kod
- Skriv tester

### Steg 4: Commit och PR
```powershell
git add .
git commit -m "feat(scope): description"
git push origin feature/XXX-my-first-feature
```

### Steg 5: Code review
- Svara på feedback
- Gör begärda ändringar
- Vänta på approval

---

## RESURSER

### Dokumentation
- `docs/README.md` - Quick start
- `docs/architecture/` - Teknisk arkitektur
- `docs/api/` - API reference
- `docs/guides/` - Tutorials

### Kontakter
- **Tech Lead:** [namn]
- **Mentor:** [namn]
- **Slack:** #engine-dev

### Läsning
- [Unreal Engine Coding Standards](https://docs.unrealengine.com/CodingStandard)
- [Game Programming Patterns](https://gameprogrammingpatterns.com/)
- [Data-Oriented Design](https://dataorienteddesign.com/)
