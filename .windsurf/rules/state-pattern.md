---
trigger: glob
globs: ["src/states/*.cpp", "src/states/*.h"]
---

# State Pattern Guidelines

<state_interface>
Alla states implementerar IState:
`cpp
class IState {
public:
    virtual ~IState() = default;
    virtual void enter() = 0;      // När state aktiveras
    virtual void exit() = 0;       // När state lämnas
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;
};
`
</state_interface>

<state_manager>
StateManager använder en stack:
- `pushState()` - Lägg till state ovanpå (för pause, inventory)
- `popState()` - Ta bort översta (återgå)
- `changeState()` - Ersätt nuvarande (menu  play)
</state_manager>

<planned_states>
1. MenuState - Huvudmeny
2. PlayState - Gameplay
3. PauseState - Pausmeny (overlay)
4. InventoryState - Inventory UI
5. DialogState - NPC dialog
6. SaveLoadState - Spara/ladda meny
</planned_states>

<state_transitions>
`
MenuState [Start] PlayState
PlayState [ESC] PauseState (push)
PlayState [I] InventoryState (push)
PlayState [Talk] DialogState (push)
PauseState [ESC] PlayState (pop)
PauseState [Quit] MenuState (change)
`
</state_transitions>
