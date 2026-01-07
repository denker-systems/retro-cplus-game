---
description: Debugging workflow for SDL2 game
---

# Debug Workflow

> Systematisk felsökning

## 1. Check Build Errors
```powershell
cd build; cmake --build . --config Debug 2>&1 | Select-String "error"
```

## 2. Common SDL2 Errors

### SDL Init Failed
```cpp
if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
}
```

### Texture Load Failed
```cpp
if (!texture) {
    std::cerr << "IMG Error: " << IMG_GetError() << std::endl;
}
```

### Audio Init Failed
```cpp
if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "Mix Error: " << Mix_GetError() << std::endl;
}
```

---

## 3. Debug Output
```cpp
#include <iostream>
std::cout << "[DEBUG] value = " << value << std::endl;
```

---

## 4. Common Problems

| Problem | Cause | Solution |
|---------|-------|----------|
| Black window | No render calls | Check SDL_RenderPresent() |
| No input | Event loop broken | Check SDL_PollEvent() |
| Crash on start | nullptr | Check init() return values |
| No texture | Wrong path | Check assets/ folder |
| No audio | Mixer not init | Check Mix_OpenAudio() |

---

## 5. Visual Studio Debugger
```powershell
cd build
cmake --build . --config Debug
# Open .sln in Visual Studio, set breakpoints, F5
```
