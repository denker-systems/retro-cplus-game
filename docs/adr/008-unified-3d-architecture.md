# ADR-008: Unified 3D Architecture - 2D som Projection Mode

**Datum:** 2026-01-09  
**Status:** ✅ Accepterad  
**Beslutsfattare:** Calle + Cascade

---

## Kontext

Vi har ett fundamentalt arkitekturproblem med duplicerad 2D/3D logik:

### Nuvarande Problem
```
ActorObjectExtended
├── CharacterActor (2D - Vec2 position)
│   ├── PlayerActor (2D)
│   └── NPCActor (2D)
└── Character3DActor (3D - glm::vec3 position)
    └── Player3DActor (3D)
```

**Konsekvenser:**
- Duplicerad kod för physics (Box2D vs PhysX)
- Duplicerad kod för rendering (SDL2D vs OpenGL3D)
- Duplicerad kod för movement (2D vs 3D)
- Svårt att migrera actors mellan 2D och 3D
- Confusion om vilken actor-typ som ska användas
- Breaking changes vid migration

---

## Beslut

**Allt är 3D från grunden. 2D är en projection mode.**

### Ny Unified Hierarki
```
ActorObjectExtended (alltid 3D - glm::vec3 position)
└── Character3DActor (3D base för ALLA characters)
    ├── Player3DActor
    └── NPC3DActor
```

### 2D Mode = Projection av 3D

**2D mode innebär:**
- Kamera: Orthographic projection, låst Z-rotation
- Physics: PhysX 3D men låst till XY-plan (Z=0)
- Rendering: 3D meshes men kamera ser från sidan/ovanifrån
- Movement: 3D movement men låst till XY-plan

**Fördelar:**
- En enda actor-hierarki
- En enda physics engine (PhysX)
- En enda rendering pipeline (OpenGL)
- Enkel att växla mellan 2D och 3D mode
- Ingen kod-duplicering

---

## Implementation

### Fas 1: Unified Actor Hierarchy ✅ KLAR

**Skapade:**
- `NPC3DActor` - 3D NPC som ärver från Character3DActor
- Alla nya actors använder glm::vec3 position

**Deprecated:**
- `CharacterActor` (2D) - Används fortfarande av legacy editor-kod
- `NPCActor` (2D) - Ersätts av NPC3DActor
- `PlayerActor` (2D) - Ersätts av Player3DActor

### Fas 2: 2D Projection Mode (Framtida)

**Kamera:**
```cpp
class Camera3D {
    enum class ProjectionMode {
        Perspective,  // 3D mode
        Orthographic  // 2D mode
    };
    
    void setProjectionMode(ProjectionMode mode);
    void set2DView();  // Orthographic, look down Y-axis
};
```

**Physics:**
```cpp
class PhysicsWorld3D {
    void enable2DMode();  // Lock Z-axis, disable Z-rotation
    void disable2DMode(); // Full 3D physics
};
```

**Rendering:**
```cpp
// 2D mode: Kamera ser från sidan
camera->setPosition(glm::vec3(0, 0, 10));  // Look along Z-axis
camera->setProjectionMode(Orthographic);
camera->lockRotation(true);  // No rotation in 2D mode

// 3D mode: Free camera
camera->setProjectionMode(Perspective);
camera->lockRotation(false);
```

### Fas 3: Migration av Legacy 2D Actors (Framtida)

**Steg:**
1. Migrera alla 2D actors till 3D (CharacterActor → Character3DActor)
2. Konvertera Vec2 position till glm::vec3 (x, y) → (x, 0, y)
3. Ta bort Box2D helt (använd bara PhysX)
4. Ta bort SDL2D rendering (använd bara OpenGL)
5. Implementera 2D projection mode för backward compatibility

---

## Konsekvenser

### Positiva ✅
- **En enda actor-hierarki** - Ingen confusion
- **En enda physics engine** - PhysX för allt
- **En enda rendering pipeline** - OpenGL för allt
- **Enklare kod** - Ingen duplicering
- **Flexibilitet** - Enkelt att växla mellan 2D och 3D
- **Framtidssäkert** - Redo för full 3D från start
- **Unreal Engine-liknande** - Välkänd arkitektur

### Negativa ⚠️
- **Migration krävs** - Legacy 2D actors måste migreras
- **Breaking changes** - Gamla 2D-actors fungerar inte direkt
- **Mer komplex 2D mode** - Kräver projection mode implementation

### Teknisk Skuld
- [ ] Migrera CharacterActor till Character3DActor
- [ ] Migrera alla 2D actors till 3D
- [ ] Ta bort Box2D dependency
- [ ] Implementera 2D projection mode
- [ ] Ta bort SDL2D rendering kod

---

## Verifiering

### NPC3DActor Fungerar ✅
```
✅ NPC3DActor skapad (ärver från Character3DActor)
✅ RuntimeWorld laddar NPCs som NPC3DActor
✅ RuntimeRenderer renderar NPCs med rosa/bruna kuber
✅ 2 NPCs renderas i Tavern scene
✅ Bartender at (0.9, 1.0, 2.8)
✅ Drunk Patron at (2.1, 1.0, 2.9)
```

### Nästa Steg
1. Migrera EditorWorldManager att använda NPC3DActor
2. Implementera 2D projection mode i Camera3D
3. Migrera alla legacy 2D actors
4. Ta bort Box2D och SDL2D rendering

---

## Referenser

- Unreal Engine: Everything is 3D, 2D is orthographic camera
- Unity: Similar approach with 2D/3D modes
- Godot: 2D and 3D nodes but unified transform system

---

## Exempel: 2D Mode i Unified 3D

```cpp
// Setup 2D mode
camera->setProjectionMode(Camera3D::Orthographic);
camera->setPosition(glm::vec3(0, 10, 0));  // Look down from above
camera->lookAt(glm::vec3(0, 0, 0));
physics->enable2DMode();  // Lock Z-axis

// Actors are still 3D
player->setPosition3D(glm::vec3(5.0f, 0.0f, 3.0f));  // Y=0 in 2D mode
npc->setPosition3D(glm::vec3(2.0f, 0.0f, 1.0f));

// Rendering: 3D meshes but orthographic projection = looks 2D
renderer->render(camera, scene);  // Looks like 2D game!
```

---

## Migration Timeline

| Fas | Uppgift | Tid | Status |
|-----|---------|-----|--------|
| 1 | NPC3DActor | 30min | ✅ Klar |
| 2 | 2D Projection Mode | 2-3h | ⏳ Planerad |
| 3 | Migrera Legacy Actors | 4-6h | ⏳ Planerad |
| 4 | Ta bort Box2D | 1h | ⏳ Planerad |
| 5 | Ta bort SDL2D Rendering | 2h | ⏳ Planerad |

**Total:** 9-12 timmar för komplett migration

---

## Beslut

**Accepterat:** Unified 3D architecture är rätt väg framåt.

**Nästa session:** Implementera 2D projection mode och migrera fler actors.
