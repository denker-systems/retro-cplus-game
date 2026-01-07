---
description: Komplett debugging-process för Retro Engine
auto_execution_mode: 1
---

# Debug Workflow

> Systematisk felsökning för SDL2/Box2D game engine

## 1. Identifiera Problemtyp

### Problemklassificering

| Symptom | Typ | Primärt Verktyg | Sekundärt Verktyg |
|---------|-----|-----------------|-------------------|
| Kompilerar inte | Build Error | Compiler output | CMakeLists.txt |
| Linkar inte | Linker Error | Linker output | target_link_libraries |
| Kraschar vid start | Init Error | Debugger | SDL_GetError() |
| Kraschar under spel | Runtime Error | Stack trace | Logging |
| Fel beteende | Logic Error | Debugger + Logging | Unit tests |
| Långsamt/stutter | Performance | Profiler | Timing code |
| Minnes-problem | Memory Error | Address Sanitizer | Valgrind |
| Visuellt fel | Render Error | RenderDoc | Logging |
| Ljud-problem | Audio Error | Mix_GetError() | Logging |

### Prioriteringsordning
1. 🔴 **Crash** - Högsta prioritet
2. 🟠 **Data loss** - Hög prioritet (t.ex. sparning fungerar ej)
3. 🟡 **Broken feature** - Medium prioritet
4. 🟢 **Minor issue** - Låg prioritet

---

## 2. Build Errors

### Hitta Errors
```powershell
cmake --build . --config Debug 2>&1 | Select-String "error"
```

### Vanliga Compiler Errors

| Error | Orsak | Lösning |
|-------|-------|---------|
| `error C2065: undeclared identifier` | Variable/typ inte deklarerad | Inkludera rätt header |
| `error C2039: is not a member` | Fel klassnamn eller metod | Kontrollera stavning |
| `error C2143: syntax error` | Syntaxfel (saknar ;, }, etc) | Kolla föregående rad |
| `error C2511: overloaded member not found` | Signatur matchar inte | Jämför .h och .cpp |
| `error C4430: missing type specifier` | Saknar forward declaration | Lägg till forward decl eller include |

### Vanliga Linker Errors

| Error | Orsak | Lösning |
|-------|-------|---------|
| `LNK2001: unresolved external symbol` | Implementation saknas | Lägg till .cpp implementation |
| `LNK2019: unresolved external symbol` | Library inte länkad | Lägg till i target_link_libraries |
| `LNK2005: already defined` | Multiple definitions | Använd #pragma once |
| `LNK1104: cannot open file` | Library/obj saknas | Kontrollera build path |

### Felsökning av Linker Errors
```powershell
# Se vilka symbols som saknas
cmake --build . --config Debug 2>&1 | Select-String "unresolved external"

# Vanliga orsaker:
# 1. Ny .cpp fil ej tillagd i CMakeLists.txt
# 2. Virtuell metod deklarerad men ej implementerad
# 3. Library saknas i target_link_libraries
```

---

## 3. Runtime Crashes

### Bygg Debug Version
```powershell
cd build
cmake --build . --config Debug
.\Debug\RetroGame.exe
```

### Få Stack Trace i Visual Studio
1. Öppna projektet i Visual Studio (`.sln` filen i build/)
2. Sätt startup project till RetroGame eller RetroEditor
3. Tryck F5 för att starta debugging
4. När crash händer, inspektera Call Stack window

### Vanliga Crash-orsaker

| Crash | Typisk Orsak | Lösning |
|-------|--------------|---------|
| Access violation | Null pointer dereference | Lägg till null-check |
| Stack overflow | Oändlig rekursion | Kontrollera rekursiva anrop |
| Heap corruption | Buffer overflow | Använd Address Sanitizer |
| Pure virtual call | Anrop i constructor/destructor | Undvik virtuella anrop i ctor/dtor |

### Null Pointer Debugging
```cpp
// DÅLIGT - Kraschar om actor är nullptr
actor->update(dt);

// BRA - Kontrollera först
if (actor) {
    actor->update(dt);
} else {
    std::cerr << "[ERROR] Actor is null!" << std::endl;
}

// BRA - Med assertion (debug only)
assert(actor != nullptr && "Actor must not be null");
actor->update(dt);
```

---

## 4. SDL2 Debugging

### SDL2 Error Checking Pattern
```cpp
// SDL Init
if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "[SDL] Init failed: " << SDL_GetError() << std::endl;
    return false;
}

// Window creation
m_window = SDL_CreateWindow("Retro Game", 
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    640, 400, SDL_WINDOW_SHOWN);
if (!m_window) {
    std::cerr << "[SDL] Window creation failed: " << SDL_GetError() << std::endl;
    return false;
}

// Renderer creation
m_renderer = SDL_CreateRenderer(m_window, -1, 
    SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
if (!m_renderer) {
    std::cerr << "[SDL] Renderer creation failed: " << SDL_GetError() << std::endl;
    return false;
}
```

