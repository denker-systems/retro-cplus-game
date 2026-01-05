---
trigger: always_on
description: SDL2 game development patterns and best practices
---

# SDL2 Guidelines

<initialization>
- Always check SDL_Init return value
- Initialize subsystems: SDL_INIT_VIDEO | SDL_INIT_AUDIO
- SDL_image: IMG_Init with required formats
- SDL_mixer: Mix_OpenAudio with appropriate settings
</initialization>

<game_loop>
Standard game loop pattern:
1. handleEvents() - SDL_PollEvent
2. update(deltaTime) - Game logic
3. render() - Clear, draw, present
</game_loop>

<rendering>
- Use SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
- SDL_RenderClear before drawing
- SDL_RenderPresent after all draws
- Textures loaded via IMG_LoadTexture
</rendering>

<cleanup>
Order matters:
1. Destroy textures
2. Destroy renderer
3. Destroy window
4. Mix_Quit, IMG_Quit, SDL_Quit
</cleanup>

<delta_time>
Calculate frame-independent movement:
`cpp
Uint32 current = SDL_GetTicks();
float deltaTime = (current - lastTime) / 1000.0f;
lastTime = current;
position += velocity * deltaTime;
`
</delta_time>
