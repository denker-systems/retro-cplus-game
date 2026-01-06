# Actor System Integration Checklist

## 📋 Översikt
Denna checklista täcker allt som behöver göras för att migrera från det gamla Node/Entity-systemet till det nya Actor/Component-systemet.

---

## 🎯 Fas 5D: Full Migration

### 1. Core System Integration
- [x] **PlayState migrering**
  - [x] Byt `PlayerCharacter` → `PlayerActor`
  - [x] Byt `NPC` → `NPCActor`
  - [x] Byt `Room` → `Scene` (redan klart)
  - [x] Uppdatera includes från `entities/` till `actors/`
  - [x] Migrera player spawn till Actor position

- [ ] **Game Loop Integration**
  - [x] Spel-loopen uppdaterar Actors från Scene
  - [x] Rendering via Components (SpriteComponent)
  - [ ] Ta bort legacy rendering code
  - [x] Delta-time propagation till Actors

### 2. Component System Integration
- [x] **SpriteComponent**
  - [x] Alla spelobjekt använder SpriteComponent
  - [x] TextureManager integration med SpriteComponent
  - [ ] Origin/pivot fungerar för alla sprites
  - [ ] Tint/opacity fungerar i gameplay

- [x] **MovementComponent**
  - [x] Point-and-click movement
  - [ ] Pathfinding integration
  - [x] Walk area clamping
  - [x] Speed/velocity calculations

- [x] **DialogComponent**
  - [x] NPC dialogs trigger
  - [ ] Dialog tree integration
  - [ ] Villkorliga dialogval
  - [ ] DialogSystem koppling

- [x] **InventoryComponent**
  - [x] Item pickup via Component
  - [ ] Inventory UI integration
  - [ ] Item combine/use
  - [ ] InventorySystem koppling

- [x] **InteractionComponent**
  - [x] Hotspot interactions
  - [x] Click detection
  - [ ] Interaction text/feedback
  - [ ] Villkorliga interaktioner

### 3. Actor Hierarchy Implementation
- [ ] **CharacterActor**
  - [ ] PlayerActor implementation klar
  - [ ] NPCActor implementation klar
  - [ ] Health/stamina system
  - [ ] Animation states (idle, walk, talk)

- [ ] **EnvironmentActor**
  - [ ] PropActor för statiska objekt
  - [ ] TileMapActor för grid-baserade banor
  - [ ] ParallaxActor för bakgrunder
  - [ ] Collision integration

- [ ] **InteractiveActor**
  - [ ] Hotspot actors
  - [ ] Trigger areas
  - [ ] Door/exits
  - [ ] Pickable items

### 4. Data Loading Integration
- [x] **GameDataLoader**
  - [x] Ladda Scenes istället för Rooms
  - [ ] Skapa Actors från JSON
  - [ ] Component data från JSON
  - [x] Spawn positions från JSON

- [x] **RoomToSceneConverter**
  - [x] Konvertera alla RoomData
  - [x] Skapa proper Actors
  - [x] Sätt component properties
  - [x] Ta bort hybrid rendering

### 5. Editor Integration
- [x] **ViewportPanel**
  - [x] Rendera Actors direkt
  - [x] Ta bort hybrid-lösning
  - [x] Actor selection/editing
  - [ ] Component property editing

- [ ] **SceneGraphPanel**
  - [ ] Visa Actor-hierarki
  - [ ] Component lista per Actor
  - [ ] Drag-and-drop Actors
  - [ ] Add/Remove Components

- [ ] **PropertyEditor**
  - [ ] Edit Actor properties
  - [ ] Edit Component values
  - [ ] Real-time preview
  - [ ] Undo/redo support

### 6. System Integration
- [ ] **Input System**
  - [ ] Input till Actors
  - [ ] Mouse click detection
  - [ ] Keyboard input routing
  - [ ] Component input handling