### SDL2_image Error Checking
```cpp
// Load texture
SDL_Texture* texture = IMG_LoadTexture(m_renderer, path.c_str());
if (!texture) {
    std::cerr << "[IMG] Failed to load texture '" << path << "': " 
              << IMG_GetError() << std::endl;
    return nullptr;
}
```

### SDL2_mixer Error Checking
```cpp
// Open audio
if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "[Mix] Failed to open audio: " << Mix_GetError() << std::endl;
    return false;
}

// Load sound
Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
if (!sound) {
    std::cerr << "[Mix] Failed to load sound '" << path << "': " 
              << Mix_GetError() << std::endl;
    return nullptr;
}

// Load music
Mix_Music* music = Mix_LoadMUS(path.c_str());
if (!music) {
    std::cerr << "[Mix] Failed to load music '" << path << "': " 
              << Mix_GetError() << std::endl;
    return nullptr;
}
```

---

## 5. Logging

### Logging Levels
```cpp
// Quick debug (ta bort innan commit!)
std::cout << "[DEBUG] value = " << value << std::endl;

// Med fil och rad
std::cout << "[" << __FILE__ << ":" << __LINE__ << "] "
          << "value = " << value << std::endl;

// Med funktion
std::cout << "[" << __FUNCTION__ << "] value = " << value << std::endl;

// Structured logging
std::cout << "[DEBUG][Actor] Position: (" 
          << m_position.x << ", " << m_position.y << ")" << std::endl;
std::cout << "[ERROR][DataLoader] Failed to load: " << path << std::endl;
std::cout << "[WARN][Physics] Body has zero mass" << std::endl;
```

### Targeted Debugging
```cpp
// Lägg till runt misstänkt kod
void Actor::update(float dt) {
    std::cout << "[Actor::update] ENTER - dt=" << dt << std::endl;
    
    std::cout << "[Actor::update] Position before: " 
              << m_position.x << ", " << m_position.y << std::endl;
    
    // ... suspected buggy code ...
    
    std::cout << "[Actor::update] Position after: " 
              << m_position.x << ", " << m_position.y << std::endl;
    
    std::cout << "[Actor::update] EXIT" << std::endl;
}
```

### Conditional Logging
```cpp
// Endast logga om villkor uppfylls
if (m_position.x < 0 || m_position.x > 640) {
    std::cout << "[WARN] Actor outside bounds: x=" << m_position.x << std::endl;
}

// Debug flag
#ifdef DEBUG_ACTORS
    std::cout << "[Actor] " << m_name << " updated" << std::endl;
#endif
```

---

## 6. Visual Studio Debugger

### Setup
```powershell
cd build
cmake --build . --config Debug
# Öppna retro-cplus-game.sln i Visual Studio
```

### Kortkommandon

| Tangent | Funktion |
|---------|----------|
| **F5** | Start/Continue debugging |
| **Shift+F5** | Stop debugging |
| **F9** | Toggle breakpoint |
| **Ctrl+F9** | Disable breakpoint |
| **F10** | Step over |
| **F11** | Step into |
| **Shift+F11** | Step out |
| **Ctrl+Shift+F10** | Set next statement |

### Breakpoints
- **Conditional breakpoint:** Högerklicka → Conditions → Sätt villkor
- **Hit count:** Högerklicka → Conditions → Hit Count
- **Data breakpoint:** Debug → New Breakpoint → Data Breakpoint

### Watch Variables
1. Debug → Windows → Watch → Watch 1
2. Skriv variabelnamn
3. Expandera structs/classes

### Call Stack
1. Debug → Windows → Call Stack
2. Dubbelklicka för att navigera till funktion
3. Granska lokala variabler i varje frame

---

## 7. Memory Debugging

### Address Sanitizer (ASan)

```cmake
# CMakeLists.txt
if(ENABLE_ASAN)
    target_compile_options(${TARGET} PRIVATE 
        /fsanitize=address  # MSVC
        # -fsanitize=address  # GCC/Clang
    )
endif()
```

### Build med ASan
```powershell
cmake .. -DENABLE_ASAN=ON
cmake --build . --config Debug
.\Debug\RetroGame.exe
# ASan kommer rapportera memory issues
```

### Vanliga Memory Bugs

| Problem | Symptom | Lösning |
|---------|---------|---------|
| Use after free | Crash/korruption | Använd smart pointers |
| Double free | Crash | Använd smart pointers |
| Buffer overflow | Korruption | Kontrollera bounds |
| Memory leak | RAM ökar | Använd RAII/smart pointers |
| Uninitialized variable | Undefined behavior | Initiera alla variabler |

