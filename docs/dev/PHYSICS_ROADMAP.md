# Physics System Roadmap - Box2D Integration

**Start:** 2026-01-06  
**Status:** ✅ Fas 1-4 KLARA - Core physics implementerat

---

## OOP Arkitektur - Följer Engine Pattern

**VIKTIGT:** Fysik-systemet måste följa vår befintliga Actor/Component-arkitektur.

### Befintlig Hierarki
```
Object (abstract)
└── ActorObject (concrete)
    └── ActorObjectExtended (+ components)
        ├── CharacterActor
        │   ├── PlayerActor
        │   └── NPCActor
        ├── InteractiveActor
        └── EnvironmentActor
```

### Ny Physics-Integration
```
ActorObjectExtended
├── addComponent<RigidBody2DComponent>()   ← Box2D body
├── addComponent<Collider2DComponent>()    ← Box2D shape
└── addComponent<CharacterController2D>()  ← Platformer logic

PhysicsWorld2D (owned by Scene/Level)
├── manages all b2Bodies
├── step() called in game loop
└── debug rendering
```

### Component Pattern (som befintliga)
```cpp
// Samma mönster som SpriteComponent, MovementComponent etc
class RigidBody2DComponent : public ActorComponent {
    // Följer exakt samma livscykel:
    void onAttach() override;   // Skapa Box2D body
    void onDetach() override;   // Destroy Box2D body
    void update(float dt) override;  // Sync position
};
```

---

## Översikt - Före/Efter

```
Befintligt system (egen kod)     →    Box2D-baserat system
├── PhysicsBody                       ├── PhysicsWorld2D (Scene-owned)
├── KinematicBody                     ├── RigidBody2DComponent
├── CollisionShape                    ├── Collider2DComponent
└── SpatialGrid                       ├── CharacterController2D
                                      └── PhysicsContactListener
```

---

## Fas 1: Box2D Foundation ⏳

**Mål:** Skapa wrapper-klasser för Box2D som integreras med Scene

### Filer att skapa:
```
src/engine/physics/box2d/
├── PhysicsWorld2D.h/cpp      # Box2D world wrapper
├── PhysicsDebugDraw.h/cpp    # SDL2 debug rendering  
└── PhysicsConversions.h      # Pixel ↔ Meter helpers (GLM-baserad)
```

### PhysicsWorld2D (ägs av Scene)
```cpp
class PhysicsWorld2D {
    b2WorldId m_worldId;
    float m_pixelsPerMeter = 32.0f;
    
    // Lifecycle (kallas från Scene)
    void initialize();
    void shutdown();
    void step(float deltaTime);
    
    // Body management (kallas från RigidBody2DComponent)
    b2BodyId createBody(const BodyDef2D& def);
    void destroyBody(b2BodyId body);
    
    // World settings
    void setGravity(glm::vec2 gravity);
    glm::vec2 getGravity() const;
    
    // Debug (kallas från editor)
    void debugDraw(SDL_Renderer* renderer);
};
```

### Integration med Scene
```cpp
class Scene : public WorldContainer {
    std::unique_ptr<PhysicsWorld2D> m_physicsWorld;
    
    void update(float dt) override {
        // Physics step FÖRE actor updates
        if (m_physicsWorld) {
            m_physicsWorld->step(dt);
        }
        
        // Sedan actor updates (synkar position från Box2D)
        WorldContainer::update(dt);
    }
};
```

### Konverteringar
```cpp
// Box2D använder meter, vi använder pixlar
inline b2Vec2 pixelsToMeters(float x, float y);
inline glm::vec2 metersToPixels(b2Vec2 pos);
```

**Estimat:** 1-2 timmar

---

## Fas 2: RigidBody2DComponent ⏳

**Mål:** Component för actors med Box2D-fysik (följer ActorComponent pattern)

### Filer:
```
src/engine/components/
├── RigidBody2DComponent.h/cpp   # Ärver ActorComponent
└── Collider2DComponent.h/cpp    # Ärver ActorComponent
```