- [ ] **Audio System**
  - [ ] AudioComponent för Actors
  - [ ] Footstep sounds
  - [ ] Ambient sounds per Scene
  - [ ] Dialog audio

- [ ] **Save System**
  - [ ] Spara Actor states
  - [ ] Component serialization
  - [ ] Scene state persistence
  - [ ] Load/restore Actors

### 7. Legacy Cleanup
- [x] **Byt namn Room → Scene**
  - [x] Skapa Scene.h/cpp (parallellt med Room)
  - [x] Skapa SceneManager (parallellt med RoomManager)
  - [x] Uppdatera PlayState för SceneManager
  - [ ] Ta bort dubbel laddning (Room + Scene)
  
- [ ] **Ta bort gamla klasser**
  - [ ] `src/engine/entities/` hela mappen
  - [ ] Node/Node2D klasser
  - [ ] Legacy Sprite/AnimatedSprite
  - [ ] Gammal rendering code

- [x] **Uppdatera alla includes**
  - [x] Byt `entities/NPC.h` → `actors/NPC.h`
  - [x] NPC i namespace `engine::actors`
  - [ ] Byt `Node` → `ActorObject`
  - [ ] Byt `Sprite` → `SpriteComponent`

- [ ] **CMakeLists.txt**
  - [ ] Ta bort entities/ från sources
  - [ ] Lägg till actors/ om saknas
  - [ ] Bort gamla dependencies
  - [ ] Lägg till nya dependencies

### 8. Testing & Validation
- [ ] **Unit Tests**
  - [ ] Actor creation/destruction
  - [ ] Component functionality
  - [ ] Scene management
  - [ ] Data loading/saving

- [ ] **Integration Tests**
  - [ ] Spel-loop med Actors
  - [ ] Editor med Actors
  - [ ] Save/load med Actors
  - [ ] Performance tests

- [ ] **Manual Testing**
  - [ ] Spela igenom hela spelet
  - [ ] Testa alla interaktioner
  - [ ] Verifiera dialoger
  - [ ] Testa inventory

---

## 🚨 Kritiska Dependencies

### Måste göras i ordning:
1. **PlayState migrering** (spel måste använda Actors)
2. **Component integration** (funktion måste fungera)
3. **Editor rendering** (visa Actors korrekt)
4. **Legacy cleanup** (ta bort gamla system)

### Kan göras parallellt:
- Component implementation
- Actor specialiseringar
- Data loading
- Editor panels

---

## 📊 Progress Tracking

### Status per område:
```
Core System        ████████████░░  85%
Components         ████░░░░░░░░░░  30%
Actor Hierarchy    ██░░░░░░░░░░░░  15%
Data Loading       ████████░░░░░░  60%
Editor Integration █░░░░░░░░░░░░░  5%
System Integration ░░░░░░░░░░░░░░  0%
Legacy Cleanup     ██████░░░░░░░░  40%
Testing            ░░░░░░░░░░░░░░  0%
```

---

## 🎯 Nästa Steg (Prioriterad)

1. **Ta bort dubbel laddning**
   - GameDataLoader laddar nu till både Room och Scene
   - Ta bort Room-laddning, behåll endast Scene

2. **Ta bort entities/NPC.h**
   - Legacy NPC-klass ersatt av actors/NPC.h
   - Uppdatera alla filer som fortfarande refererar till entities/

3. **Rensa CMakeLists.txt**
   - Ta bort entities/ från CORE_SOURCES
   - Verifiera att alla actors/ filer inkluderas

4. **Testa alla interaktioner**
   - Dialog med NPCs
   - Item pickup
   - Room transitions

---

## 📝 Anteckningar

- **Kom ihåg:** Components är nyckeln - allt ska gå via components
- **Tänk UE5:** AActor + UActorComponent pattern
- **Minimal implementation:** Börja med grundfunktionen, utöka sen
- **Testa ofta:** Bygg och kör efter varje större ändring

---

*Senast uppdaterad: 2026-01-06*
