---
description: Komplett feature-utveckling för Retro Engine
auto_execution_mode: 1
---

# New Feature Workflow

> Enterprise feature development för Retro Engine

## Fas 1: Planering

### 1.1 Definiera Feature

| Fråga | Svar |
|-------|------|
| **Vad?** | [Kort beskrivning av featuren] |
| **Varför?** | [Problemet som löses] |
| **Hur?** | [Teknisk approach] |
| **Beroenden?** | [Andra system som påverkas] |
| **Estimat?** | [Tid: timmar/dagar] |

### 1.2 Feature Kategorier

| Kategori | Scope | Exempel |
|----------|-------|---------|
| **Actor** | `core`, `actors` | PlayerActor, NPCActor, HotspotActor |
| **Component** | `components` | SpriteComponent, AnimationComponent |
| **System** | `world` | Scene, Level, World |
| **Editor** | `editor`, `panels` | ViewportPanel, InspectorPanel |
| **Data** | `data` | DataLoader, JSON schema |
| **Graphics** | `graphics` | TextureManager, rendering |
| **Audio** | `audio` | AudioManager, sound effects |
| **Physics** | `physics` | Box2D integration |

### 1.3 Design Considerations

```markdown
## Feature Design: [Namn]

### Problem
[Beskriv problemet som ska lösas]

### Lösning
[Beskriv den tekniska lösningen]

### API Design
[Hur ska featuren användas?]

### Files Affected
- `src/engine/xxx/NewClass.h` - Ny fil
- `src/engine/xxx/ExistingClass.cpp` - Modifieras
- `CMakeLists.txt` - Lägg till nya filer

### Dependencies
- Kräver: [System X, System Y]
- Påverkar: [System Z]

### Testing Strategy
- Unit tests: [Vad ska testas]
- Integration tests: [Hur interagerar med andra system]
- Manual tests: [Steg att verifiera]
```

---

## Fas 2: Setup

### 2.1 Skapa Feature Branch

```powershell
# Hämta senaste main
git checkout main
git pull origin main

# Skapa feature branch
git checkout -b feature/description

# Verifiera
git branch --show-current
```

### 2.2 Branch Namngivning

| Prefix | Användning | Exempel |
|--------|------------|---------|
| `feature/` | Ny funktionalitet | `feature/player-walk-area` |
| `bugfix/` | Buggfix | `bugfix/scene-loading` |
| `hotfix/` | Kritisk production fix | `hotfix/crash-on-save` |
| `refactor/` | Refaktorering | `refactor/actor-hierarchy` |
| `docs/` | Dokumentation | `docs/api-reference` |
| `spike/` | Research/prototyp | `spike/new-physics` |

### 2.3 Verifiera Startläge

// turbo
```powershell
cd build; cmake --build . --config Release
.\Release\RetroGame.exe
```

---

## Fas 3: Implementation

### 3.1 Skapa Header-fil

**Actor Template:**
```cpp
/**
 * @file NewActor.h
 * @brief [Kort beskrivning]
 * 
 * @details
 * [Detaljerad beskrivning av vad klassen gör,
 * hur den används, och eventuella speciella considerations]
 * 
 * @author [Namn]
 * @date YYYY-MM-DD
 * 
 * @see ActorObjectExtended
 * @see RelatedClass
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include <string>

/**
 * @class NewActor
 * @brief [Kort klass-beskrivning]
 * 
 * @details
 * [Detaljerad beskrivning av klassens syfte och ansvar]
 * 
 * @par Components
 * - SpriteComponent - Rendering
 * - AnimationComponent - Animation playback
 * 
 * @par Usage
 * @code
 * auto actor = std::make_unique<NewActor>("MyActor");
 * actor->setPosition(100.0f, 200.0f);
 * scene->addActor(std::move(actor));
 * @endcode
 */
class NewActor : public ActorObjectExtended {
public:
    /**
     * @brief Constructs a new NewActor
     * @param name Unique name for this actor
     */
    explicit NewActor(const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~NewActor() override = default;
    
    // ----- Lifecycle -----
    
    /**
     * @brief Updates actor state
     * @param deltaTime Time since last frame in seconds
     */
    void update(float deltaTime) override;
    
    /**
     * @brief Renders the actor
     * @param renderer SDL renderer to draw to
     */
    void render(SDL_Renderer* renderer) override;
    
    // ----- Public API -----
    
    /**
     * @brief [Method description]
     * @param param1 [Parameter description]
     * @return [Return value description]
     */
    bool someMethod(int param1);
    
private:
    // ----- Member Variables -----
    
    /** @brief [Description of member] */
    int m_someValue = 0;
    
    /** @brief [Description of member] */
    float m_anotherValue = 1.0f;
};
```