### Smart Pointer Pattern
```cpp
// DÅLIGT - Risk för leaks och double free
Actor* actor = new Actor();
// ...
delete actor;
actor->update(dt);  // Use after free!

// BRA - RAII med unique_ptr
auto actor = std::make_unique<Actor>("Player");
scene->addActor(std::move(actor));
// Automatiskt cleaned up
```

---

## 8. Retro Engine-specifika Problem

### Vanliga Retro Engine Bugs

| Problem | Orsak | Lösning |
|---------|-------|---------|
| Svart fönster | SDL_RenderPresent saknas | Kolla render loop |
| Ingen input | SDL_PollEvent saknas | Kolla event loop |
| Texture saknas | Fel sökväg | Kolla `assets/` folder |
| Inget ljud | Mix_OpenAudio misslyckades | Kolla audio init |
| Walk area funkar ej | JSON parsning fel | Kolla DataLoader |
| Actor uppdateras ej | Ej tillagd i Scene | Kolla scene->addActor() |
| Component fungerar ej | getComponent returnerar nullptr | Kolla addComponent |
| Scene visas ej | Scene ej satt aktiv | Kolla level->setActiveScene() |
| Hotspot klick fungerar ej | Fel koordinater | World space vs screen space |
| Sparning misslyckas | Sync före save saknas | Kolla save flow |

### Data Loading Debugging
```cpp
// JSON loading med error handling
void DataLoader::loadRoom(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[DataLoader] Cannot open: " << path << std::endl;
        return;
    }
    
    try {
        nlohmann::json json = nlohmann::json::parse(file);
        
        // Debug: Print loaded data
        std::cout << "[DataLoader] Loaded: " << json.dump(2) << std::endl;
        
        // Kontrollera required fields
        if (!json.contains("id")) {
            std::cerr << "[DataLoader] Missing 'id' field" << std::endl;
            return;
        }
        
        // ... parse data ...
        
    } catch (const nlohmann::json::exception& e) {
        std::cerr << "[DataLoader] JSON error: " << e.what() << std::endl;
    }
}
```

### Actor/Component Debugging
```cpp
// Debugging actor components
void ActorObjectExtended::debugComponents() {
    std::cout << "[Actor] " << getName() << " components:" << std::endl;
    for (auto& comp : m_components) {
        std::cout << "  - " << typeid(*comp).name() << std::endl;
    }
}

// Debugging scene actors
void Scene::debugActors() {
    std::cout << "[Scene] " << getName() << " actors:" << std::endl;
    for (auto& actor : m_actors) {
        std::cout << "  - " << actor->getName() 
                  << " at (" << actor->getPosition().x 
                  << ", " << actor->getPosition().y << ")" << std::endl;
    }
}
```

---

## 9. Performance Debugging

### Basic Timing
```cpp
#include <chrono>

auto start = std::chrono::high_resolution_clock::now();

// Code to measure
scene->update(dt);

auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
std::cout << "[PERF] Scene::update took " << duration.count() << " µs" << std::endl;
```

### Frame Time Monitoring
```cpp
// In game loop
static float frameTimeAccum = 0;
static int frameCount = 0;

frameTimeAccum += dt;
frameCount++;

if (frameTimeAccum >= 1.0f) {
    float avgFrameTime = frameTimeAccum / frameCount;
    float fps = frameCount / frameTimeAccum;
    std::cout << "[PERF] FPS: " << fps << " (avg frame: " 
              << avgFrameTime * 1000.0f << " ms)" << std::endl;
    frameTimeAccum = 0;
    frameCount = 0;
}
```

---

## 10. Debug Checklista

### Innan Du Börjar
- [ ] Kan du reproducera buggen konsekvent?
- [ ] Vet du exakta steg för att trigga buggen?
- [ ] Har du kollat senaste commits (kan vara regression)?

### Under Debugging
- [ ] Läs error messages noggrant
- [ ] Kolla SDL_GetError() / IMG_GetError() / Mix_GetError()
- [ ] Lägg till targeted logging
- [ ] Använd debugger breakpoints
- [ ] Kontrollera variabelvärden
- [ ] Verifiera antaganden med assertions

### Efter Fix
- [ ] Verifiera att buggen är fixad
- [ ] Kontrollera att inget annat gått sönder
- [ ] Ta bort debug logging
- [ ] Dokumentera root cause och fix
- [ ] Överväg att lägga till test för att förhindra regression

---

## Quick Reference

### Debug Build och Kör
```powershell
cd build
cmake --build . --config Debug
.\Debug\RetroGame.exe
```

### Hitta Errors
```powershell
cmake --build . 2>&1 | Select-String "error"
```

### Common Debug Patterns
```cpp
// Null check
if (!ptr) { std::cerr << "ptr is null!" << std::endl; return; }

// Bounds check
if (index >= vec.size()) { std::cerr << "Index out of bounds!" << std::endl; return; }

// Value logging
std::cout << "[DEBUG] value=" << value << std::endl;
```
