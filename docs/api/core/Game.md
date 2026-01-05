# Game

> Central spelklass som koordinerar alla system

## Includes
```cpp
#include "Game.h"
```

## Översikt

`Game` är huvudklassen som hanterar SDL-initiering, spelloop och state management. Den koordinerar alla subsystem och håller ihop spelet.

## Konstruktor

```cpp
Game();
~Game();
```

## Publika Metoder

### init
```cpp
bool init(const std::string& title, int width, int height);
```
**Beskrivning:** Initierar SDL, skapar fönster och renderer, laddar managers.

**Parametrar:**
- `title` - Fönstertitel
- `width` - Fönsterbredd (skalas till GAME_WIDTH internt)
- `height` - Fönsterhöjd (skalas till GAME_HEIGHT internt)

**Returnerar:** `true` om initiering lyckades, annars `false`

**Exempel:**
```cpp
Game game;
if (!game.init("Retro Adventure", 1280, 800)) {
    return -1;
}
```

### run
```cpp
void run();
```
**Beskrivning:** Startar huvudspelloopen (blockerar tills spelet avslutas).

**Exempel:**
```cpp
game.run(); // Kör tills användaren stänger
```

### quit
```cpp
void quit();
```
**Beskrivning:** Avslutar spelet och rensar upp alla resurser.

### pushState
```cpp
void pushState(std::unique_ptr<IState> state);
```
**Beskrivning:** Lägger till en ny state på stacken (overlay).

**Parametrar:**
- `state` - State att pusha

**Exempel:**
```cpp
game.pushState(std::make_unique<PauseState>());
```

### popState
```cpp
void popState();
```
**Beskrivning:** Tar bort översta state från stacken.

### changeState
```cpp
void changeState(std::unique_ptr<IState> state);
```
**Beskrivning:** Byter ut nuvarande state.

**Parametrar:**
- `state` - Ny state att byta till

### getRenderer
```cpp
SDL_Renderer* getRenderer() const;
```
**Beskrivning:** Hämtar SDL renderer.

**Returnerar:** Pekare till SDL_Renderer

### screenToGame
```cpp
void screenToGame(int screenX, int screenY, int& gameX, int& gameY) const;
```
**Beskrivning:** Konverterar skärmkoordinater till spelkoordinater (hanterar letterboxing).

**Parametrar:**
- `screenX` - X-koordinat på skärmen
- `screenY` - Y-koordinat på skärmen
- `gameX` - Output: X-koordinat i spelet (640x400)
- `gameY` - Output: Y-koordinat i spelet

## Konstanter

```cpp
static constexpr int GAME_WIDTH = 640;
static constexpr int GAME_HEIGHT = 400;
```

Spelets logiska upplösning. SDL hanterar skalning automatiskt via `SDL_RenderSetLogicalSize`.

## Privata Medlemmar

```cpp
SDL_Window* m_window;
SDL_Renderer* m_renderer;
std::unique_ptr<StateManager> m_stateManager;
bool m_running;
Uint32 m_lastFrameTime;
SDL_Rect m_viewport;
float m_scale;
```

## Spelloop

```cpp
while (m_running) {
    handleEvents();  // SDL_PollEvent, input
    update(deltaTime); // Logik
    render();        // Rita allt
}
```

## Initiering

1. SDL_Init (VIDEO, AUDIO)
2. Skapa fönster och renderer
3. SDL_RenderSetLogicalSize (640x400)
4. Initiera managers (Texture, Audio, Font)
5. Ladda data (rooms, dialogs, quests)
6. Starta med MenuState

## Se även

- [StateManager](../states/StateManager.md)
- [IState](../states/IState.md)
- [TextureManager](../graphics/TextureManager.md)