**Component Template:**
```cpp
/**
 * @file NewComponent.h
 * @brief [Kort beskrivning]
 */
#pragma once

#include "engine/components/ActorComponent.h"

/**
 * @class NewComponent
 * @brief [Kort klass-beskrivning]
 */
class NewComponent : public ActorComponent {
public:
    NewComponent() = default;
    ~NewComponent() override = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // ----- Configuration -----
    
    void setSomething(int value);
    int getSomething() const;
    
private:
    int m_something = 0;
};
```

### 3.2 Skapa Implementation-fil

```cpp
/**
 * @file NewActor.cpp
 * @brief Implementation of NewActor
 */

#include "NewActor.h"
#include "engine/components/SpriteComponent.h"
#include <iostream>

NewActor::NewActor(const std::string& name)
    : ActorObjectExtended(name)
{
    // Lägg till components
    auto* sprite = addComponent<SpriteComponent>();
    if (sprite) {
        // Configure sprite
    }
}

void NewActor::update(float deltaTime) {
    // Call parent
    ActorObjectExtended::update(deltaTime);
    
    // Actor-specific update logic
}

void NewActor::render(SDL_Renderer* renderer) {
    // Call parent (renders components)
    ActorObjectExtended::render(renderer);
    
    // Actor-specific rendering (if any)
}

bool NewActor::someMethod(int param1) {
    m_someValue = param1;
    return true;
}
```

### 3.3 Uppdatera CMakeLists.txt

```cmake
# Lägg till nya filer i target_sources
target_sources(RetroCore PRIVATE
    # ... existing files ...
    
    # New files
    src/engine/actors/NewActor.cpp
    src/engine/actors/NewActor.h
)
```

### 3.4 Implementation Guidelines

| Regel | Beskrivning |
|-------|-------------|
| **Naming** | PascalCase för klasser, camelCase för metoder, m_ prefix för members |
| **Includes** | Sortera: samma modul → engine → third-party → standard library |
| **Memory** | Använd `std::unique_ptr` för ownership, raw pointers för non-owning |
| **Error handling** | Returnera bool eller use std::optional, logga errors |
| **Const correctness** | Markera const metoder, const& för pass-by-reference |

---

## Fas 4: Testning

### 4.1 Build och Kör

// turbo
```powershell
cd build; cmake --build . --config Release
```

### 4.2 Smoke Test

```powershell
# Testa spelet
.\Release\RetroGame.exe

# Testa editorn
.\Release\RetroEditor.exe
```

### 4.3 Feature-specifik Testning

```markdown
## Test Plan: [Feature Name]

### Unit Tests
- [ ] Test A: [Beskrivning]
- [ ] Test B: [Beskrivning]

### Integration Tests
- [ ] Feature interagerar korrekt med System X
- [ ] Data sparas/laddas korrekt

### Manual Tests
1. [ ] Starta spelet
2. [ ] Utför action X
3. [ ] Verifiera resultat Y
4. [ ] Kontrollera inga errors i console

### Edge Cases
- [ ] Null input
- [ ] Tom data
- [ ] Stora mängder data

### Regression
- [ ] Befintliga features fungerar fortfarande
- [ ] Inga nya warnings/errors vid build
```

### 4.4 Verifiera Inga Regressioner

```powershell
# Kör alla tester
ctest -C Release --output-on-failure

# Manuell verifiering av relaterade features
```

---

## Fas 5: Dokumentation

### 5.1 Kod-dokumentation

Verifiera att alla publika API:er är dokumenterade:

- [ ] Fil-header med `@file`, `@brief`, `@author`, `@date`
- [ ] Klass-dokumentation med `@class`, `@brief`, `@details`
- [ ] Metoder med `@brief`, `@param`, `@return`
- [ ] Members med `@brief`
- [ ] Inline-kommentarer för komplex logik

### 5.2 CHANGELOG Entry

Lägg till under `[Unreleased]` i `docs/CHANGELOG.md`:

```markdown
## [Unreleased]

### Added
- `NewActor` - [Kort beskrivning av vad den gör]
- `NewComponent` - [Kort beskrivning]

### Changed
- `ExistingClass` - [Vad ändrades och varför]
```

### 5.3 ROADMAP Update

Uppdatera `docs/ROADMAP.md` om det är en milestone:

