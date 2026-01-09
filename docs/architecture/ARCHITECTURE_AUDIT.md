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
| `editor/legacy/` | 12 | 🔴 Legacy - TA BORT |

---

## 🔴 NYA FYND (Fördjupad Audit)

### 5. SINGLETON EXPLOSION - 37 Singletons!

Projektet har **37 singleton-klasser** med `static instance()`:

| Kategori | Antal | Exempel |
|----------|-------|---------|
| Systems | 15 | `AISystem`, `QuestSystem`, `InventorySystem`, `DialogSystem` |
| Managers | 8 | `RoomManager`, `SceneManager`, `TextureManager`, `AudioManager` |
| Data | 3 | `DataLoader`, `GameSettings`, `VideoSettings` |
| Editor | 7 | `CommandManager`, `EditorCore`, `ImGuiManager` |
| Utils | 4 | `Logger`, `FileWatcher`, `EventBus` |

**Problem:**
- Svårt att testa (global state)
- Implicit dependencies
- Ordningsproblem vid initiering/shutdown
- Memory leaks vid shutdown

**Rekommendation:**
- Konvertera till dependency injection
- Skapa `GameServices` container för runtime systems
- Skapa `EditorServices` container för editor

### 6. RoomManager vs SceneManager - IDENTISK KOD!

```cpp
// RoomManager.h - 63 rader
class RoomManager {
    std::unordered_map<std::string, std::unique_ptr<Room>> m_rooms;
    Room* m_currentRoom;
    void setOnRoomChange(callback);
    // ...
};

// SceneManager.h - 63 rader (EXAKT SAMMA!)
class SceneManager {
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
    Scene* m_currentScene;
    void setOnSceneChange(callback);
    // ...
};
```

**Åtgärd:** Konsolidera till endast `SceneManager`

### 7. DataLoader vs GameDataLoader

| Klass | Syfte | Problem |
|-------|-------|---------|
| `DataLoader` | Ladda JSON → data structs | OK |
| `GameDataLoader` | Registrera data i systems | Duplicerar logik |

`GameDataLoader` är en 368-raders fil som bara kopierar data från `DataLoader` till systems.

**Åtgärd:** Flytta registreringslogik till respektive System

### 8. ViewportPanel EXPLOSION - 12 filer!

```
editor/panels/viewport/
├── ViewportPanel.h (huvudklass)
├── ViewportPanel_Actors.cpp
├── ViewportPanel_Core.cpp
├── ViewportPanel_Input.cpp
├── ViewportPanel_Level.cpp
├── ViewportPanel_Scene.cpp
└── ViewportPanel_World.cpp

editor/viewport/
├── ViewportPanel.cpp/h (ANNAN FIL!)
├── ViewportPanelNew.cpp/h
├── Viewport3DPanel.cpp/h
├── ViewportRenderer.cpp/h
├── ViewportLevelRenderer.cpp/h
├── ViewportSceneRenderer.cpp/h
└── ViewportWorldRenderer.cpp/h
```

**Problem:**
- 2 olika `ViewportPanel` klasser i olika mappar!
- `ViewportPanelNew` - ofärdig refaktorering?
- 4 olika Renderer-klasser

**Åtgärd:** 
- Konsolidera till EN ViewportPanel
- EN renderer-hierarki

### 9. Legacy-mapp i Editor (12 filer)

```
editor/legacy/
├── DebugEditor.cpp/h
├── EditorTabRenderer.cpp/h
├── FileBrowser.cpp/h
├── RoomDataManager.cpp/h
├── TiledIntegration.cpp/h
└── VisualRoomEditor.cpp/h (16KB!)
```

**Åtgärd:** Granska och ta bort eller migrera

### 10. Duplicerade Data-strukturer

| Struct | Fil | Duplicering |
|--------|-----|-------------|
| `RoomData` | GameData.h | = `SceneData` (alias) |
| `HotspotData` | GameData.h | vs `Hotspot` i Hotspot.h |
| `NPCData` | GameData.h | vs `NPC`/`NPCActor` |
| `ItemData` | GameData.h | vs `Item` i InventorySystem.h |

Samma data definieras på flera ställen!

---

## 📋 Utökad Cleanup-Plan

### Fas 1: Ta bort Legacy (KRITISKT)
1. [ ] Ta bort `entities/` mappen helt
2. [ ] Ta bort `editor/legacy/` eller migrera nödvändigt
3. [ ] Konsolidera `RoomManager` → `SceneManager`

### Fas 2: Konsolidera Viewport
1. [ ] Slå ihop de 2 ViewportPanel-klasserna
2. [ ] Ta bort `ViewportPanelNew` om ofärdig
3. [ ] Förenkla renderer-hierarkin

### Fas 3: Reducera Singletons
1. [ ] Skapa `GameServices` container
2. [ ] Migrera systems till DI pattern
3. [ ] Behåll endast nödvändiga singletons (Logger, etc.)

### Fas 4: Konsolidera Data
1. [ ] Ta bort `RoomData` alias → använd `SceneData`
2. [ ] Slå ihop `GameDataLoader` logik i respektive System
3. [ ] Standardisera data-strukturer

---

## 🔴 ÄNNU FLER FYND (Djupare Audit)

### 11. TextureManager DUPLICERING - 2 stycken!

| Klass | Syfte | Problem |
|-------|-------|---------|
| `TextureManager` | SDL_Texture caching | SDL2 renderer |
| `GLTextureManager` | OpenGL texture caching | ImGui rendering |

