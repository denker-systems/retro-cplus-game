---
trigger: glob
globs: ["*.cpp", "*.h"]
description: C++ coding standards for game engine development
---

# C++ Guidelines

> Modern C++17 för game development

## Naming Conventions

| Typ | Format | Exempel |
|-----|--------|---------|
| Klasser | PascalCase | `PlayerCharacter` |
| Metoder | camelCase | `getPosition()` |
| Medlemsvariabler | m_ prefix | `m_position` |
| Statiska | s_ prefix | `s_instance` |
| Konstanter | SCREAMING_SNAKE | `MAX_HEALTH` |
| Namespaces | lowercase | `engine` |

---

## Headers

```cpp
#pragma once  // Always use pragma once

// Include order:
// 1. Corresponding header (for .cpp)
// 2. Engine headers
// 3. Third-party (SDL, etc.)
// 4. Standard library
```

---

## Classes

```cpp
class ClassName {
public:
    ClassName();                           // Constructor
    virtual ~ClassName();                  // Virtual destructor for base classes
    
    ClassName(const ClassName&) = delete;  // Delete copy if not needed
    ClassName& operator=(const ClassName&) = delete;

private:
    std::unique_ptr<Resource> m_resource;  // RAII ownership
};
```

---

## Memory Management

```cpp
// Ownership: unique_ptr
std::unique_ptr<Actor> m_actor;

// Shared (rare): shared_ptr
std::shared_ptr<Texture> m_sharedTexture;

// Non-owning reference: raw pointer
Actor* getActor();  // Does not transfer ownership
```

---

## Error Handling

```cpp
if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
    return false;
}
```

---

## Best Practices

- **const correctness** - Mark methods const when possible
- **override keyword** - Always use for virtual methods
- **Forward declarations** - In headers when possible
- **RAII** - Wrap resources in classes
