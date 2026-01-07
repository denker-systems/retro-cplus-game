---
trigger: always_on
description: State pattern for game flow
---

# State Pattern

> Game state management

## IState Interface

```cpp
class IState {
public:
    virtual ~IState() = default;
    
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* r) = 0;
    virtual void handleEvent(const SDL_Event& e) = 0;
};
```

---

## StateManager

```cpp
class StateManager {
    std::stack<std::unique_ptr<IState>> m_states;
    
public:
    void pushState(std::unique_ptr<IState> state);
    void popState();
    void changeState(std::unique_ptr<IState> state);
    
    void update(float dt);
    void render(SDL_Renderer* r);
    void handleEvent(const SDL_Event& e);
};
```

---

## Game States

| State | Purpose |
|-------|---------|
| MenuState | Main menu |
| PlayState | Gameplay |
| PauseState | Pause overlay |
| DialogState | NPC dialog |
| InventoryState | Inventory UI |

---

## Transitions

```
MenuState ──[Start]──▶ PlayState
PlayState ──[ESC]────▶ PauseState (push)
PlayState ──[Talk]───▶ DialogState (push)
PauseState ─[Resume]─▶ PlayState (pop)
```

---

## Usage

```cpp
// Push overlay
stateManager.pushState(std::make_unique<PauseState>());

// Pop back
stateManager.popState();

// Replace
stateManager.changeState(std::make_unique<MenuState>());
```
