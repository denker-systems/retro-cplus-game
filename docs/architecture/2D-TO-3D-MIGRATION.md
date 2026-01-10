# 2D → Unified 3D Migration Plan

> Komplett kartläggning och migration plan

**Datum:** 2026-01-09  
**Status:** Planering  
**Estimat:** 15-20 timmar totalt

---

## 🔍 Nuvarande Läge - Duplicerad Arkitektur

### Problem: Två Parallella System

```
┌─────────────────────────────────────────────────────────┐
│                    NUVARANDE (DUPLICERAT)                │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  2D System                    3D System                  │
│  ├── Vec2 position            ├── glm::vec3 position    │
│  ├── Box2D physics            ├── PhysX physics         │
│  ├── SDL2D rendering          ├── OpenGL rendering      │
│  ├── CharacterActor           ├── Character3DActor      │
│  ├── NPCActor                 ├── NPC3DActor ✅         │
│  ├── PlayerActor              ├── Player3DActor ✅      │
│  └── RigidBody2DComponent     └── RigidBody3DComponent  │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

**Konsekvens:** Kod-duplicering, confusion, migration svår

---

## 🎯 Målbild - Unified 3D Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   UNIFIED 3D (MÅL)                       │
├─────────────────────────────────────────────────────────┤
│                                                          │
│  Allt är 3D                                              │
│  ├── glm::vec3 position (alltid)                        │
│  ├── PhysX physics (alltid)                             │
│  ├── OpenGL rendering (alltid)                          │
│  ├── Character3DActor (bas för alla)                    │
│  ├── NPC3DActor                                          │
│  ├── Player3DActor                                       │
│  └── RigidBody3DComponent                                │
│                                                          │
│  2D Mode = Projection                                    │
│  ├── Orthographic camera                                 │
│  ├── PhysX med locked Z-axis                            │
│  └── Same rendering, different view                     │
│                                                          │
└─────────────────────────────────────────────────────────┘
```

**Fördelar:** En kodbas, enkel migration, flexibel

---

## 📊 Kartläggning

### Actors

| Actor | Typ | Används Av | Migration |
|-------|-----|------------|-----------|
| **3D (Klara)** ||||
| Character3DActor | 3D ✅ | Runtime, Editor | ✅ Klar |
| Player3DActor | 3D ✅ | Runtime, Editor | ✅ Klar |
| NPC3DActor | 3D ✅ | Runtime | ✅ Klar |
| StaticMeshActor | 3D ✅ | Editor | ✅ Klar |
| CameraActor | 3D ✅ | Editor | ✅ Klar |
| LightActor | 3D ✅ | Editor | ✅ Klar |
| PlayerStartActor | 3D ✅ | Runtime, Editor | ✅ Klar |
| PlayerConfigActor | 3D ✅ | Editor | ✅ Klar |
| **2D (Legacy)** ||||
| CharacterActor | 2D ❌ | Editor (legacy states) | ⏳ Migrera |
| PlayerActor | 2D ❌ | Editor (legacy states) | ⏳ Migrera |
| NPCActor | 2D ❌ | Editor (EditorWorldManager) | ⏳ Migrera |
| Pawn | 2D ❌ | Legacy | ⏳ Ta bort |
| Controller | 2D ❌ | Legacy | ⏳ Ta bort |
| VisualActor | 2D ❌ | Legacy nodes | ⏳ Ta bort |
| InteractiveActor | 2D ❌ | Legacy | ⏳ Ta bort |
| ItemActor | 2D ❌ | Legacy | ⏳ Ta bort |
| SpriteActor | 2D ❌ | Legacy | ⏳ Ta bort |
| EnvironmentActor | 2D ❌ | Legacy | ⏳ Ta bort |

### Components

