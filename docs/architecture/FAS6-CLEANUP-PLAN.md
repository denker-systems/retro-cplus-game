# Fas 6-7: Legacy Cleanup Plan

> Säker borttagning av deprecated 2D system

**Datum:** 2026-01-10  
**Status:** Investigation  
**Risk:** HÖG - Kan bryta legacy states

---

## 🔍 Dependency Analysis

### Box2D System

**Filer att ta bort:**
```
src/engine/physics/box2d/
├── PhysicsWorld2D.h/cpp
├── PhysicsLoader.h/cpp
└── PhysicsConversions.h
```

**Används av:**
- `src/engine/components/RigidBody2DComponent.cpp`
- `src/engine/components/CharacterController2D.cpp`
- `src/engine/components/Collider2DComponent.cpp`
- `src/game/states/PlayState.cpp` (legacy)
- `src/editor/core/EditorPlayMode.cpp` (PhysicsManager)

**CMakeLists.txt:**
- `find_package(box2d CONFIG REQUIRED)`
- `box2d::box2d` i target_link_libraries

---

### 2D Components

**Filer att ta bort:**
```
src/engine/components/
├── RigidBody2DComponent.h/cpp
├── CharacterController2D.h/cpp
└── Collider2DComponent.h/cpp
```

**Används av:**
- `src/game/states/PlayState.cpp` (legacy platformer mode)
- `src/editor/viewport/ui/ViewportActorRenderer.cpp` (rendering)

---

### Legacy Actors

**Filer att ta bort:**
```
src/engine/actors/
├── CharacterActor.h/cpp (PlayerActor + NPCActor definierade här)
├── Pawn.h/cpp
├── Controller.h/cpp
├── PlayerController.h/cpp
└── AIController.h/cpp
```

**Används av:**
- `src/game/states/PlayState.cpp` - PlayerActor
- `src/engine/world/Scene.cpp` - REDAN MIGRERAD till NPC3DActor
- `src/engine/entities/PlayerCharacter.h` - Forwarding header

---

## ⚠️ KRITISKA BEROENDEN

### Game States (BLOCKERS)

**PlayState.cpp använder:**
- `engine::PlayerActor m_player`
- `RigidBody2DComponent`
- `CharacterController2D`
- `Collider2DComponent`

**Problem:** PlayState används av:
- `src/game/states/MenuState.cpp` (startar PlayState)
- `src/editor/core/main.cpp` (editor startar med MenuState)
- `src/game/Game.cpp` (StateManager)

**Lösning:** Kan INTE ta bort förrän Game.cpp refactored.

---

### PhysicsManager

**PhysicsManager.h/cpp använder:**
- `PhysicsWorld2D* m_world2D`
- `PhysicsWorld3D* m_world3D`

**Används av:**
- EditorPlayMode (initializeBoth())
- RuntimeApp (PhysX only)

**Lösning:** Behåll PhysicsManager men gör 2D optional.

---

## 🎯 Säker Borttagningsplan

### Steg 1: Kommentera Ut i CMakeLists.txt (SÄKERT)

```cmake
# DEPRECATED: Box2D system - commented out for testing
# find_package(box2d CONFIG REQUIRED)
# src/engine/physics/box2d/PhysicsLoader.cpp
# src/engine/physics/box2d/PhysicsWorld2D.cpp
# src/engine/components/RigidBody2DComponent.cpp
# src/engine/components/CharacterController2D.cpp
# src/engine/components/Collider2DComponent.cpp
```

**Resultat:** Build kommer att faila för PlayState.cpp

---

### Steg 2: Fixa PlayState.cpp (KRÄVS)

**Option A: Minimal fix - Kommentera ut physics**
```cpp
// Kommentera ut platformer mode i PlayState
// if (settings.isPlatformerMode() && scene->hasPhysics()) {
//     // Physics kod...
// }
```

**Option B: Migrera till Player3DActor**
- Ersätt `PlayerActor` med `Player3DActor`
- Ta bort 2D physics components
- Använd CharacterController3DComponent

**Option C: Ta bort PlayState helt**
- Kräver Game.cpp refactor
- Editor startar direkt i EditorState (inte via MenuState)

---

### Steg 3: Uppdatera PhysicsManager (SÄKERT)

```cpp
// PhysicsManager.h
class PhysicsManager {
    // Make 2D optional
    void initialize3D();  // Only 3D
    void initializeBoth(); // 3D + 2D (legacy)
    
    bool is2DInitialized() const { return m_world2D != nullptr; }
};
```

---

### Steg 4: Ta Bort Filer (SIST)

**Endast när:**
- [ ] PlayState fixad eller borttagen
- [ ] Alla builds lyckas
- [ ] Editor fungerar
- [ ] Runtime fungerar

**Filer att ta bort:**
1. `src/engine/physics/box2d/` (hela mappen)
2. `src/engine/components/RigidBody2DComponent.*`
3. `src/engine/components/CharacterController2D.*`
4. `src/engine/components/Collider2DComponent.*`

---

## 🚨 REKOMMENDATION

**GÖR INTE Fas 6 ÄN!**

**Anledning:**
- PlayState används fortfarande av editor startup
- Bryter editor om vi tar bort 2D components
- Kräver Game.cpp refactor först

**Istället:**
1. Behåll deprecated filer (markerade som DEPRECATED)
2. Fokusera på att 3D-systemet fungerar 100%
3. Planera Game.cpp refactor i separat session
4. Gör Fas 6 cleanup när Game.cpp inte behöver states

---

## ✅ Vad Som Kan Göras Säkert NU

**Fas 7 Cleanup (delvis):**
- [ ] Uppdatera dokumentation
- [ ] Skapa migration guide
- [ ] Testa att runtime fungerar 100%
- [ ] Testa att editor fungerar 100%
- [ ] Verifiera Build Panel

**Fas 6 (FRAMTIDA SESSION):**
- Kräver Game.cpp refactor först
- Sedan kan Box2D och 2D components tas bort säkert

---

## 📝 Slutsats

**Fas 1-5:** ✅ KLARA och säkra  
**Fas 6:** ⚠️ VÄNTA - Kräver Game.cpp refactor  
**Fas 7:** ✅ Delvis möjlig (dokumentation, testing)

**Rekommendation:** Avsluta migration här. Fas 6 görs i framtida session efter Game.cpp refactor.
