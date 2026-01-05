# State Pattern

## Översikt

Game states hanteras med State Pattern via `StateManager` som använder en stack.

## Diagram

```
         ┌─────────────┐
         │ StateManager│
         │  (stack)    │
         └──────┬──────┘
                │
    ┌───────────┼───────────┐
    ▼           ▼           ▼
┌────────┐ ┌────────┐ ┌────────┐
│MenuState│ │PlayState│ │Options │
└────────┘ └────────┘ └────────┘
```

## States

| State | Beskrivning | Fil |
|-------|-------------|-----|
| MenuState | Huvudmeny | `src/states/MenuState.h/.cpp` |
| PlayState | Gameplay | `src/states/PlayState.h/.cpp` |
| OptionsState | Inställningar | `src/states/OptionsState.h/.cpp` |
| PauseState | Pausmeny | 📋 Planerad |
| DialogState | Dialog-overlay | 📋 Planerad |

## API

```cpp
// Byt state helt
stateManager->changeState(std::make_unique<PlayState>());

// Lägg state ovanpå (overlay)
stateManager->pushState(std::make_unique<PauseState>());

// Ta bort översta
stateManager->popState();
```

## Flöde

```
Start → MenuState → PlayState → PauseState (overlay)
                  ↓
            OptionsState
```

## Se även

- [IState API](../api/core/IState.md)
- [StateManager API](../api/core/StateManager.md)
