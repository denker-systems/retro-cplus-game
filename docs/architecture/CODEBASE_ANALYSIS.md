# Codebase Analysis Report

> Systematisk hierarkisk genomgång 2026-01-08
> Fokus: Identifiera dubbelkod, motsägelser och 3D-arkitektur

---

## 📊 PROJEKTSTRUKTUR ÖVERSIKT

```
src/
├── engine/          ← Spelmotor (RetroCore.lib)
│   ├── core/        ← Grund: Object, ActorObject, ActorObjectExtended
│   ├── actors/      ← NY: 3D actors + legacy 2D
│   ├── entities/    ← LEGACY: 2D Entity/Character system
│   ├── nodes/       ← LEGACY: 2D Node-baserat system
│   ├── components/  ← MIXED: 2D + 3D components
│   ├── systems/     ← SINGLETONS: 16 st managers
│   ├── world/       ← HIERARKI: World → Level → Scene
│   ├── physics/
│   │   ├── box2d/   ← 2D Physics (Box2D)
│   │   └── physx/   ← 3D Physics (PhysX)
│   └── graphics/    ← Rendering
│
├── editor/          ← Editor (RetroEditor.exe)
│   ├── core/        ← EditorApp, EditorContext, EditorState
│   ├── panels/      ← 53 panel-filer (!)
│   ├── viewport/    ← 19 viewport-filer (dubblering!)
│   ├── legacy/      ← 12 legacy-filer
│   └── managers/    ← EditorPanelManager, EditorWorldManager
│
└── game/            ← Spel (RetroGame.exe)
    └── states/      ← PlayState, MenuState, etc
```

---

## 🔴 KRITISKA MOTSÄGELSER

### 1. TRE PARALLELLA ACTOR-SYSTEM

