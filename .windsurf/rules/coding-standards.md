---
trigger: always_on
description: C++ coding standards for game engine development
---

# Coding Standards

> Enterprise C++ standards for 50+ developer teams

## Naming Conventions

| Type | Convention | Example |
|------|------------|---------|
| Classes | PascalCase | `PlayerCharacter` |
| Interfaces | I prefix | `IRenderable` |
| Methods | camelCase | `getPosition()` |
| Member vars | m_ prefix | `m_position` |
| Static vars | s_ prefix | `s_instance` |
| Constants | SCREAMING_SNAKE | `MAX_HEALTH` |
| Namespaces | lowercase | `engine` |
| Files | PascalCase | `PlayerCharacter.cpp` |

---

## File Structure

### Header Files
```cpp
/**
 * @file ClassName.h
 * @brief Brief description
 */
#pragma once

// 1. Engine headers
#include "engine/core/Object.h"

// 2. Third-party
#include <SDL.h>

// 3. Standard library
#include <memory>
#include <string>

namespace engine {

class ClassName {
public:
    // Constructors/Destructor
    // Public methods
    
private:
    // Private methods
    // Member variables
};

} // namespace engine
```

---

## Class Design

### Rule of Five
```cpp
class Resource {
public:
    Resource();                              // Constructor
    ~Resource();                             // Destructor
    Resource(const Resource&);               // Copy constructor
    Resource& operator=(const Resource&);    // Copy assignment
    Resource(Resource&&) noexcept;           // Move constructor
    Resource& operator=(Resource&&) noexcept;// Move assignment
};
```

### Delete When Needed
```cpp
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
};
```

---

## Memory Management

### Smart Pointers
```cpp
// Ownership
std::unique_ptr<Actor> m_actor;

// Shared (rare)
std::shared_ptr<Texture> m_sharedTexture;

// Non-owning reference
Actor* getActor();  // Does not transfer ownership
```

### RAII
```cpp
// Wrap resources in classes
class SDLTexture {
    SDL_Texture* m_texture;
public:
    ~SDLTexture() { SDL_DestroyTexture(m_texture); }
};
```

---

## Modern C++

### Use auto Appropriately
```cpp
// Good - obvious type
auto it = map.find(key);
auto ptr = std::make_unique<Actor>();

// Bad - unclear type
auto x = getValue();  // What type?
```

### Range-based For
```cpp
// Prefer
for (const auto& item : container) { }

// Over
for (size_t i = 0; i < container.size(); i++) { }
```

### nullptr
```cpp
// Use nullptr, not NULL or 0
Actor* actor = nullptr;
```

---

## Error Handling

### Check Returns
```cpp
if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    LOG_ERROR("SDL Error: {}", SDL_GetError());
    return false;
}
```

### Exceptions
- Use sparingly in game code
- Prefer error codes for performance-critical paths
- Document thrown exceptions

---

## Const Correctness

```cpp
class Actor {
public:
    // Const method - doesn't modify state
    Vec2 getPosition() const { return m_position; }
    
    // Const reference - avoids copy
    const std::string& getName() const { return m_name; }
    
    // Const parameter
    void setName(const std::string& name);
};
```

---

## Threading

### Thread Safety Notes
```cpp
/**
 * @par Thread Safety
 * Not thread-safe. Call only from main thread.
 */
void update();

/**
 * @par Thread Safety
 * Thread-safe. Can be called from any thread.
 */
int getCount() const;
```

---

## Forbidden Practices

- ❌ Raw `new`/`delete` (use smart pointers)
- ❌ C-style casts (use `static_cast`, etc.)
- ❌ `using namespace std;` in headers
- ❌ Magic numbers (use named constants)
- ❌ Deep inheritance (max 4 levels)
- ❌ God classes
- ❌ Circular dependencies
