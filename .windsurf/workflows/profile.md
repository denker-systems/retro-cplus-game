---
description: Performance profiling workflow
---

# Profile Workflow

> Performance analysis and optimization

## When to Profile

- Frame rate drops
- Long load times
- Memory growth
- Before release
- After major changes

---

## 1. Identify Target

### Performance Goals
| Metric | Target | Current |
|--------|--------|---------|
| Frame time | < 16.6ms (60 FPS) | ? |
| Load time | < 3s | ? |
| Memory | < 500 MB | ? |

### Measure Current
```cpp
auto start = std::chrono::high_resolution_clock::now();
// Code to measure
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
std::cout << "Duration: " << duration.count() << "µs" << std::endl;
```

---

## 2. CPU Profiling

### Visual Studio Profiler
```powershell
# 1. Build Release with debug info
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . --config RelWithDebInfo

# 2. In Visual Studio:
#    Debug > Performance Profiler > CPU Usage
```

### Tracy Profiler (if integrated)
```cpp
#include <tracy/Tracy.hpp>

void update() {
    ZoneScoped;  // Mark function for profiling
    // Code
}
```

### Manual Instrumentation
```cpp
class ScopedTimer {
    const char* m_name;
    std::chrono::high_resolution_clock::time_point m_start;
public:
    ScopedTimer(const char* name) : m_name(name), 
        m_start(std::chrono::high_resolution_clock::now()) {}
    ~ScopedTimer() {
        auto duration = std::chrono::high_resolution_clock::now() - m_start;
        auto ms = std::chrono::duration_cast<std::chrono::microseconds>(duration);
        LOG_DEBUG("{}: {}µs", m_name, ms.count());
    }
};

#define PROFILE_SCOPE(name) ScopedTimer _timer##__LINE__(name)

void update() {
    PROFILE_SCOPE("Update");
    // Code
}
```

---

## 3. Memory Profiling

### Track Allocations
```cpp
// Override new/delete for tracking
void* operator new(size_t size) {
    g_totalAllocated += size;
    return malloc(size);
}
```

### Visual Studio Memory Profiler
```powershell
# Debug > Performance Profiler > Memory Usage
```

### Check for Leaks
```cpp
// Address Sanitizer
cmake .. -DENABLE_ASAN=ON
```

---

## 4. GPU Profiling

### SDL2 Rendering Stats
```cpp
SDL_RendererInfo info;
SDL_GetRendererInfo(renderer, &info);
std::cout << "Renderer: " << info.name << std::endl;
```

### Draw Call Count
```cpp
// Track draw calls per frame
static int s_drawCalls = 0;
void drawSprite(...) {
    s_drawCalls++;
    SDL_RenderCopy(...);
}
void endFrame() {
    LOG_DEBUG("Draw calls: {}", s_drawCalls);
    s_drawCalls = 0;
}
```

---

## 5. Optimization Strategies

### CPU Optimizations
| Issue | Solution |
|-------|----------|
| Hot function | Optimize algorithm |
| Cache misses | Improve data layout |
| Branching | Remove conditionals |
| Allocations | Pool/reuse objects |

### Memory Optimizations
| Issue | Solution |
|-------|----------|
| Memory leaks | Fix ownership |
| Fragmentation | Use pools |
| Large allocations | Stream/lazy load |

### GPU Optimizations
| Issue | Solution |
|-------|----------|
| Too many draw calls | Batch sprites |
| Large textures | Texture atlas |
| Overdraw | Sort by depth |

---

## 6. Benchmark

### Create Benchmark
```cpp
void benchmarkPhysics() {
    const int ITERATIONS = 1000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < ITERATIONS; i++) {
        physicsSystem.update(0.016f);
    }
    
    auto duration = std::chrono::high_resolution_clock::now() - start;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    
    std::cout << "Physics: " << ms.count() / ITERATIONS << "ms per update" << std::endl;
}
```

### Track Over Time
```markdown
## Performance Log

| Date | Build | FPS | Memory | Notes |
|------|-------|-----|--------|-------|
| 2024-01-01 | abc123 | 60 | 200MB | Baseline |
| 2024-01-15 | def456 | 55 | 250MB | Added physics |
```

---

## 7. Document Results

```markdown
## Profile Report: [Date]

### Summary
- Target: 60 FPS (16.6ms)
- Actual: 55 FPS (18.2ms)
- Bottleneck: Physics update

### Findings
1. Physics::update() - 8ms (44%)
2. Render::draw() - 5ms (27%)
3. Other - 5ms (27%)

### Recommendations
1. Optimize collision detection
2. Implement spatial partitioning
3. Reduce physics iterations

### Action Items
- [ ] Implement quad tree (#XXX)
- [ ] Profile after changes
```

---

## Checklist

- [ ] Performance goals defined
- [ ] Baseline measured
- [ ] Profiler run
- [ ] Bottleneck identified
- [ ] Optimization implemented
- [ ] Improvement verified
- [ ] No regressions
- [ ] Results documented
