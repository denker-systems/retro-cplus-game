---
trigger: always_on
description: Extended C++ coding standards based on Google/ISO guidelines
---

# Extended Coding Standards

> Based on Google C++ Style Guide & ISO C++ Core Guidelines

## Naming (Google Style)

### Types - PascalCase
```cpp
class MyExcitingClass { };
struct UrlTableProperties { };
enum class UrlTableError { };
using PropertiesMap = std::map<std::string, int>;
```

### Variables - snake_case
```cpp
// Local variables
std::string table_name;      // ✓ snake_case
std::string tableName;       // ✗ mixed case

// Class members - trailing underscore
class TableInfo {
private:
    std::string table_name_;  // ✓
    static Pool* pool_;       // ✓
};

// Struct members - no underscore
struct UrlTableProperties {
    std::string name;         // ✓
    int num_entries;          // ✓
};
```

### Constants - kPascalCase
```cpp
const int kDaysInAWeek = 7;
constexpr int kMaxPlayers = 4;
static const int kTableVersion = 3;
```

### Functions - PascalCase
```cpp
void AddTableEntry();
void DeleteUrl();
void OpenFileOrDie();

// Accessors/mutators can be snake_case
int count() const;
void set_count(int count);
```

---

## Smart Pointers (ISO Guidelines)

### R.20: Use unique_ptr for exclusive ownership
```cpp
// Prefer
std::unique_ptr<Foo> CreateFoo();
void ConsumeFoo(std::unique_ptr<Foo> ptr);

// Avoid
Foo* CreateFoo();  // Who owns this?
```

### R.21: Use shared_ptr only when sharing
```cpp
// Only when multiple owners needed
std::shared_ptr<const Texture> m_sharedTexture;

// Never use auto_ptr
std::auto_ptr<Foo> p;  // ❌ Deprecated
std::unique_ptr<Foo> p; // ✓
```

### F.7: For general use, take T* or T&
```cpp
// Don't force ownership model on callers
void process(Actor* actor);           // ✓ Non-owning
void process(Actor& actor);           // ✓ Non-owning, non-null

// Not this
void process(std::unique_ptr<Actor>); // ❌ Forces ownership transfer
```

---

## Functions (ISO Guidelines)

### F.2: A function should perform a single logical operation
```cpp
// Bad - does too much
void processAndSaveAndNotify();

// Good - single responsibility
void process();
void save();
void notify();
```

### F.3: Keep functions short and simple
```cpp
// Target: Functions that fit on one screen (~60 lines)
// Extract helper functions for complex logic
```

### F.4: constexpr for compile-time evaluation
```cpp
constexpr int square(int n) { return n * n; }
constexpr int TILE_SIZE = 32;
constexpr int GRID_WIDTH = square(8);  // Computed at compile time
```

### F.6: noexcept for functions that don't throw
```cpp
void swap(Actor& a, Actor& b) noexcept;
~Actor() noexcept;                      // Destructors should be noexcept
Actor(Actor&&) noexcept;                // Move operations should be noexcept
```

---

## Classes (ISO Guidelines)

### C.2: Use class if invariant; struct if data varies independently
```cpp
// Class - has invariants (private data)
class Actor {
    Vec2 m_position;  // Invariant: always valid
public:
    void setPosition(Vec2 pos);
};

// Struct - plain data
struct Vec2 {
    float x, y;       // Can vary independently
};
```

### C.21: Rule of Zero/Five
```cpp
// Rule of Zero - Let compiler generate
class SimpleClass {
    std::string m_name;
    std::vector<int> m_data;
    // No destructor, copy, move needed!
};

// Rule of Five - If you define one, define all
class ResourceOwner {
public:
    ~ResourceOwner();
    ResourceOwner(const ResourceOwner&);
    ResourceOwner& operator=(const ResourceOwner&);
    ResourceOwner(ResourceOwner&&) noexcept;
    ResourceOwner& operator=(ResourceOwner&&) noexcept;
};
```

### C.35: Virtual destructor for base classes
```cpp
class Base {
public:
    virtual ~Base() = default;  // ✓ Allows proper cleanup
};

// Or protected non-virtual if not deleted through base
class Base {
protected:
    ~Base() = default;          // ✓ Can't delete through base pointer
};
```

### C.128: Virtual functions: exactly one of virtual, override, final
```cpp
class Base {
    virtual void draw();        // Base declares virtual
};

class Derived : public Base {
    void draw() override;       // ✓ Use override
    // void draw() virtual;     // ❌ Don't repeat virtual
};

class Final : public Derived {
    void draw() final;          // ✓ Use final if no more overriding
};
```

---

## Error Handling (ISO Guidelines)

### E.6: Use RAII to prevent leaks
```cpp
// Bad
void process() {
    FILE* f = fopen("file.txt", "r");
    // ... what if exception here?
    fclose(f);  // May never execute
}

// Good
void process() {
    std::ifstream f("file.txt");
    // ... RAII ensures cleanup
}  // f automatically closed
```

### E.25: Simulate RAII if exceptions disabled
```cpp
class ScopedLock {
    Mutex& m_mutex;
public:
    ScopedLock(Mutex& m) : m_mutex(m) { m_mutex.lock(); }
    ~ScopedLock() { m_mutex.unlock(); }
};
```

---

## Performance (ISO Guidelines)

### P.9: Don't waste time or space
```cpp
// Avoid unnecessary copies
void process(const std::vector<int>& data);  // ✓ Reference
void process(std::vector<int> data);         // ❌ Copy

// But for small types, copy is fine
void process(int value);                     // ✓ Copy is cheap
void process(Vec2 pos);                      // ✓ Small struct, copy OK
```

### P.10: Prefer immutable data
```cpp
// Prefer const
const int MAX_PLAYERS = 4;
const std::string& getName() const;

// Mark parameters const when not modified
void process(const Actor& actor);
```

---

## Concurrency (ISO Guidelines)

### CP.20: Use RAII for locks
```cpp
// Good
{
    std::lock_guard<std::mutex> lock(m_mutex);
    // Protected code
}  // Automatically unlocked

// Bad
m_mutex.lock();
// What if exception?
m_mutex.unlock();
```

### CP.44: Name your lock_guards
```cpp
std::lock_guard<std::mutex> lock(m_mutex);  // ✓ Named
std::lock_guard<std::mutex>(m_mutex);       // ❌ Anonymous, unlocks immediately!
```

---

## Include Order (Google Style)

```cpp
// 1. Related header
#include "MyClass.h"

// 2. C system headers
#include <cstdlib>

// 3. C++ standard library
#include <memory>
#include <string>
#include <vector>

// 4. Other libraries
#include <SDL.h>
#include <nlohmann/json.hpp>

// 5. Project headers
#include "engine/core/Actor.h"
```

---

## Comments (ISO Guidelines)

### NL.1: Don't say in comments what can be stated in code
```cpp
// Bad
int x = 5;  // Set x to 5

// Good
int player_health = 5;  // Self-documenting name
```

### NL.2: State intent in comments
```cpp
// Bad - describes what
i++;  // Increment i

// Good - describes why
i++;  // Advance to next animation frame
```
