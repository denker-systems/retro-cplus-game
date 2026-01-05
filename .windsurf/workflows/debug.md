---
description: Debugging workflow för SDL2-spelet
---

# Debug Workflow

## 1. Kompileringsfel
`powershell
cd build
cmake --build . --config Debug 2>&1 | Select-String "error"
`

## 2. Runtime-fel

### SDL fel
`cpp
if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
}
`

### Textur-fel
`cpp
if (!texture) {
    std::cerr << "IMG Error: " << IMG_GetError() << std::endl;
}
`

### Ljud-fel
`cpp
if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "Mix Error: " << Mix_GetError() << std::endl;
}
`

## 3. Debug output
`cpp
#include <iostream>
std::cout << "[DEBUG] Variable: " << value << std::endl;
`

## 4. Vanliga problem

| Problem | Orsak | Lösning |
|---------|-------|---------|
| Svart fönster | Ingen render | Kolla SDL_RenderPresent() |
| Ingen input | Event loop fel | Kolla SDL_PollEvent() |
| Krasch vid start | nullptr | Kolla init() returvärden |
| Textur visas inte | Fel sökväg | Kolla assets/-mappen i build |
| Inget ljud | Mixer ej init | Kolla Mix_OpenAudio() |

## 5. Visual Studio Debugger
`powershell
cd build
cmake --build . --config Debug
# Öppna RetroAdventure.sln i Visual Studio
# Sätt breakpoints och kör F5
`
