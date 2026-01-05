---
description: C++ OOP arkitektur och design patterns för spelutveckling
---

# OOP Architecture Rules

## Principer

### 1. Prefer Composition Over Inheritance
- Använd komponenter för delad funktionalitet
- Max 2-3 nivåer i arvshierarki
- Undvik "diamond of death" (multipelt arv)

### 2. Single Responsibility Principle
- En klass = ett ansvar
- Separera data från logik när möjligt
- Managers hanterar collections, inte enskilda objekt

### 3. Interface Segregation
- Små, fokuserade interfaces (IRenderable, IUpdatable, ICollidable)
- Klasser implementerar endast relevanta interfaces

## Entity Hierarki

```
Entity (abstract)
├── Character (abstract)
│   ├── PlayerCharacter
│   └── AICharacter (abstract)
│       ├── NPCCharacter
│       └── EnemyCharacter
├── Item
└── Hotspot
```

## Kod-struktur

### Abstract Base Classes
```cpp
// Alla entities ärver från Entity
class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    
    // Position och storlek
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    
protected:
    float m_x = 0, m_y = 0;
    int m_width = 0, m_height = 0;
    bool m_active = true;
};
```

### Components (Composition)
```cpp
// Komponenter för delad funktionalitet
class MovementComponent {
public:
    void setTarget(float x, float y);
    void update(float deltaTime, float& x, float& y);
private:
    float m_targetX, m_targetY;
    float m_speed = 100.0f;
};

class AnimationComponent {
public:
    void play(const std::string& animName);
    void update(float deltaTime);
    int getCurrentFrame() const;
};
```

### Character med Components
```cpp
class Character : public Entity {
public:
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
protected:
    std::unique_ptr<MovementComponent> m_movement;
    std::unique_ptr<AnimationComponent> m_animation;
    int m_health = 100;
    std::string m_name;
};
```

## Filstruktur

```
src/
├── entities/
│   ├── Entity.h              # Abstract base
│   ├── Character.h/.cpp      # Character base
│   ├── PlayerCharacter.h/.cpp
│   ├── AICharacter.h/.cpp    # AI base
│   ├── NPCCharacter.h/.cpp
│   ├── Item.h/.cpp
│   └── Hotspot.h/.cpp
│
├── components/
│   ├── MovementComponent.h/.cpp
│   ├── AnimationComponent.h/.cpp
│   ├── CollisionComponent.h/.cpp
│   └── DialogComponent.h/.cpp
```

## Naming Conventions

| Typ | Format | Exempel |
|-----|--------|---------|
| Abstract class | Prefix med beskrivning | `Character`, `AICharacter` |
| Concrete class | Suffix med typ | `PlayerCharacter`, `NPCCharacter` |
| Interface | Prefix med I | `IRenderable`, `IUpdatable` |
| Component | Suffix med Component | `MovementComponent` |
| Manager/System | Suffix med Manager/System | `EntityManager`, `DialogSystem` |

## Best Practices

1. **Virtual destructor** - Alltid i base classes
2. **Override keyword** - Använd alltid vid override
3. **Protected members** - För data som subklasser behöver
4. **Unique_ptr för components** - RAII och tydlig ownership
5. **Forward declarations** - I headers, includes i .cpp
6. **Const correctness** - Getters ska vara const

## Anti-patterns att undvika

- ❌ Djupa arvshierarkier (>3 nivåer)
- ❌ God classes (för mycket ansvar)
- ❌ Circular dependencies
- ❌ Public member variables (utom structs)
- ❌ Dynamic_cast i hot paths