Två helt separata texture managers med samma API men olika backends.

**Åtgärd:** Överväg unified interface eller tydlig dokumentation

### 12. RoomState.h har FEL @file-kommentar

```cpp
// RoomState.h säger:
@file RoomPanel.h  // FEL!
```

Filen heter `RoomState.h` men dokumentationen säger `RoomPanel.h`.

### 13. PlayState är MASSIV - 25KB!

`PlayState.cpp` är **25,742 bytes** - detta är en God Class.

Bör brytas upp i:
- `PlayerController`
- `HotspotInteraction`
- `NPCInteraction`
- `SceneNavigation`

### 14. 100+ TODO/FIXME kommentarer!

| Kategori | Antal | Filer |
|----------|-------|-------|
| Vendor (imgui, imnodes) | ~25 | Ignorera |
| Engine | ~40 | Behöver åtgärdas |
| Editor | ~35 | Behöver åtgärdas |

**Kritiska TODOs:**
- `Scene.cpp`: "TODO: Implement proper layer system with actors"
- `SceneLoader.cpp`: "TODO: Convert Layer to Actor-based system"
- `CollisionShape.cpp`: "TODO: Implement AABB vs Polygon"
- `KinematicBody.cpp`: "TODO: Implement proper collision normal"

### 15. 9 Game States - Möjlig Förenkling

| State | Storlek | Typ |
|-------|---------|-----|
| `PlayState` | 25KB | Core gameplay |
| `RoomState` | 17KB | Legacy? |
| `DialogState` | 7KB | Overlay |
| `InventoryState` | 5KB | Overlay |
| `MenuState` | 4KB | Menu |
| `PauseState` | 4KB | Overlay |
| `OptionsState` | 9KB | Menu |
| `SaveLoadState` | 6KB | Menu |
| `QuestLogState` | 5KB | Overlay |

**Fråga:** Är `RoomState` samma som `PlayState`? Duplicering?

### 16. Physics har 3 olika body-typer

```
engine/physics/
├── PhysicsBody.h (base)
├── KinematicBody.h 
├── CollisionShape.h
├── box2d/         (2D)
└── physx/         (3D)
```

Plus `RigidBody2DComponent` och `RigidBody3DComponent` i components.

**Risk:** Förvirring mellan physics abstractions och components

### 17. AI Tools är välstrukturerade ✅

```
ai/tools/
├── ActorTools.cpp (17KB)
├── SceneTools.cpp
├── DialogTools.cpp
├── QuestTools.cpp
├── ItemTools.cpp
├── HotspotTools.cpp
├── LevelTools.cpp
├── ContextTools.cpp
└── CommandTools.cpp
```

Bra separation of concerns.

---

## 📊 KOMPLETT STATISTIK

| Kategori | Antal | Problem |
|----------|-------|---------|
| Singletons | 37 | FÖR MÅNGA |
| TODO/FIXME | 100+ | Teknisk skuld |
| Duplicerade klasser | ~15 | Förvirrande |
| Legacy-filer | ~25 | Bör tas bort |
| God Classes (>15KB) | 3 | PlayState, RoomState, EditorWorldManager |

---

## 📋 REVIDERAD CLEANUP-PRIORITERING

> **Baserad på:** Michael Feathers "Working Effectively with Legacy Code"
> **Princip:** Ta bort legacy-kod SIST, inte först

### Feathers Legacy Code Change Algorithm

```
1. Identify change points     ← Var ska vi ändra?
2. Find test points           ← Var kan vi testa?
3. Break dependencies         ← Göra testbart (redirect, inte ta bort)
4. Write tests                ← Skydda mot regression
5. Make changes and REFACTOR  ← Ändra + ta bort legacy SIST
```

---

### FAS 1: Bygg ut ny funktionalitet (3D-fokus)
1. [x] StaticMeshActor med physics ✅
2. [x] RigidBody3DComponent ✅
3. [x] EditorPlayMode med Play/Pause/Stop ✅
4. [x] CameraActor (3D kamera med follow-target) ✅
5. [x] LightActor (Directional/Point/Spot) ✅
6. [ ] 3D scene serialization

### FAS 2: Bryt dependencies (redirect, inte ta bort)
6. [ ] `entities/Character.h` → forwarding header till `CharacterActor`
7. [ ] `RoomManager` → redirect till `SceneManager`
8. [ ] Fixa `RoomState.h` @file-kommentar
9. [ ] Reducera singletons via dependency injection

### FAS 3: Konsolidera dupliceringar
10. [ ] Slå ihop ViewportPanel-varianter
11. [ ] Slå ihop `GameDataLoader` logik i Systems
12. [ ] Standardisera data-strukturer

### FAS 4: Refactor och cleanup (SIST)
13. [ ] Ta bort `entities/` mappen (efter redirect fungerar)
14. [ ] Ta bort `editor/legacy/` (efter migration)
15. [ ] Ta bort forwarding headers
16. [ ] Städa TODO/FIXME

---

### ⚠️ Feathers-varningar (C++ specifikt)

> "Avoid singletons in your own code. **There's no reason to do this to yourself.**"

> "Breaking dependencies is worth introducing some ugliness. 
> Think of it as a **scar you can heal later** once you've tests in place."

### Forwarding Header Exempel

```cpp
// entities/Character.h - DEPRECATED forwarding header
#pragma once
#include "engine/actors/CharacterActor.h"

// DEPRECATED: Use engine::CharacterActor directly
using Character = engine::CharacterActor;
```

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

