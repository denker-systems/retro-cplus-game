---
trigger: always_on
description: SDL2 game development patterns
---

# SDL2 Patterns

> Best practices for SDL2 game development

## Initialization

```cpp
// Initialize subsystems
if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
    return false;
}

// Image loading
if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    std::cerr << "IMG Error: " << IMG_GetError() << std::endl;
}

// Audio
if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "Mix Error: " << Mix_GetError() << std::endl;
}
```

---

## Game Loop

```cpp
while (m_running) {
    // 1. Delta time
    Uint32 current = SDL_GetTicks();
    float dt = (current - lastTime) / 1000.0f;
    lastTime = current;
    
    // 2. Events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        handleEvent(event);
    }
    
    // 3. Update
    update(dt);
    
    // 4. Render
    SDL_RenderClear(m_renderer);
    render();
    SDL_RenderPresent(m_renderer);
}
```

---

## Cleanup Order

```cpp
// Order matters!
SDL_DestroyTexture(texture);   // 1. Textures
SDL_DestroyRenderer(renderer); // 2. Renderer
SDL_DestroyWindow(window);     // 3. Window
Mix_Quit();                    // 4. Mixer
IMG_Quit();                    // 5. Image
SDL_Quit();                    // 6. SDL
```

---

## RAII Wrappers

```cpp
using SDLWindowPtr = std::unique_ptr<
    SDL_Window, 
    decltype(&SDL_DestroyWindow)
>;

using SDLRendererPtr = std::unique_ptr<
    SDL_Renderer, 
    decltype(&SDL_DestroyRenderer)
>;
```

---

## Error Checking

```cpp
// Always check returns
if (!texture) {
    LOG_ERROR("Failed to load: {}", IMG_GetError());
}
```