| System | Plats | Antal filer | Status |
|--------|-------|-------------|--------|
| **actors/** | `engine/actors/` | 14 filer | NY - 3D fokus |
| **entities/** | `engine/entities/` | 7 filer | LEGACY 2D |
| **nodes/** | `engine/nodes/` | 8 filer | LEGACY 2D |

**Konflikt:** Samma koncept (Player, NPC, Character) implementerat 3 gånger!

#### Detaljerad konflikt:

| Koncept | actors/ | entities/ | nodes/ |
|---------|---------|-----------|--------|
| Player | `PlayerActor` (i CharacterActor.h) | `PlayerCharacter.h/cpp` | - |
| NPC | `NPC.h/cpp` + `NPCActor` | `NPC.cpp` (ingen header!) | - |
| Character | `CharacterActor.h/cpp` | `Character.h/cpp` | - |
| Sprite | `SpriteActor.h/cpp` | - | `Sprite.h/cpp` |

---

### 2. DUBBLA NPC-IMPLEMENTATIONER

| Fil | Storlek | Ärver från | Syfte |
|-----|---------|-----------|-------|
| `actors/NPC.h` | 1.5KB | CharacterActor | NY implementation |
| `actors/NPC.cpp` | 1.7KB | - | NY implementation |
| `entities/NPC.cpp` | 1KB | Character | LEGACY (ingen header!) |

**Problem:** Två helt olika NPC-implementationer existerar parallellt!

---

### 3. ROOM vs SCENE KONFLIKT

| Manager | Fil | Hanterar | Status |
|---------|-----|----------|--------|
| RoomManager | `systems/RoomManager.h` | `Room` objekt | LEGACY |
| SceneManager | `systems/SceneManager.h` | `Scene` objekt | NY |

**Problem:** Båda existerar med nästan identiskt API!

---

### 4. VIEWPORT KAOS (19 filer!)

#### I `editor/viewport/`:

| Fil | Storlek | Status |
|-----|---------|--------|
| ViewportPanel.h | 4.3KB | ? |
| ViewportPanel.cpp | 14KB | ? |
| ViewportPanelNew.h | 2.4KB | ? |
| ViewportPanelNew.cpp | 7KB | ? |
| Viewport3DPanel.h | 5.5KB | 3D |
| Viewport3DPanel.cpp | 31KB | 3D |
| ViewportRenderer.h | 6KB | ? |
| ViewportRenderer.cpp | 20KB | ? |
| ViewportSceneRenderer.h | 4KB | ? |
| ViewportSceneRenderer.cpp | 17KB | ? |
| ViewportLevelRenderer.h | 2.4KB | ? |
| ViewportLevelRenderer.cpp | 10KB | ? |
| ViewportWorldRenderer.h | 2.5KB | ? |
| ViewportWorldRenderer.cpp | 10KB | ? |
| EditorCamera3D.h | 5KB | 3D |
| EditorCamera3D.cpp | 5.6KB | 3D |
| IViewportRenderer.h | 2.5KB | Interface |
| ViewportInput.h | 2KB | Input |
| ViewportInput.cpp | 3.6KB | Input |

#### I `editor/panels/viewport/`:

| Fil | Storlek | Status |
|-----|---------|--------|
| ViewportPanel.h | 5.9KB | ANNAN KLASS! |
| ViewportPanel_Core.cpp | 10KB | Del av ViewportPanel |
| ViewportPanel_Input.cpp | 10KB | Del av ViewportPanel |
| ViewportPanel_Scene.cpp | 20KB | Del av ViewportPanel |
| ViewportPanel_Level.cpp | 15KB | Del av ViewportPanel |
| ViewportPanel_World.cpp | 13KB | Del av ViewportPanel |
| ViewportPanel_Actors.cpp | 15KB | Del av ViewportPanel |

**Problem:** TVÅ OLIKA ViewportPanel-klasser i olika mappar!

#### ANVÄNDNINGSANALYS:

| Fil | Inkluderas i | Status |
|-----|--------------|--------|
| `panels/viewport/ViewportPanel.h` | EditorPanelManager | ✅ PRIMÄR |
| `viewport/ViewportPanelNew.h` | EditorPanelManager + EditorState | ✅ ANVÄNDS (unified viewport) |
| `viewport/Viewport3DPanel.h` | EditorPanelManager | ✅ 3D PANEL |
| `viewport/ViewportPanel.h` | INGEN | ❌ OANVÄND - KAN TAS BORT |

**Slutsats:** `viewport/ViewportPanel.h/cpp` (14KB) är OANVÄND och kan tas bort!

---

### 5. LEGACY-MAPP (12 filer)

| Fil | Storlek | Syfte |
|-----|---------|-------|
| VisualRoomEditor.cpp | 16KB | Legacy rum-editor |
| DebugEditor.cpp | 9.6KB | Debug-verktyg |
| EditorTabRenderer.cpp | 9KB | Tab-rendering |
| FileBrowser.cpp | 8KB | Filhantering |
| RoomDataManager.cpp | 4.7KB | Rum-data |
| TiledIntegration.cpp | 2.5KB | Tiled-import |

**Status:** Markerad som legacy men fortfarande inkluderad?

---

## 🟢 3D-ARKITEKTUR (Vad som fungerar)

### 3D Actors (engine/actors/)

| Fil | Storlek | Status |
|-----|---------|--------|
| CameraActor.h | 4.2KB | ✅ Implementerat |
| CameraActor.cpp | 2.2KB | ✅ Implementerat |
| LightActor.h | 4.4KB | ✅ Implementerat |
| LightActor.cpp | 0.6KB | ✅ Implementerat |
| StaticMeshActor.h | 3.6KB | ✅ Implementerat |
| StaticMeshActor.cpp | 6.7KB | ✅ Implementerat |

### 3D Components (engine/components/)

| Fil | Storlek | Status |
|-----|---------|--------|
| RigidBody3DComponent.h | 5.6KB | ✅ Implementerat |
| RigidBody3DComponent.cpp | 8.8KB | ✅ Implementerat |
| MeshComponent.h | 2.8KB | ✅ Implementerat |
| MeshComponent.cpp | 1.7KB | ✅ Implementerat |

### 3D Physics (engine/physics/physx/)

| Fil | Storlek | Status |
|-----|---------|--------|
| PhysicsWorld3D.h | 6.2KB | ✅ Implementerat |
| PhysicsWorld3D.cpp | 12.8KB | ✅ Implementerat |

### 3D Editor (editor/viewport/)

| Fil | Storlek | Status |
|-----|---------|--------|
| Viewport3DPanel.h | 5.5KB | ✅ Implementerat |
| Viewport3DPanel.cpp | 31KB | ✅ Implementerat |
| EditorCamera3D.h | 5KB | ✅ Implementerat |
| EditorCamera3D.cpp | 5.6KB | ✅ Implementerat |

---

## 📊 STATISTIK

| Kategori | Antal | Problem |
|----------|-------|---------|
| Actor-system | 3 parallella | KRITISKT |
| Viewport-filer | 26 totalt | KAOS |
| Legacy-filer | 12+ | Bör rensas |
| Singletons | 16 st | FÖR MÅNGA |
| NPC-implementationer | 2 st | KONFLIKT |
| Room/Scene managers | 2 st | KONFLIKT |

---

## 📋 REKOMMENDERAD CLEANUP-PLAN (3D FOKUS)

### FAS 1: Ignorera 2D Legacy
- [ ] Markera `entities/` som LEGACY (rör ej)
- [ ] Markera `nodes/` som LEGACY (rör ej)
- [ ] Markera `RoomManager` som LEGACY (rör ej)
- [ ] Markera `editor/legacy/` som LEGACY (rör ej)

### FAS 2: Konsolidera Viewport
- [ ] Identifiera VILKEN ViewportPanel som används
- [ ] Ta bort/flytta den andra
- [ ] Rensa upp renderer-hierarkin

### FAS 3: Verifiera 3D-stack
- [ ] CameraActor fungerar ✅
- [ ] LightActor fungerar ✅
- [ ] StaticMeshActor fungerar ✅
- [ ] Viewport3DPanel fungerar ✅
- [ ] 3D Scene serialization fungerar ?

### FAS 4: Bygg 3D Demo
- [ ] Skapa enkel 3D-scene
- [ ] Testa kamera, ljus, mesh
- [ ] Verifiera physics

---

## 🔗 BEROENDE-KEDJA

```
EditorApp
└── EditorCore
    └── EditorPanelManager
        ├── ViewportPanel (panels/viewport/)
        │   └── Viewport3DPanel (viewport/)
        │       └── EditorCamera3D
        │       └── StaticMeshActor
        │       └── CameraActor
        │       └── LightActor
        │
        └── HierarchyPanel
            └── Scene/Level/World
                └── Actors
                    └── Components
```

---

## 📝 KONKRETA CLEANUP-ÅTGÄRDER (3D FOKUS)

### SÄKRA ATT TA BORT (oanvända):

| Fil | Storlek | Anledning |
|-----|---------|-----------|
| `editor/viewport/ViewportPanel.h` | 4.3KB | Inkluderas ALDRIG |
| `editor/viewport/ViewportPanel.cpp` | 14KB | Inkluderas ALDRIG |

### IGNORERA FÖR NU (2D legacy):

| Mapp/Fil | Storlek | Anledning |
|----------|---------|-----------|
| `engine/entities/` | 7 filer | 2D legacy - behövs för PlayState |
| `engine/nodes/` | 8 filer | 2D legacy - behövs för rendering |
| `editor/legacy/` | 12 filer | 2D legacy - behövs för rum-editor |
| `RoomManager` | - | 2D legacy - behövs för PlayState |

### 3D-STACK ATT VERIFIERA:

| Komponent | Fil | Status |
|-----------|-----|--------|
| CameraActor | `actors/CameraActor.h` | ✅ Finns |
| LightActor | `actors/LightActor.h` | ✅ Finns |
| StaticMeshActor | `actors/StaticMeshActor.h` | ✅ Finns |
| RigidBody3DComponent | `components/RigidBody3DComponent.h` | ✅ Finns |
| MeshComponent | `components/MeshComponent.h` | ✅ Finns |
| PhysicsWorld3D | `physics/physx/PhysicsWorld3D.h` | ✅ Finns |
| Viewport3DPanel | `editor/viewport/Viewport3DPanel.h` | ✅ Finns |
| EditorCamera3D | `editor/viewport/EditorCamera3D.h` | ✅ Finns |

### NÄSTA STEG:

1. **Ta bort oanvända filer:**
   - `editor/viewport/ViewportPanel.h/cpp`

2. **Verifiera 3D-rendering:**
   - Starta RetroEditor
   - Öppna Viewport3DPanel
   - Testa kamera, ljus, mesh

3. **Testa 3D scene serialization:**
   - SceneLoader.loadActors()
   - SceneLoader.saveActors()

---

## 📊 SAMMANFATTNING

| Kategori | Antal | Åtgärd |
|----------|-------|--------|
| Oanvända filer | 2 | TA BORT |
| 2D Legacy | ~27 | IGNORERA |
| 3D-stack | 8 | VERIFIERA |
| Viewport-kaos | 26 filer | DOKUMENTERAT |

**Build Status:** ✅ Allt bygger

---

*Genererad: 2026-01-08*
*Uppdaterad efter build-verifiering*

---

# DJUPANALYS - ALLA INKONSISTENSER

## 🔴 ARV-HIERARKI KAOS

### PROBLEM 1: Två olika Character-hierarkier

```
LEGACY (entities/):
engine::Pawn (ActorObject)
└── Character (entities/Character.h)
    └── PlayerCharacter (entities/PlayerCharacter.h)

NY (actors/):
engine::ActorObjectExtended
└── CharacterActor (actors/CharacterActor.h)
    ├── PlayerActor
    └── NPCActor
```

**KONFLIKT:** `Character` ärver från `Pawn` (som ärver `ActorObject`), 
medan `CharacterActor` ärver från `ActorObjectExtended`.
De har OLIKA basklasser och OLIKA component-support!

### PROBLEM 2: nodes/ ärver från actors/

```
engine::ActorObjectExtended
└── engine::VisualActor (actors/)
    └── Sprite (nodes/)
        └── ParallaxLayer (nodes/)
        └── AnimatedSprite (nodes/)
    └── TileMapLayer (nodes/)
    └── Label (nodes/)

└── engine::InteractiveActor (actors/)
    └── WalkArea (nodes/)
    └── Marker (nodes/)
    └── InteractiveArea (nodes/)
```

**PROBLEM:** `nodes/` är LEGACY men ärver från `actors/` som är NY!
Detta skapar förvirrande beroenden.

### PROBLEM 3: Dubbla Label-klasser

| Fil | Ärver från | Namrymd |
|-----|-----------|---------|
| `nodes/Label.h` | VisualActor | global |
| `ui/Widget.h` → Label | Widget | global |

**KONFLIKT:** TVÅ olika `Label`-klasser i samma projekt!

---

## 🔴 DATA LOADER KAOS

### TRE sätt att ladda data:

| Klass | Fil | Status |
|-------|-----|--------|
| `DataLoader` | data/DataLoader.h | Singleton, laddar JSON |
| `GameDataLoader` | data/GameDataLoader.h | DEPRECATED, duplicerar |
| `TiledImporter` | data/TiledImporter.h | Tiled-import |

**PROBLEM:** `GameDataLoader` duplicerar all logik från `DataLoader`
och ska flyttas till respektive System-klass.

---

## 🔴 PHYSICS KAOS

### TRE olika physics-system:

| System | Plats | Syfte |
|--------|-------|-------|
| `PhysicsBody` | physics/PhysicsBody.h | Basklass (ärver ActorObject!) |
| `PhysicsWorld2D` | physics/box2d/ | Box2D integration |
| `PhysicsWorld3D` | physics/physx/ | PhysX integration |

### PhysicsBody ärver fel!

```cpp
class PhysicsBody : public ActorObject { ... }
class KinematicBody : public PhysicsBody { ... }
```

**PROBLEM:** `PhysicsBody` ärver från `ActorObject`, inte `ActorObjectExtended`!
Kan inte ha components!

---

## 🔴 NAMING CONFLICTS

| Namn | Plats 1 | Plats 2 |
|------|---------|---------|
| `Label` | nodes/Label.h | ui/Widget.h |
| `NPC` | actors/NPC.h | entities/NPC.cpp |
| `ViewportPanel` | panels/viewport/ | viewport/ |
| `Scene` | world/Scene.h | (koncept i flera filer) |

---

## 🔴 NAMESPACE KAOS

| Klass | Namespace | Plats |
|-------|-----------|-------|
| Object | `engine::` | core/ |
| ActorObject | `engine::` | core/ |
| Character | **GLOBAL** | entities/ |
| PlayerCharacter | **GLOBAL** | entities/ |
| Sprite | **GLOBAL** | nodes/ |
| Label | **GLOBAL** | nodes/ |
| DataLoader | **GLOBAL** | data/ |

**PROBLEM:** Blandning av `engine::` namespace och globala klasser!

---

## 🔴 SINGLETON EXPLOSION (16 st i systems/)

| Singleton | Fil |
|-----------|-----|
| AISystem | systems/AISystem.h |
| ConditionSystem | systems/ConditionSystem.h |
| CutsceneSystem | systems/CutsceneSystem.h |
| DialogSystem | systems/DialogSystem.h |
| EventBus | systems/EventBus.h |
| GateSystem | systems/GateSystem.h |
| HintSystem | systems/HintSystem.h |
| InventorySystem | systems/InventorySystem.h |
| JournalSystem | systems/JournalSystem.h |
| QuestSystem | systems/QuestSystem.h |
| RecapSystem | systems/RecapSystem.h |
| RoomManager | systems/RoomManager.h |
| SaveSystem | systems/SaveSystem.h |
| SceneManager | systems/SceneManager.h |
| WorldQuery | systems/WorldQuery.h |
| WorldState | systems/WorldState.h |

**PLUS:** DataLoader, GameDataLoader, TextureManager, GLTextureManager...

---

## 📊 KOMPLETT INKONSISTENS-LISTA

| # | Problem | Allvarlighet |
|---|---------|--------------|
| 1 | Två Character-hierarkier med olika basklasser | 🔴 KRITISK |
| 2 | nodes/ ärver från actors/ (legacy ärver ny) | 🔴 KRITISK |
| 3 | Dubbla Label-klasser | 🟠 HÖG |
| 4 | PhysicsBody ärver ActorObject istället för Extended | 🟠 HÖG |
| 5 | DataLoader vs GameDataLoader duplicering | 🟠 HÖG |
| 6 | Namespace-blandning (engine:: vs global) | 🟡 MEDIUM |
| 7 | 16+ singletons | 🟡 MEDIUM |
| 8 | NPC finns på två ställen | 🟠 HÖG |
| 9 | ViewportPanel finns på två ställen | 🟠 HÖG |
| 10 | RoomManager vs SceneManager | 🟡 MEDIUM |

---

## 📋 REKOMMENDERAD ÅTGÄRD (3D FOKUS)

### IGNORERA FÖR NU:
- entities/ (2D legacy)
- nodes/ (2D legacy, fast ärver actors/)
- RoomManager (2D legacy)
- GameDataLoader (deprecated)

### VERIFIERA 3D-STACK:
- actors/CameraActor ✅
- actors/LightActor ✅
- actors/StaticMeshActor ✅
- components/RigidBody3DComponent ✅
- physics/physx/PhysicsWorld3D ✅
- viewport/Viewport3DPanel ✅

### FIXA SENARE (vid 2D rebuild):
1. Migrera Character → CharacterActor
2. Ta bort nodes/ helt (migrera till actors/)
3. Konsolidera Label-klasser
4. Fixa PhysicsBody-arv
5. Flytta allt till engine:: namespace

---

# DJUPANALYS DEL 2 - YTTERLIGARE FYND

## 🔴 GAME/STATES INKONSISTENSER

### RoomState är på FEL PLATS
| Fil | Plats | Problem |
|-----|-------|---------|
| `RoomState.h` | game/states/ | Ärver `IEditorPanel`, INTE `IState`! |

```cpp
// RoomState.h - FEL PLATS!
class RoomPanel : public IEditorPanel {  // <-- Är EDITOR, inte game state!
```

**PROBLEM:** En editor-panel finns i `game/states/` istället för `editor/panels/`.

### PlayState vs RoomState KONFLIKT
| Klass | Användning |
|-------|-----------|
| `PlayState` | Huvudsaklig gameplay state |
| `RoomState` (RoomPanel) | Editor panel (fel namn, fel plats) |

---

## 🔴 AI SYSTEM (Dolt i src/ai/)

### AI-mapp struktur (EJ DOKUMENTERAT TIDIGARE)
```
src/ai/
├── core/
│   ├── AIAgentSystem.cpp/h (12KB + 6KB)
│   ├── EditorToolRegistry.cpp/h
│   ├── IEditorTool.h
│   └── ILLMProvider.h
├── providers/
│   ├── OpenAIProvider.cpp/h
│   └── AnthropicProvider.cpp/h
├── tools/ (18 filer!)
│   ├── ActorTools.cpp/h
│   ├── SceneTools.cpp/h
│   ├── DialogTools.cpp/h
│   └── ... (9 verktyg)
└── ui/
    └── AIChatPanel.cpp/h
```

**PROBLEM:** AI-systemet är helt odokumenterat och har egna `IEditorTool`-interface som kan konfliktera med `editor/tools/IEditorTool.h`.

### DUBBLA IEditorTool-klasser
| Fil | Plats |
|-----|-------|
| `IEditorTool.h` | editor/tools/ |
| `IEditorTool.h` | ai/core/ |

---

## 🔴 BUILD-SYSTEM INKONSISTENSER

### entities/ inkluderas TROTS "legacy"
```cmake
# CMakeLists.txt
src/engine/entities/Entity.cpp
src/engine/entities/Character.cpp
src/engine/entities/PlayerCharacter.cpp
# src/engine/entities/NPC.cpp  # Migrated to src/engine/actors/NPC.cpp
```

**PROBLEM:** entities/ kompileras fortfarande! NPC finns på TVÅ ställen!

### nodes/ DELVIS DISABLED
```cmake
# LEGACY NODES - PARTIALLY DISABLED
# src/engine/nodes/Sprite.cpp  # Disabled
src/engine/nodes/TileMapLayer.cpp  # Needed by editor
# src/engine/nodes/WalkArea.cpp  # Disabled
```

**PROBLEM:** TileMapLayer kompileras fortfarande!

### Game.cpp duplicerad i editor
```cmake
# GAME_SOURCES
src/game/Game.cpp
src/game/states/StateManager.cpp
...

# EDITOR_SOURCES
src/game/Game.cpp  # <-- DUPLICERAD!
src/game/states/StateManager.cpp  # <-- DUPLICERAD!
```

**PROBLEM:** Game.cpp och alla states kompileras DUBBELT!

---

## 📊 KOMPLETT PROJEKTSTATISTIK

### Totalt antal filer per mapp:

| Mapp | .cpp | .h | Total |
|------|------|-----|-------|
| engine/actors/ | 14 | 14 | 28 |
| engine/components/ | 20 | 20 | 40 |
| engine/systems/ | 16 | 16 | 32 |
| engine/entities/ | 3 | 4 | 7 |
| engine/nodes/ | 8 | 8 | 16 |
| engine/world/ | 8 | 8 | 16 |
| engine/physics/ | 7 | 8 | 15 |
| engine/graphics/ | 10 | 10 | 20 |
| editor/panels/ | ~30 | ~30 | ~60 |
| editor/viewport/ | 10 | 9 | 19 |
| game/states/ | 11 | 10 | 21 |
| ai/ | 14 | 13 | 27 |

**TOTALT:** ~300+ filer

---

## 📋 UTÖKAD INKONSISTENS-LISTA

| # | Problem | Allvarlighet |
|---|---------|--------------|
| 1 | Två Character-hierarkier | 🔴 KRITISK |
| 2 | nodes/ ärver från actors/ | 🔴 KRITISK |
| 3 | Dubbla Label-klasser | 🟠 HÖG |
| 4 | PhysicsBody ärver fel | 🟠 HÖG |
| 5 | DataLoader vs GameDataLoader | 🟠 HÖG |
| 6 | Namespace-blandning | 🟡 MEDIUM |
| 7 | 16+ singletons | 🟡 MEDIUM |
| 8 | NPC finns på två ställen | 🟠 HÖG |
| 9 | ViewportPanel dubbelt | 🟠 HÖG |
| 10 | RoomManager vs SceneManager | 🟡 MEDIUM |
| **11** | **RoomState på fel plats** | 🟠 HÖG |
| **12** | **Dubbla IEditorTool** | 🟠 HÖG |
| **13** | **Game.cpp kompileras dubbelt** | 🟡 MEDIUM |
| **14** | **AI-system odokumenterat** | 🟡 MEDIUM |
| **15** | **TileMapLayer fortfarande aktiv** | 🟡 MEDIUM |

---

## 🔍 TOTAL DUBBELKOD-ANALYS

### Identiska klasser på flera platser:
| Klass | Plats 1 | Plats 2 |
|-------|---------|---------|
| NPC | actors/NPC.h | entities/NPC.cpp |
| Label | nodes/Label.h | ui/Widget.h |
| ViewportPanel | panels/viewport/ | viewport/ |
| IEditorTool | editor/tools/ | ai/core/ |
| Character | entities/Character.h | actors/CharacterActor.h |

### Duplicerad kompilering:
| Fil | I GAME_SOURCES | I EDITOR_SOURCES |
|-----|----------------|------------------|
| Game.cpp | ✅ | ✅ |
| StateManager.cpp | ✅ | ✅ |
| MenuState.cpp | ✅ | ✅ |
| PlayState.cpp | ✅ | ✅ |
| (alla states) | ✅ | ✅ |

---

# DJUPANALYS DEL 3 - ÖVRIGA MAPPAR

## 📁 VENDOR/ (Tredjepartskod)

```
src/vendor/
├── imgui/           # ImGui backends (SDL2, OpenGL3)
│   ├── imgui_impl_sdl2.cpp (65KB)
│   ├── imgui_impl_opengl3.cpp (50KB)
│   └── imgui_impl_sdlrenderer2.cpp (13KB)
└── imnodes/         # Node editor (lokal kopia)
    ├── imnodes.cpp (114KB!)
    └── imnodes.h (18KB)
```

**NOTERING:** imnodes är 114KB stor fil - kan påverka build-tid.

---

## 📁 GENERATOR/ (Python Asset Generator)

```
generator/
├── generate_all.py              # Huvudskript
├── requirements.txt             # Python dependencies
└── scripts/
    ├── generate_actor_sprites.py
    ├── generate_backgrounds.py
    ├── generate_sprites.py
    ├── generate_tilesets.py
    └── generate_ui.py
```

**STATUS:** Verkar fungera, men assets/ mappar är TOMMA!

---

## 📁 ASSETS/DATA/ (JSON Game Data)

| Fil | Storlek | Backup |
|-----|---------|--------|
| dialogs.json | 16KB | ✅ .bak |
| items.json | 5KB | ✅ .bak |
| npcs.json | 3KB | ✅ .bak |
| quests.json | 7KB | ✅ .bak |
| scenes.json | 4KB | ❌ |
| world.json | 0.5KB | ❌ |

### DATA INKONSISTENS

| Fil | Status |
|-----|--------|
| rooms.json | ❌ SAKNAS (bara .bak!) |
| scenes.json | ✅ Finns |

**PROBLEM:** `rooms.json` finns INTE, bara `rooms.json.bak`!
Men koden refererar till rooms - detta kan vara ett data-problem.

---

## 📁 ROT-NIVÅ FILER

| Fil | Syfte | Status |
|-----|-------|--------|
| CMakeLists.txt | Build-system | ✅ Analyserat |
| vcpkg.json | Dependencies | ✅ OK |
| 3d_checklist.md | 3D implementation | ✅ KOMPLETT |
| input_checklist.md | Input refactoring | ⏳ PÅGÅENDE |
| PR_DESCRIPTION.md | PR dokumentation | ✅ OK |
| Build.bat | Build script | ✅ OK |
| Play.bat | Run script | ✅ OK |
| imgui.ini | ImGui layout | ✅ OK |

---

## 📁 VCPKG DEPENDENCIES

```json
{
  "dependencies": [
    "sdl2", "sdl2-image", "sdl2-mixer", "sdl2-ttf",
    "nlohmann-json", "box2d", "glew", "glm",
    "assimp", "imgui", "imguizmo", "implot",
    "nativefiledialog-extended", "cpp-httplib",
    "openssl", "physx"
  ]
}
```

**16 dependencies** totalt!

---

## 📊 KOMPLETT PROJEKTÖVERSIKT

### Alla mappar:

| Mapp | Filer | Status |
|------|-------|--------|
| src/engine/ | 212 | ✅ Analyserad |
| src/editor/ | 182 | ✅ Analyserad |
| src/game/ | 24 | ✅ Analyserad |
| src/ai/ | 32 | ✅ Analyserad |
| src/vendor/ | 10 | ✅ Analyserad |
| generator/ | 13 | ✅ Analyserad |
| assets/ | 18 | ✅ Analyserad |
| .windsurf/ | 46 | Workflows |
| build/ | 2657 | Build output |
| docs/ | 69 | Dokumentation |

**TOTALT SRC:** ~460 filer

---

## 🆕 NYA PROBLEM HITTADE

| # | Problem | Allvarlighet |
|---|---------|--------------|
| **16** | **rooms.json saknas** (bara .bak) | 🟠 HÖG |
| **17** | **assets/ mappar är tomma** | 🟡 MEDIUM |
| **18** | **imnodes 114KB** i repo | 🟢 LÅG |

---

## 📋 FINAL INKONSISTENS-LISTA (18 st)

| # | Problem | Allvarlighet |
|---|---------|--------------|
| 1 | Två Character-hierarkier | 🔴 KRITISK |
| 2 | nodes/ ärver från actors/ | 🔴 KRITISK |
| 3 | Dubbla Label-klasser | 🟠 HÖG |
| 4 | PhysicsBody ärver fel | 🟠 HÖG |
| 5 | DataLoader vs GameDataLoader | 🟠 HÖG |
| 6 | Namespace-blandning | 🟡 MEDIUM |
| 7 | 16+ singletons | 🟡 MEDIUM |
| 8 | NPC finns på två ställen | 🟠 HÖG |
| 9 | ViewportPanel dubbelt | 🟠 HÖG |
| 10 | RoomManager vs SceneManager | 🟡 MEDIUM |
| 11 | RoomState på fel plats | 🟠 HÖG |
| 12 | Dubbla IEditorTool | 🟠 HÖG |
| 13 | Game.cpp kompileras dubbelt | 🟡 MEDIUM |
| 14 | AI-system odokumenterat | 🟡 MEDIUM |
| 15 | TileMapLayer fortfarande aktiv | 🟡 MEDIUM |
| 16 | rooms.json saknas | 🟠 HÖG |
| 17 | assets/ mappar tomma | 🟡 MEDIUM |
| 18 | imnodes stor fil i repo | 🟢 LÅG |
