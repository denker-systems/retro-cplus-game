---
trigger: always_on
description: Performance guidelines
---

# Performance Guidelines

> Optimization best practices

## Performance Targets

| Metric | Target |
|--------|--------|
| Frame time | < 16.6ms (60 FPS) |
| Load time | < 3 seconds |
| Memory | < 500 MB |

---

## Hot Path Rules

```cpp
// [HOT PATH] - Called every frame
void update(float dt) {
    // Avoid allocations
    // Minimize branching
    // Cache-friendly access
}
```

---

## Cache-Friendly Data

```cpp
// Good - contiguous memory
std::vector<Transform> m_transforms;

// Bad - pointer chasing
std::vector<Actor*> m_actors;
```

---

## Avoid in Hot Paths

- ❌ Memory allocation
- ❌ `dynamic_cast`
- ❌ Virtual function calls (when possible)
- ❌ String operations
- ❌ File I/O

---

## Optimization Checklist

- [ ] Profile before optimizing
- [ ] Optimize hot paths first
- [ ] Measure after changes
- [ ] Document optimizations
