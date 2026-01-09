# Architecture Audit Report

> Djupgående analys av Engine/Editor arkitektur
> Datum: 2026-01-08

## 🔴 Kritiska Fynd: 3 Parallella Hierarkier

Projektet har **TRE** separata objekt-hierarkier som gör samma sak:

### Hierarki 1: Entity-system (LEGACY)
```
Entity (src/engine/entities/)
└── Character
    └── PlayerCharacter
```
**Problem:** Duplicerar Object/ActorObject funktionalitet
**Status:** Bör fasas ut

### Hierarki 2: Actor-system (MODERN - UE5-stil)
```
Object (src/engine/core/)
└── ActorObject
    └── ActorObjectExtended (+ komponenter)
        ├── VisualActor
        ├── CharacterActor → PlayerActor, NPCActor
        ├── EnvironmentActor
        ├── SpriteActor
        ├── InteractiveActor
        ├── ItemActor
        └── StaticMeshActor (3D)
```
**Status:** ✅ Rätt arkitektur - använd denna

### Hierarki 3: Node-system (BORTTAGET men spår kvar)
```
nodes/ mapp innehåller fortfarande:
├── Sprite.h → ärver från VisualActor (OK)
├── AnimatedSprite.h
├── Label.h
├── ParallaxLayer.h
├── TileMapLayer.h
└── WalkArea.h
```
**Status:** Hybrid - ärver nu från VisualActor

---

## 🟠 Duplicerad Funktionalitet

### 1. Sprite-rendering (3 implementationer!)

| Klass | Plats | Syfte |
|-------|-------|-------|
| `Sprite` | nodes/ | Legacy node, ärver VisualActor |
| `SpriteComponent` | components/ | Komponent för sprite-rendering |
| `SpriteActor` | actors/ | Actor med SpriteComponent |

**Rekommendation:** 
- Behåll `SpriteComponent` som primär
- Konvertera `Sprite` till att använda `SpriteComponent` internt
- `SpriteActor` är korrekt (wrapper med komponent)

### 2. Character (2 implementationer!)

| Klass | Plats | Ärver från |
|-------|-------|------------|
| `Character` | entities/ | Pawn |
| `CharacterActor` | actors/ | ActorObjectExtended |

**Problem:** 
- `Character` i entities/ ärver från `Pawn` 
- `CharacterActor` i actors/ ärver från `ActorObjectExtended`
- Duplicerade fält: `m_moveSpeed`, `m_health`, `m_name`

**Rekommendation:**
- Fasa ut `entities/Character.h`
- Använd endast `actors/CharacterActor.h`

### 3. NPC (Wrapper pattern - OK men onödigt)

| Klass | Plats | Ärver från |
|-------|-------|------------|
| `NPC` | actors/NPC.h | Inget (wrapper) |
| `NPCActor` | actors/CharacterActor.h | CharacterActor |

**Analys:** `NPC` är en wrapper som innehåller `std::unique_ptr<NPCActor>` - 
detta är för kompatibilitet med legacy `Room` system.

**Rekommendation:**
- Fasa ut `NPC` wrapper när `Room` → `Scene` migration är klar
- Använd `NPCActor` direkt i `Scene`

### 4. Position/Transform (duplicerat överallt)

| Klass | Fält |
|-------|------|
| `Entity` | `m_x, m_y, m_width, m_height` |
| `Character` | `m_x, m_y, m_width, m_height` (igen!) |
| `ActorObject` | `m_position, m_positionZ, m_rotation, m_scale` |

**Problem:** Character duplicerar sina egna x/y istället för att använda ärvda

---

## 🟡 Arkitekturproblem

### 1. Room vs Scene
- `Room` (engine/Room.h) - Legacy rum-klass
- `Scene` (engine/world/Scene.h) - Modern WorldContainer-baserad

**Problem:** Båda används parallellt, data synkas mellan dem via `RoomToSceneConverter`