| Component | Typ | Används | Migration |
|-----------|-----|---------|-----------|
| **3D (Klara)** ||||
| RigidBody3DComponent | 3D ✅ | Editor | ✅ Klar |
| CharacterController3DComponent | 3D ✅ | Runtime, Editor | ✅ Klar |
| CameraComponent | 3D ✅ | Editor | ✅ Klar |
| MeshComponent | 3D ✅ | Editor | ✅ Klar |
| **2D (Legacy)** ||||
| RigidBody2DComponent | 2D ❌ | Legacy states | ⏳ Migrera |
| Collider2DComponent | 2D ❌ | Legacy states | ⏳ Migrera |
| CharacterController2D | 2D ❌ | Legacy states | ⏳ Migrera |
| **Neutral (Fungerar med båda)** ||||
| SpriteComponent | 2D/3D ✅ | Editor, Runtime | ✅ Behåll |
| AnimationComponent | 2D/3D ✅ | Editor | ✅ Behåll |
| DialogComponent | Neutral ✅ | Legacy | ✅ Behåll |
| InventoryComponent | Neutral ✅ | Legacy | ✅ Behåll |
| MovementComponent | 2D ❌ | Legacy | ⏳ Migrera |

### Physics

| System | Typ | Filer | Används | Migration |
|--------|-----|-------|---------|-----------|
| PhysX 3D | 3D ✅ | 8 filer | Runtime, Editor | ✅ Behåll |
| Box2D | 2D ❌ | 12 filer | Legacy states | ⏳ Ta bort |

### Rendering

| System | Typ | Används | Migration |
|--------|-----|---------|-----------|
| OpenGL 3D | 3D ✅ | Runtime, Editor viewport | ✅ Behåll |
| SDL2D | 2D ❌ | Legacy states, UI | ⏳ Migrera UI till ImGui |

---

## 🚀 Migration Plan

### Fas 1: Core Actors ✅ KLAR (30 min)
- [x] Skapa NPC3DActor
- [x] Uppdatera RuntimeWorld
- [x] Uppdatera RuntimeRenderer
- [x] Verifiera NPCs renderas

### Fas 2: Editor Integration ⏳ PÅGÅENDE (~90% klar)
- [x] Scene.cpp skapar NPC3DActor (commit d3c667b)
- [x] Viewport3DPanel renderar NPC3DActor (commit 7c777d9)
- [x] RuntimeWorld laddar scenes.json fallback (commit 91eb4b8)
- [x] EditorPlayMode uppdaterar NPC3DActor (commit nästa)
- [ ] Testa editor Play mode - verifiera NPCs syns
- [ ] Testa tester.exe - verifiera NPCs laddas från scenes.json

### Fas 3: 2D Projection Mode ⏳ PÅGÅENDE (~80% klar)
- [x] Implementera Camera3D::setProjectionMode(Orthographic) (commit cae79c8)
- [x] Orthographic projection matrix implementation
- [x] UI toggle för 2D/3D mode i ViewportToolbar (commit nästa)
- [x] Implementera PhysicsWorld3D::enable2DMode() (lock Z-axis)
- [x] Koppla 2D/3D buttons till camera + physics
- [ ] Testa 2D-läge i editor viewport
- [ ] Testa 2D-läge i runtime

### Fas 4: Migrera Legacy Actors ✅ KLAR (~100% klar)
- [x] Markera CharacterActor/PlayerActor/NPCActor som DEPRECATED
- [x] Dokumentera migration path i headers
- [x] Markera VisualActor, InteractiveActor, ItemActor som 3D-kompatibla
- [x] Beslut: Behåll Vec2 i ActorObject för backward compatibility
- [x] Nya 3D actors använder glm::vec3 (Character3DActor, NPC3DActor, etc.)

**Strategi:** Hybrid approach
- ActorObject behåller Vec2 + Z för compatibility
- Character3DActor använder glm::vec3 direkt
- Båda systemen fungerar parallellt
- Legacy actors tas bort i Fas 6 efter Game.cpp refactor

### Fas 5: Migrera Components ✅ KLAR (~100% klar)
- [x] Markera RigidBody2DComponent som DEPRECATED
- [x] Markera Collider2DComponent som DEPRECATED
- [x] Markera CharacterController2D som DEPRECATED
- [x] Dokumentera att RigidBody3DComponent ska användas
- [x] Dokumentera att CharacterController3DComponent ska användas

