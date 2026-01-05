---
description: Skapar en ny feature/system enligt arkitekturen
---

# New Feature Workflow

## 1. Välj feature från roadmap
Kolla `docs/ROADMAP.md` för nästa prioriterade feature.

## 2. Skapa mappstruktur
Baserat på arkitekturen i `docs/ARCHITECTURE.md`:

`powershell
# Exempel för nytt system
New-Item -ItemType Directory -Path "src/systems" -Force
`

## 3. Skapa header (.h)
`cpp
#pragma once

#include <SDL.h>
#include <memory>
#include <string>

class FeatureName {
public:
    FeatureName();
    ~FeatureName();

    void init();
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);

private:
    // Medlemsvariabler med m_ prefix
};
`

## 4. Skapa implementation (.cpp)
`cpp
#include "FeatureName.h"
#include <iostream>

FeatureName::FeatureName() = default;
FeatureName::~FeatureName() = default;

void FeatureName::init() {
    std::cout << "FeatureName initialized" << std::endl;
}

void FeatureName::update(float deltaTime) {
    // Logik här
}

void FeatureName::render(SDL_Renderer* renderer) {
    // Rendering här
}
`

## 5. Lägg till i CMakeLists.txt
`cmake
set(SOURCES
    # ... existing sources
    src/systems/FeatureName.cpp
)
`

## 6. Integrera i Game
`cpp
#include "systems/FeatureName.h"
// Lägg till som unique_ptr member
// Initiera i Game::init()
// Anropa i update() och render()
`

## 7. Bygg och testa
// turbo
`powershell
cd build; cmake --build . --config Release
`

## 8. Commit
`powershell
git add .
git commit -m "feat(systems): add FeatureName system"
`
