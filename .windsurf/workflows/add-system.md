---
description: Lägger till ett nytt spelsystem enligt arkitekturen
---

# Add System Workflow

Följ detta för att lägga till ett av de planerade systemen från ROADMAP.

## System att implementera (i ordning)
1. StateManager - Game state hantering
2. TextureManager - Textur-laddning och cache
3. AudioManager - Musik och SFX
4. RoomSystem - Rum och navigation
5. DialogSystem - NPC dialoger
6. InventorySystem - Items och inventory
7. QuestSystem - Quests och mål
8. AISystem - NPC beteende
9. SaveSystem - Spara/ladda

## Steg för StateManager (exempel)

### 1. Skapa mapp
// turbo
`powershell
New-Item -ItemType Directory -Path "src/states" -Force
`

### 2. Skapa IState.h (interface)
`cpp
#pragma once
#include <SDL.h>

class Game;

class IState {
public:
    virtual ~IState() = default;
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;
    
    void setGame(Game* game) { m_game = game; }
protected:
    Game* m_game = nullptr;
};
`

### 3. Skapa StateManager.h
`cpp
#pragma once
#include <stack>
#include <memory>
#include "IState.h"

class StateManager {
public:
    void pushState(std::unique_ptr<IState> state);
    void popState();
    void changeState(std::unique_ptr<IState> state);
    void update(float dt);
    void render(SDL_Renderer* renderer);
    void handleEvent(const SDL_Event& event);
    bool isEmpty() const;
private:
    std::stack<std::unique_ptr<IState>> m_states;
};
`

### 4. Uppdatera CMakeLists.txt
Lägg till nya .cpp filer i SOURCES.

### 5. Integrera i Game
Ersätt direkt rendering med StateManager.

### 6. Testa
// turbo
`powershell
cd build; cmake --build . --config Release
`