**Strategi:** Hybrid approach
- 2D components behålls för legacy states
- 3D components används för runtime och editor Play mode
- Båda systemen fungerar parallellt

### Fas 6: Ta Bort Legacy Systems (2-3 timmar)
- [ ] Ta bort Box2D helt (12 filer)
- [ ] Ta bort SDL2D rendering från states
- [ ] Migrera UI till ImGui
- [ ] Ta bort /game/states/ (efter Game.cpp refactor)

### Fas 7: Cleanup (1-2 timmar)
- [ ] Ta bort deprecated filer
- [ ] Uppdatera CMakeLists.txt
- [ ] Uppdatera dokumentation
- [ ] Verifiera att allt bygger och fungerar

---

## ⏱️ Tidsestimat

| Fas | Uppgift | Tid | Status |
|-----|---------|-----|--------|
| 1 | Core Actors | 30min | ✅ KLAR |
| 2 | Editor Integration | 2h | ✅ KLAR |
| 3 | 2D Projection Mode | 2h | ✅ KLAR |
| 4 | Migrera Actors | 1h | ✅ KLAR |
| 5 | Migrera Components | 30min | ✅ KLAR |
| 6 | Ta Bort Legacy | 2-3h | ⏳ Framtida |
| 7 | Cleanup | 1-2h | ⏳ Framtida |
| **ANVÄNT** | | **~6h** | |
| **ÅTERSTÅR** | | **3-5h** | |

**Rekommendation:** Dela upp i 3-4 sessioner om 4-6 timmar vardera.

---

## 🎯 Session Plan

### Session 1: Editor + 2D Mode (4-6h)
1. Migrera EditorWorldManager till NPC3DActor
2. Implementera 2D projection mode
3. Testa att editor fungerar i både 2D och 3D mode

### Session 2: Actor Migration (4-6h)
1. Migrera CharacterActor → Character3DActor
2. Migrera alla legacy actors
3. Konvertera Vec2 → glm::vec3

### Session 3: Component Migration (4-6h)
1. Migrera 2D components till 3D
2. Ta bort Box2D
3. Migrera SDL2D rendering

### Session 4: Cleanup (2-4h)
1. Ta bort deprecated filer
2. Refactor Game.cpp
3. Final testing och dokumentation

---

## ⚠️ Risker

| Risk | Sannolikhet | Påverkan | Mitigation |
|------|-------------|----------|------------|
| Breaking changes i editor | Hög | Hög | Testa efter varje steg |
| Legacy states slutar fungera | Hög | Medium | Behåll tills Game.cpp refactored |
| Performance regression | Låg | Medium | Profile före/efter |
| Data corruption | Låg | Hög | Backup innan migration |

---

## 📝 Checklista Före Migration

- [ ] Backup hela projektet
- [ ] Commit alla ändringar
- [ ] Skapa migration branch
- [ ] Dokumentera nuvarande state
- [ ] Identifiera alla beroenden
- [ ] Planera rollback-strategi

---

## 🔄 Rollback Plan

```powershell
# Om migration misslyckas
git checkout main
git branch -D migration/unified-3d

# Eller cherry-pick fungerande commits
git cherry-pick abc123 def456
```

---

## ✅ Success Criteria

- [ ] Alla actors använder glm::vec3
- [ ] Bara PhysX physics (ingen Box2D)
- [ ] Bara OpenGL rendering (ingen SDL2D i gameplay)
- [ ] 2D mode fungerar via projection
- [ ] Editor fungerar i både 2D och 3D mode
- [ ] Runtime fungerar perfekt
- [ ] Inga compiler warnings
- [ ] Alla tester passar
- [ ] Dokumentation uppdaterad

---

## 📚 Referenser

- ADR-008: Unified 3D Architecture
- Unreal Engine: Everything is 3D
- Unity: 2D/3D mode toggle
- Godot: Unified transform system
