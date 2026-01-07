---
trigger: always_on
description: OOP architecture and design patterns for game engine
---

# OOP Architecture

> Component-based design med skalbar hierarki

## Design Principles

### 1. Composition Over Inheritance
```cpp
// BRA: Komponenter för funktionalitet
auto* sprite = actor->addComponent<SpriteComponent>();
auto* physics = actor->addComponent<RigidBody2DComponent>();

// UNDVIK: Djupa arvshierarkier
class FlyingEnemyWithShield : public FlyingEnemy  // För djupt!
```

### 2. Single Responsibility
- En klass = ett ansvar
- Managers hanterar collections
- Components hanterar specifik funktionalitet

### 3. Interface Segregation
- Små, fokuserade interfaces
- `IRenderable`, `IUpdatable`, `ISerializable`

---

## Actor Hierarchy (max 4 nivåer)

```
Object
 ActorObject
     ActorObjectExtended
         CharacterActor
            PlayerActor
            NPCActor
         EnvironmentActor
         InteractiveActor
```

---

## Component System

```cpp
class ActorObjectExtended : public ActorObject {
public:
    template<typename T>
    T* addComponent() {
        auto component = std::make_unique<T>();
        T* ptr = component.get();
        m_components.push_back(std::move(component));
        return ptr;
    }
    
    template<typename T>
    T* getComponent() {
        for (auto& c : m_components) {
            if (auto* t = dynamic_cast<T*>(c.get()))
                return t;
        }
        return nullptr;
    }
    
private:
    std::vector<std::unique_ptr<ActorComponent>> m_components;
};
```

---

## Naming Conventions

| Typ | Format | Exempel |
|-----|--------|---------|
| Abstract base | Descriptive | `Character` |
| Concrete | Suffix med typ | `PlayerCharacter` |
| Interface | I-prefix | `IRenderable` |
| Component | -Component suffix | `SpriteComponent` |
| Manager | -Manager suffix | `TextureManager` |

---

## Anti-patterns

-  Djupa hierarkier (>4 nivåer)
-  God classes (för mycket ansvar)
-  Circular dependencies
-  Public member variables
-  dynamic_cast i hot paths
