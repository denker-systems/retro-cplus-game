---
trigger: always_on
description: Memory safety guidelines for C++ game development
---

# Memory Safety

> Preventing memory bugs in game engines

## Ownership Model

### Clear Ownership Rules
```cpp
// 1. unique_ptr = exclusive ownership
std::unique_ptr<Actor> m_player;

// 2. shared_ptr = shared ownership (rare)
std::shared_ptr<Texture> m_sharedTexture;

// 3. Raw pointer = non-owning reference
Actor* getPlayer();  // Caller doesn't own

// 4. Reference = non-owning, non-null
void process(Actor& actor);
```

---

## Smart Pointer Guidelines

### Use make_unique/make_shared
```cpp
// Good - exception safe, single allocation
auto actor = std::make_unique<Actor>("Player");
auto tex = std::make_shared<Texture>();

// Bad - not exception safe
std::unique_ptr<Actor> actor(new Actor("Player"));
```

### Transfer Ownership Explicitly
```cpp
// Clear ownership transfer
void takeOwnership(std::unique_ptr<Actor> actor);
scene.addActor(std::move(actor));  // Explicit move

// Non-owning access
Actor* actor = scene.getActor("player");  // Borrow only
```

### Avoid shared_ptr for Resources
```cpp
// Bad - unclear lifetime
class Actor {
    std::shared_ptr<Texture> m_texture;  // Who owns this?
};

// Good - managed by TextureManager
class Actor {
    Texture* m_texture;  // Borrowed from TextureManager
};
```

---

## Null Safety

### Always Initialize Pointers
```cpp
Actor* m_target = nullptr;  // ✓ Explicit null
Actor* m_target;            // ❌ Uninitialized!
```

### Check Before Use
```cpp
if (m_target) {
    m_target->update(dt);
}

// Or use optional for "may not exist"
std::optional<Actor*> findActor(const std::string& name);
```

### Use References When Non-Null Required
```cpp
// Can never be null
void process(Actor& actor);

// May be null
void process(Actor* actor);
```

---

## Array Safety

### Prefer std::vector
```cpp
// Good
std::vector<Actor*> m_actors;

// Bad - manual memory management
Actor** m_actors;
int m_actorCount;
```

### Bounds Checking
```cpp
// Use .at() in debug for bounds checking
actor = m_actors.at(index);  // Throws if out of bounds

// Or check manually
if (index < m_actors.size()) {
    actor = m_actors[index];
}
```

### Use span for Array Parameters
```cpp
// Good - knows its size
void process(std::span<Actor*> actors);

// Bad - can't check bounds
void process(Actor** actors, size_t count);
```

---

## Resource Management

### RAII Wrappers
```cpp
// Wrap all SDL resources
class SDLWindow {
    SDL_Window* m_window = nullptr;
public:
    SDLWindow(const char* title, int w, int h) {
        m_window = SDL_CreateWindow(title, 
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            w, h, SDL_WINDOW_SHOWN);
    }
    ~SDLWindow() {
        if (m_window) SDL_DestroyWindow(m_window);
    }
    SDL_Window* get() const { return m_window; }
};
```

### Custom Deleters
```cpp
using SDLTexturePtr = std::unique_ptr<
    SDL_Texture, 
    decltype(&SDL_DestroyTexture)
>;

SDLTexturePtr texture(
    IMG_LoadTexture(renderer, path),
    SDL_DestroyTexture
);
```

---

## Common Memory Bugs

### Use After Free
```cpp
// Bug
Actor* actor = new Actor();
delete actor;
actor->update(dt);  // ❌ Use after free!

// Prevention: use unique_ptr
auto actor = std::make_unique<Actor>();
// Can't use after reset()
```

### Double Free
```cpp
// Bug
Actor* actor = new Actor();
delete actor;
delete actor;  // ❌ Double free!

// Prevention: unique_ptr handles this
```

### Memory Leak
```cpp
// Bug
void process() {
    Actor* actor = new Actor();
    if (error) return;  // ❌ Leak!
    delete actor;
}

// Prevention: RAII
void process() {
    auto actor = std::make_unique<Actor>();
    if (error) return;  // ✓ Cleaned up
}
```

### Dangling Pointer
```cpp
// Bug
Actor* getActor() {
    Actor actor;
    return &actor;  // ❌ Returns dangling pointer!
}

// Prevention: return by value or smart pointer
std::unique_ptr<Actor> createActor();
```

---

## Address Sanitizer

### Enable in Debug
```cmake
if(ENABLE_ASAN)
    target_compile_options(${TARGET} PRIVATE 
        -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(${TARGET} PRIVATE 
        -fsanitize=address)
endif()
```

### Run Tests with ASan
```powershell
cmake -DENABLE_ASAN=ON ..
cmake --build . --config Debug
ctest -C Debug
```
