---
trigger: glob
globs: ["src/engine/actors/*.cpp", "src/engine/actors/*.h", "src/engine/core/*.cpp", "src/engine/core/*.h"]
description: Actor and Entity guidelines for game objects
---

# Entity & Actor Guidelines

> Component-based Actor system

## Actor Hierarchy

```
Object (abstract root)
 ActorObject (transform, visibility)
     ActorObjectExtended (component support)
         CharacterActor
            PlayerActor
            NPCActor
         EnvironmentActor
            TileMapActor
            PropActor
         InteractiveActor
            HotspotActor
         SpriteActor
```

---

## Base ActorObject

```cpp
class ActorObject : public Object {
public:
    // Transform
    void setPosition(float x, float y);
    Vec2 getPosition() const;
    
    // Visibility
    void setVisible(bool visible);
    bool isVisible() const;
    
    // Lifecycle
    virtual void onActorBegin();
    virtual void onActorEnd();
    
protected:
    Vec2 m_position{0, 0};
    float m_rotation = 0.0f;
    Vec2 m_scale{1, 1};
    bool m_visible = true;
};
```

---

## Component System

```cpp
// Add components to actors
auto* sprite = actor->addComponent<SpriteComponent>();
auto* rb = actor->addComponent<RigidBody2DComponent>();

// Get components
auto* sprite = actor->getComponent<SpriteComponent>();
```

---

## Actor Types

| Actor | Användning |
|-------|------------|
| PlayerActor | Spelarkaraktär, input |
| NPCActor | AI-karaktärer, dialog |
| HotspotActor | Interaktiva områden |
| PropActor | Statiska objekt |
| TileMapActor | Tile-baserade nivåer |

---

## Best Practices

- **Composition over inheritance** - Use components
- **Max 3-4 levels** in hierarchy
- **Virtual destructor** in base classes
- **Override keyword** for virtual methods