```markdown
## Fas X: [Fasnamn]
- [x] NewActor implementation
- [ ] Nästa uppgift
```

### 5.4 API Documentation (Om Public)

Skapa `docs/api/NewActor.md` för viktiga klasser:

```markdown
# NewActor

## Overview
[Beskrivning av vad klassen gör]

## Usage
```cpp
auto actor = std::make_unique<NewActor>("MyActor");
actor->configure(params);
scene->addActor(std::move(actor));
```

## API Reference

### Constructor
`NewActor(const std::string& name)`
- **name**: Unikt namn för actorn

### Methods
#### `void someMethod(int param)`
[Beskrivning]

## See Also
- [RelatedActor](RelatedActor.md)
- [SpriteComponent](SpriteComponent.md)
```

---

## Fas 6: Code Review Preparation

### 6.1 Self-Review

```powershell
# Se alla ändringar
git diff main...HEAD

# Se ändrade filer
git diff main...HEAD --stat
```

### 6.2 Self-Review Checklista

| Kategori | Krav |
|----------|------|
| **Kod** | Följer coding standards |
| **Kod** | Ingen debug-kod kvar |
| **Kod** | Ingen utkommenterad kod |
| **Kod** | Inga TODOs utan issue-referens |
| **Build** | Kompilerar utan errors |
| **Build** | Inga nya warnings |
| **Test** | Feature fungerar |
| **Test** | Inga regressioner |
| **Docs** | Fil-headers kompletta |
| **Docs** | Klasser dokumenterade |
| **Docs** | CHANGELOG uppdaterad |

### 6.3 Final Build Check

// turbo
```powershell
cd build; cmake --build . --config Release
```

---

## Fas 7: Commit

### 7.1 Stage Ändringar

```powershell
# Granska först
git status
git diff

# Stage allt
git add .

# ELLER stage selektivt
git add src/engine/actors/NewActor.cpp
git add src/engine/actors/NewActor.h
git add CMakeLists.txt
```

### 7.2 Commit Message

```powershell
git commit -m "feat(actors): add NewActor with XYZ functionality

- Inherits from ActorObjectExtended
- Includes SpriteComponent for rendering
- Integrates with Scene system
- Supports configuration via JSON

Components:
- SpriteComponent
- AnimationComponent (optional)

Files:
- NewActor.h/cpp - Main implementation
- CMakeLists.txt - Added to build

Usage:
auto actor = make_unique<NewActor>(\"name\");
scene->addActor(move(actor));"
```

### 7.3 Verifiera Commit

// turbo
```powershell
git log -1 --stat
```

---

## Quick Reference

### Retro Engine Fil-struktur

```
src/
├── engine/
│   ├── core/              # Object, Actor, Component base
│   │   ├── Object.h
│   │   ├── ActorObject.h
│   │   └── ActorObjectExtended.h
│   ├── actors/            # Konkreta actors
│   │   ├── CharacterActor.h
│   │   ├── PlayerActor.h
│   │   └── SpriteActor.h
│   ├── components/        # Alla components
│   │   ├── ActorComponent.h
│   │   ├── SpriteComponent.h
│   │   └── AnimationComponent.h
│   ├── world/             # World, Level, Scene
│   │   ├── World.h
│   │   ├── Level.h
│   │   └── Scene.h
│   ├── data/              # Data loading
│   │   └── DataLoader.h
│   ├── graphics/          # Rendering
│   ├── audio/             # Audio
│   └── physics/           # Box2D
├── game/                  # Spellogik
│   └── states/            # Game states
└── editor/                # Editor
    ├── panels/            # ImGui panels
    └── commands/          # Undo/Redo
```

### Actor Hierarki

```
Object (abstract)
└── ActorObject
    └── ActorObjectExtended (+ components)
        ├── CharacterActor
        │   ├── PlayerActor
        │   └── NPCActor
        ├── EnvironmentActor
        ├── InteractiveActor
        └── SpriteActor
```

### Component System

```cpp
// Lägg till component
auto* sprite = actor->addComponent<SpriteComponent>();

// Hämta component
auto* sprite = actor->getComponent<SpriteComponent>();

// Komponenter uppdateras automatiskt av ActorObjectExtended
```

---

## ⚠️ VIKTIGA REGLER

1. **ALDRIG git commit/push utan explicit instruktion**
2. **Följ coding standards** - PascalCase, m_ prefix, etc.
3. **Dokumentera all ny kod** - Doxygen format
4. **Testa innan commit** - Build + smoke test
5. **En feature per branch** - Håll commits fokuserade
