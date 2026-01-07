---
trigger: always_on
description: Component-based architecture pattern
---

# Component Pattern

> Composition over inheritance

## Core Principle

```cpp
// Prefer composition
auto* sprite = actor->addComponent<SpriteComponent>();
auto* physics = actor->addComponent<RigidBody2DComponent>();

// Over deep inheritance
class FlyingEnemyWithShield : public FlyingEnemy { }  // Bad
```

---

## Component Base

```cpp
class ActorComponent {
public:
    virtual ~ActorComponent() = default;
    
    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void update(float dt) {}
    virtual void render(SDL_Renderer* r) {}
    
    ActorObject* getOwner() const { return m_owner; }
    
protected:
    ActorObject* m_owner = nullptr;
};
```

---

## Adding Components

```cpp
template<typename T>
T* ActorObjectExtended::addComponent() {
    auto component = std::make_unique<T>();
    T* ptr = component.get();
    component->m_owner = this;
    component->onAttach();
    m_components.push_back(std::move(component));
    return ptr;
}
```

---

## Getting Components

```cpp
template<typename T>
T* ActorObjectExtended::getComponent() {
    for (auto& c : m_components) {
        if (auto* t = dynamic_cast<T*>(c.get())) {
            return t;
        }
    }
    return nullptr;
}
```

---

## Common Components

| Component | Purpose |
|-----------|---------|
| SpriteComponent | Rendering |
| AnimationComponent | Animation |
| RigidBody2DComponent | Physics |
| Collider2DComponent | Collision |
| AudioSourceComponent | Sound |
| AIControllerComponent | AI |

---

## Best Practices

- Keep components focused (single responsibility)
- Avoid component-to-component dependencies
- Use events for communication
- Cache component pointers when accessed frequently
