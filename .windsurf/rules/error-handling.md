---
trigger: always_on
description: Error handling strategies for C++ game development
---

# Error Handling

> Robust error management for game engines

## Strategy Selection

| Context | Strategy |
|---------|----------|
| Critical failure | Assert/crash |
| Recoverable error | Error codes |
| Exceptional case | Exceptions (if enabled) |
| Optional value | std::optional |
| Expected failure | std::expected (C++23) |

---

## Assertions

### Debug Assertions
```cpp
// Only in debug builds
assert(ptr != nullptr);
assert(index < size);

// Custom macro with message
#define ENGINE_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            LOG_FATAL("Assertion failed: {} - {}", #cond, msg); \
            std::abort(); \
        } \
    } while(0)

ENGINE_ASSERT(actor != nullptr, "Actor must exist");
```

### Release Assertions
```cpp
// Critical checks that stay in release
#define ENGINE_VERIFY(cond) \
    do { \
        if (!(cond)) { \
            LOG_FATAL("Verification failed: {}", #cond); \
            std::abort(); \
        } \
    } while(0)

ENGINE_VERIFY(SDL_Init(SDL_INIT_VIDEO) == 0);
```

---

## Error Codes

### Result Type
```cpp
enum class Result {
    Success,
    FileNotFound,
    InvalidFormat,
    OutOfMemory,
    Unknown
};

Result loadTexture(const std::string& path, Texture& out);

// Usage
Texture tex;
if (loadTexture("player.png", tex) != Result::Success) {
    LOG_ERROR("Failed to load texture");
    return false;
}
```

### Error Info Struct
```cpp
struct Error {
    int code;
    std::string message;
    std::string file;
    int line;
    
    static Error None() { return {0, "", "", 0}; }
    bool isError() const { return code != 0; }
};
```

---

## std::optional

```cpp
// For "may not exist" scenarios
std::optional<Actor*> findActor(const std::string& name);

// Usage
if (auto actor = findActor("player")) {
    actor.value()->update(dt);
}

// Or with value_or
auto health = getHealth().value_or(100);
```

---

## RAII for Resources

### SDL Resource Wrappers
```cpp
// Texture wrapper
class SDLTexture {
    SDL_Texture* m_texture = nullptr;
public:
    SDLTexture(SDL_Renderer* r, const char* path) {
        m_texture = IMG_LoadTexture(r, path);
    }
    ~SDLTexture() {
        if (m_texture) SDL_DestroyTexture(m_texture);
    }
    
    // Non-copyable
    SDLTexture(const SDLTexture&) = delete;
    SDLTexture& operator=(const SDLTexture&) = delete;
    
    // Moveable
    SDLTexture(SDLTexture&& other) noexcept 
        : m_texture(std::exchange(other.m_texture, nullptr)) {}
    
    SDL_Texture* get() const { return m_texture; }
    operator bool() const { return m_texture != nullptr; }
};
```

### Scope Guard
```cpp
template<typename F>
class ScopeGuard {
    F m_func;
    bool m_active = true;
public:
    ScopeGuard(F f) : m_func(std::move(f)) {}
    ~ScopeGuard() { if (m_active) m_func(); }
    void dismiss() { m_active = false; }
};

// Usage
void process() {
    auto guard = ScopeGuard([&]{ cleanup(); });
    
    // ... processing ...
    
    guard.dismiss();  // Success, don't cleanup
}
```

---

## Logging Errors

```cpp
// Log levels
LOG_TRACE("Detailed trace info");
LOG_DEBUG("Debug information");
LOG_INFO("Normal operation");
LOG_WARNING("Potential problem");
LOG_ERROR("Error occurred: {}", message);
LOG_FATAL("Critical failure");

// With context
LOG_ERROR("[{}:{}] Failed: {}", __FILE__, __LINE__, error);
```

---

## SDL2 Error Handling

```cpp
// Always check SDL returns
if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    LOG_FATAL("SDL_Init failed: {}", SDL_GetError());
    return false;
}

// IMG loading
SDL_Texture* tex = IMG_LoadTexture(renderer, path);
if (!tex) {
    LOG_ERROR("IMG_LoadTexture failed: {}", IMG_GetError());
    return nullptr;
}

// Mixer
if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    LOG_ERROR("Mix_OpenAudio failed: {}", Mix_GetError());
}
```

---

## Best Practices

### DO
- ✓ Check all return values
- ✓ Use RAII for all resources
- ✓ Log errors with context
- ✓ Fail fast on unrecoverable errors
- ✓ Provide meaningful error messages

### DON'T
- ❌ Ignore error codes
- ❌ Use exceptions across DLL boundaries
- ❌ Throw in destructors
- ❌ Catch and swallow errors silently
- ❌ Use error codes without checking