### RigidBody2DComponent (följer samma pattern som MovementComponent)
```cpp
class RigidBody2DComponent : public ActorComponent {
public:
    RigidBody2DComponent(const std::string& name = "RigidBody2DComponent");
    
    // ═══════════════════════════════════════════════════════════════════
    // ACTORCOMPONENT LIFECYCLE (samma som andra components)
    // ═══════════════════════════════════════════════════════════════════
    
    void onAttach() override {
        // Hämta PhysicsWorld från Scene
        // Skapa Box2D body
    }
    
    void onDetach() override {
        // Destroy Box2D body
    }
    
    void update(float deltaTime) override {
        // Sync Actor position ← Box2D position
        m_owner->setPosition(metersToPixels(b2Body_GetPosition(m_bodyId)));
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // BODY TYPE
    // ═══════════════════════════════════════════════════════════════════
    
    enum class BodyType { Static, Dynamic, Kinematic };
    void setBodyType(BodyType type);
    BodyType getBodyType() const;
    
    // ═══════════════════════════════════════════════════════════════════
    // PHYSICS CONTROL
    // ═══════════════════════════════════════════════════════════════════
    
    void setVelocity(glm::vec2 vel);
    glm::vec2 getVelocity() const;
    
    void addForce(glm::vec2 force);
    void addImpulse(glm::vec2 impulse);
    
    void setGravityScale(float scale);
    void setFixedRotation(bool fixed);
    
private:
    b2BodyId m_bodyId;
    PhysicsWorld2D* m_world = nullptr;
    BodyType m_bodyType = BodyType::Dynamic;
};
```

### Collider2DComponent (sibling till RigidBody2DComponent)
```cpp
class Collider2DComponent : public ActorComponent {
public:
    Collider2DComponent(const std::string& name = "Collider2DComponent");
    
    void onAttach() override {
        // Hitta sibling RigidBody2DComponent
        // Skapa shape på dess body
    }
    
    // Shape types
    void setBoxShape(float width, float height);
    void setCircleShape(float radius);
    void setCapsuleShape(float width, float height);
    
    // Collision settings
    void setTrigger(bool isTrigger);
    void setCollisionLayer(uint16_t layer);
    void setCollisionMask(uint16_t mask);
    void setFriction(float friction);
    void setRestitution(float bounce);
    
private:
    b2ShapeId m_shapeId;
    RigidBody2DComponent* m_rigidBody = nullptr;
};
```

### Användning (samma stil som befintliga components)
```cpp
// Skapa actor med physics (precis som vi gör med SpriteComponent)
auto platform = std::make_unique<ActorObjectExtended>("Platform");
platform->setPosition(100, 300);

auto* rb = platform->addComponent<RigidBody2DComponent>();
rb->setBodyType(RigidBody2DComponent::BodyType::Static);

auto* col = platform->addComponent<Collider2DComponent>();
col->setBoxShape(200, 32);
col->setCollisionLayer(CollisionLayer::Ground);

scene->addActor(std::move(platform));
```

**Estimat:** 2-3 timmar

---

## Fas 3: CharacterController2D ⏳

**Mål:** Platformer-specifik rörelselogik

### Fil:
```
src/engine/physics/CharacterController2D.h/cpp
```

### Features:
```cpp
class CharacterController2D : public ActorComponent {
    // Movement
    float walkSpeed = 200.0f;
    float runSpeed = 350.0f;
    float jumpForce = 400.0f;
    
    // State
    bool isGrounded();
    bool isJumping();
    bool isRunning();
    
    // Actions
    void move(float direction);  // -1 to 1
    void jump();
    void stopJump();  // Variable jump height
    
    // Advanced
    float coyoteTime = 0.1f;     // Grace period after leaving ground
    float jumpBufferTime = 0.1f; // Pre-jump input buffer
    int maxAirJumps = 0;         // Double jump etc
};
```

**Estimat:** 2-3 timmar

---

## Fas 4: Collider Types ⏳

**Mål:** Olika kollisionstyper för level design

