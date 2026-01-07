---
trigger: always_on
description: Workspace configuration for Retro Engine project
---

# Workspace Rules

> Projektspecifika regler och konfiguration

## Project Info

| Property | Value |
|----------|-------|
| **Namn** | Retro Engine |
| **Språk** | C++17 |
| **Platform** | Windows 11 |
| **Build** | CMake + vcpkg |

---

## Folder Structure

```
retro-cplus-game/
 src/
    engine/       # Core engine (static lib)
    game/         # Game executable
    editor/       # Editor executable
 assets/           # Game assets
 docs/             # Documentation
 build/            # CMake output
```

---

## Build Commands

```powershell
# Build Release
cd build; cmake --build . --config Release

# Run Game
.\build\Release\RetroGame.exe

# Run Editor
.\build\Release\RetroEditor.exe
```

---

## Communication

- **Språk:** Svenska
- **Kodstil:** Engelska
- **Stil:** Koncis och direkt

---

## Key Workflows

| Workflow | Beskrivning |
|----------|-------------|
| `/init-session` | Starta arbetsdag |
| `/build` | Bygg projektet |
| `/git-commit` | Committa ändringar |
| `/end-session` | Avsluta arbetsdag |
| `/update-commentary` | Uppdatera kommentarer |
| `/update-docs` | Uppdatera dokumentation |

---

## Assets Location

| Typ | Mapp |
|-----|------|
| Sprites | `assets/sprites/` |
| Backgrounds | `assets/backgrounds/` |
| Sounds | `assets/sounds/` |
| Music | `assets/music/` |
| Data | `assets/data/` |
