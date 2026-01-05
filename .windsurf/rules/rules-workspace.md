---
description: Projektregler för Retro Adventure C++ SDL2 spelet
trigger: always_on
---

# Retro Adventure - Workspace Rules

## Projekt

- **Namn:** Retro Adventure
- **Språk:** C++17
- **Plattform:** Windows 11
- **Build:** CMake 3.16+ med vcpkg

## Tech Stack

| Bibliotek | Användning |
|-----------|------------|
| SDL2 | Fönster, rendering, input |
| SDL2_image | PNG-texturer |
| SDL2_mixer | Musik och ljudeffekter |
| vcpkg | Pakethantering |

## Build-kommandon

```powershell
# Bygg
cd build
cmake --build . --config Release

# Kör
.\build\Release\RetroAdventure.exe
```

## Kodstil

### Naming
- **Klasser:** PascalCase (`PlayerCharacter`, `AudioManager`)
- **Metoder:** camelCase (`handleEvent`, `setTarget`)
- **Medlemsvariabler:** m_ prefix (`m_position`, `m_health`)
- **Konstanter:** SCREAMING_SNAKE (`SCREEN_WIDTH`)

### Filstruktur
```
src/
├── states/      # Game states (State Pattern)
├── entities/    # Entity hierarchy (OOP)
├── components/  # Shared functionality
├── graphics/    # Texture, sprite, animation
├── audio/       # Sound management
├── systems/     # Game systems
└── ui/          # UI widgets
```

### Kommentarer
- Fil-header med `@file` och `@brief`
- Klass-dokumentation
- Metod-dokumentation för non-obvious funktioner
- Inline-kommentarer för komplex logik
- **Inga** kommentarer för självklara saker

## Design Patterns

1. **State Pattern** - Game states (Menu, Play, Options)
2. **Singleton** - Managers (TextureManager, AudioManager)
3. **Composition** - Entity components
4. **Observer** - Event system (framtida)

## OOP-regler

- Prefer composition over inheritance
- Max 3 nivåer i arvshierarki
- Virtual destructor i alla basklasser
- Använd `override` keyword
- `unique_ptr` för ownership
- Forward declarations i headers

## Workflows

- `/git-commit` - Commit med konventionellt format
- `/update-commentary` - Uppdatera kodkommentarer
- `/update-docs` - Uppdatera dokumentation

## Viktiga filer

| Fil | Beskrivning |
|-----|-------------|
| `docs/ROADMAP.md` | Utvecklingsplan och progress |
| `docs/ARCHITECTURE.md` | Teknisk arkitektur |
| `CMakeLists.txt` | Build-konfiguration |

## Kommunikation

- **Språk:** Svenska
- **Stil:** Koncis och direkt
- Minimala implementationer
- Följ projektets befintliga mönster

## Assets

```
assets/
├── sprites/     # Karaktärer och objekt
├── backgrounds/ # Rum-bakgrunder
├── sounds/      # Ljudeffekter (.wav)
├── music/       # Bakgrundsmusik (.ogg)
└── fonts/       # Typsnitt
```
