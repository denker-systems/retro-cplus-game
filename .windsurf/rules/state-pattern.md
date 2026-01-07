---
trigger: glob
globs: ["src/game/states/*.cpp", "src/game/states/*.h"]
description: State Pattern for game flow management
---

# State Pattern

> Game states för spelflöde

## IState Interface

```cpp
class IState {
public:
    virtual ~IState() = default;
    
    virtual void enter() = 0;       // Called when state activates
    virtual void exit() = 0;        // Called when state deactivates
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;
};
```

---

## StateManager

```cpp
class StateManager {
    std::stack<std::unique_ptr<IState>> m_states;
    
public:
    void pushState(std::unique_ptr<IState> state);  // Add overlay
    void popState();                                  // Remove top
    void changeState(std::unique_ptr<IState> state); // Replace
};
```

---

## Game States

| State | Användning |
|-------|------------|
| MenuState | Huvudmeny |
| PlayState | Gameplay |
| PauseState | Pausmeny (overlay) |
| DialogState | NPC dialog (overlay) |
| InventoryState | Inventory UI (overlay) |
| SaveLoadState | Spara/ladda |

---

## State Transitions

```
MenuState [Start] PlayState
PlayState [ESC] PauseState (push)
PlayState [I] InventoryState (push)
PlayState [Talk] DialogState (push)
PauseState [ESC] PlayState (pop)
PauseState [Quit] MenuState (change)
```

---

## Implementation

```cpp
// Push overlay state
stateManager.pushState(std::make_unique<PauseState>());

// Pop back to previous
stateManager.popState();

// Replace completely
stateManager.changeState(std::make_unique<MenuState>());
```
