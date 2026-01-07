---
description: Investigate and fix bugs systematically
---

# Investigate Workflow

> Systematisk felsökning

## 1. Check Build Errors
```powershell
cd build; cmake --build . --config Release 2>&1
```

## 2. Identify Problem Type

| Symptom | Likely Cause |
|---------|--------------|
| Compile error | Syntax, missing include |
| Linker error | Missing library, undefined symbol |
| Crash on start | nullptr, missing asset |
| Runtime bug | Logic error |

---

## 3. Debug Strategies

### Add Logging
```cpp
#include <iostream>
std::cout << "[DEBUG] value = " << value << std::endl;
```

### Check Return Values
```cpp
if (result == nullptr) {
    std::cerr << "Error: " << SDL_GetError() << std::endl;
}
```

### Isolate the Problem
- Comment out sections of code
- Binary search for problematic code
- Check git diff for recent changes

---

## 4. Common Fixes

| Problem | Solution |
|---------|----------|
| DLL missing | Copy SDL2.dll to exe folder |
| Texture not loading | Check assets/ path |
| Black screen | Check SDL_RenderPresent() |
| No input | Check SDL_PollEvent() loop |
| Crash on exit | Check cleanup order |

---

## 5. When Stuck

1. Check git log for recent changes
2. Search error message online
3. Check SDL2 documentation
4. Ask for help with full error message