### Collision Layers:
```cpp
enum class CollisionLayer : uint16_t {
    None       = 0,
    Player     = 1 << 0,  // 1
    Enemy      = 1 << 1,  // 2
    Ground     = 1 << 2,  // 4
    Wall       = 1 << 3,  // 8
    Platform   = 1 << 4,  // 16 (one-way)
    Trigger    = 1 << 5,  // 32
    Projectile = 1 << 6,  // 64
    Pickup     = 1 << 7,  // 128
};
```

### Platform Types:
- **Solid** - Full collision
- **OneWay** - Can jump through from below
- **Moving** - Kinematic platform
- **Crumbling** - Breaks after standing on it

**Estimat:** 1-2 timmar

---

## Fas 5: Editor Integration ⏳

**Mål:** Visualisera och editera colliders i editorn

### Features:
- [ ] Rita colliders i Scene view (debug overlay)
- [ ] Collider editing tools (resize, move)
- [ ] Collision layer dropdown i Properties
- [ ] Play-in-editor physics test

### UI:
```
Properties Panel:
┌─ RigidBody2D ─────────────────┐
│ Body Type: [Dynamic ▼]        │
│ Mass: [1.0    ]               │
│ Gravity Scale: [1.0    ]      │
│ Fixed Rotation: [✓]           │
├─ Collider ────────────────────┤
│ Shape: [Box ▼]                │
│ Size: [32] x [64]             │
│ Is Trigger: [ ]               │
│ Layer: [Player ▼]             │
│ Mask: [Ground, Wall ▼]        │
└───────────────────────────────┘
```

**Estimat:** 3-4 timmar

---

## Fas 6: Player Migration ⏳

**Mål:** Uppgradera PlayerActor till Box2D

### Ändringar:
```cpp
class PlayerActor : public CharacterActor {
    // Nya komponenter
    RigidBody2DComponent* m_rigidBody;
    Collider2DComponent* m_collider;
    CharacterController2D* m_controller;
    
    void update(float dt) override {
        // Input → Controller
        float moveDir = 0;
        if (input->isKeyDown(SDL_SCANCODE_A)) moveDir = -1;
        if (input->isKeyDown(SDL_SCANCODE_D)) moveDir = 1;
        
        m_controller->move(moveDir);
        
        if (input->isKeyDown(SDL_SCANCODE_LSHIFT))
            m_controller->setRunning(true);
        
        if (input->isKeyPressed(SDL_SCANCODE_SPACE))
            m_controller->jump();
    }
};
```

**Estimat:** 2 timmar

---

## Fas 7: Scene Integration ⏳

**Mål:** Ladda colliders från scenes.json

### JSON Format:
```json
{
  "id": "level1",
  "colliders": [
    {
      "type": "box",
      "x": 0, "y": 380,
      "width": 640, "height": 20,
      "layer": "ground"
    },
    {
      "type": "box", 
      "x": 100, "y": 300,
      "width": 80, "height": 16,
      "layer": "platform",
      "oneWay": true
    }
  ]
}
```

**Estimat:** 2 timmar

---

## Tidsestimering

| Fas | Beskrivning | Tid |
|-----|-------------|-----|
| 1 | Box2D Foundation | 1-2h |
| 2 | RigidBody2DComponent | 2-3h |
| 3 | CharacterController2D | 2-3h |
| 4 | Collider Types | 1-2h |
| 5 | Editor Integration | 3-4h |
| 6 | Player Migration | 2h |
| 7 | Scene Integration | 2h |
| **Total** | | **13-18h** |

---

## Dependencies

```
box2d v3.1.1  ✅ Installerad
glm v1.0.2    ✅ Installerad
```

---

## Backward Compatibility

Befintlig physics-kod (`PhysicsBody`, `KinematicBody`, `CollisionShape`) behålls för:
- Legacy kod som använder den
- Enkel kollisionsdetektering utan full physics

Box2D används för:
- Player/Enemy physics
- Dynamiska objekt
- Platformer mechanics

---

## Nästa steg

1. Starta **Fas 1** - Skapa `PhysicsWorld2D`
2. Testa med enkel gravity simulation
3. Fortsätt med komponenter