### 2. WorldContainer har Box2D, men EditorPlayMode har egen PhysicsManager
- `WorldContainer` har `std::unique_ptr<physics::PhysicsWorld2D> m_physicsWorld`
- `EditorPlayMode` har `engine::physics::PhysicsManager m_physicsManager`

**Risk:** Två parallella physics-världar

### 3. Koordinatmappning (2D ↔ 3D) - SPRIDD ÖVER 13 FILER!

`100.0f` skalningsfaktor används på **33 ställen** i **13 filer**:

| Fil | Användningar |
|-----|--------------|
| `Viewport3DPanel.cpp` | 6 |
| `TransformGizmo3D.cpp` | 4 |
| `ViewportWorldRenderer.cpp` | 4 |
| `StaticMeshActor.cpp` | 4 |
| `RigidBody3DComponent.cpp` | 3 |
| `EditorPlayMode.cpp` | 2 |
| + 7 andra filer | ... |

**Mappningslogik (bör centraliseras):**
```cpp
// 2D pixels → 3D meters
physics_x = actor_2D_x / 100.0f
physics_y = actor_Z / 100.0f    // Höjd
physics_z = actor_2D_y / 100.0f // Djup

// 3D meters → 2D pixels  
actor_2D_x = physics_x * 100.0f
actor_Z = physics_y * 100.0f    // Höjd
actor_2D_y = physics_z * 100.0f // Djup
```

**Risk:** Inkonsistent mappning, svårt att underhålla, magic number överallt

---

## ✅ Bra Implementationer

1. **WorldContainer pattern** - Bra abstraktion för World/Level/Scene
2. **Component system** - UE5-inspirerad, välstrukturerad
3. **ActorObjectExtended** - Bra komposition via komponenter
4. **CoreRedirects.h** - Tydlig migrationsdokumentation

---

## 📋 Rekommenderad Cleanup-Plan

### Fas 1: Ta bort Entity-hierarkin (HÖG PRIORITET)
1. [ ] Migrera `PlayerCharacter` → `PlayerActor`
2. [ ] Migrera `Character` funktionalitet → `CharacterActor`
3. [ ] Ta bort `entities/` mappen

### Fas 2: Konsolidera NPC
1. [ ] Slå ihop `actors/NPC.h` med `NPCActor` i `CharacterActor.h`
2. [ ] Uppdatera alla referenser

### Fas 3: Standardisera Sprite-rendering
1. [ ] `Sprite` node → använd `SpriteComponent` internt
2. [ ] Deprecate direkt `SDL_Texture*` hantering i `Sprite`

### Fas 4: Konsolidera Room/Scene
1. [ ] Fasa ut `Room` klass helt
2. [ ] Migrera all RoomData → Scene direkt

### Fas 5: Centralisera koordinatmappning
1. [ ] Skapa `CoordinateMapper` utility-klass
2. [ ] All 2D↔3D mappning via denna

---

## 📊 Fil-statistik

| Mapp | Filer | Status |
|------|-------|--------|
| `engine/core/` | 12 | ✅ Modern |
| `engine/actors/` | 24 | ✅ Modern |
| `engine/components/` | 40 | ✅ Modern |
| `engine/entities/` | 7 | 🔴 Legacy - TA BORT |
| `engine/nodes/` | 16 | 🟡 Hybrid |
| `engine/world/` | 16 | ✅ Modern |

---

## Klasshierarki-diagram

```
MODERN (Använd denna):
engine::Object
└── engine::ActorObject
    └── engine::ActorObjectExtended
        ├── VisualActor
        │   └── Sprite (nodes/)
        ├── CharacterActor
        │   ├── PlayerActor
        │   └── NPCActor
        ├── EnvironmentActor
        ├── SpriteActor
        ├── InteractiveActor
        ├── ItemActor
        └── StaticMeshActor

LEGACY (Ta bort):
Entity
└── Character (ärver även Pawn!)
    └── PlayerCharacter

WorldContainer
├── World
├── Level
└── Scene
```

