# Retro Adventure - Visuell Arkitektur

---

## 🌳 Komplett OOP-Hierarki

```
                                    Object (Universal bas)
                                       |
        +------------------------------+--------------------------------+
        |                              |                                |
   ActorObject                   ActorComponent                   WorldContext
   (i världen)                   (beteenden)                      (containers)
        |                              |                                |
        |                              |                                |
   +----+----+                    +----+----+                      +----+----+
   |    |    |                    |    |    |                      |    |    |
 Pawn Ctrl Items               Sprite Move Health               World Lvl Scene
   |
   |
Character
   |
   +----+
   |    |
Player NPC
```

---

## 📦 ActorObject Hierarki

```
ActorObject
│
├── Pawn (Kontrollerbar)
│   └── Character (Humanoid)
│       ├── Player
│       └── NPC
│
├── Controller (Kontrollerar Pawns)
│   ├── PlayerController (Input)
│   └── AIController (AI-logik)
│
├── VisualActor (Synliga)
│   ├── Sprite
│   ├── AnimatedSprite
│   └── Label
│
├── InteractiveActor (Interaktiva)
│   ├── InteractiveArea (Hotspots)
│   ├── WalkArea
│   └── Marker
│
└── ItemActor (Items)
    └── InventoryItem
        ├── Weapon
        └── Consumable
```

---

## 🔧 ActorComponent Hierarki

```
ActorComponent
│
├── SceneComponent (med transform)
│
├── Visual Components
│   ├── SpriteComponent
│   └── AnimationComponent
│
├── Gameplay Components
│   ├── MovementComponent
│   ├── HealthComponent
│   ├── CollisionComponent
│   └── InteractionComponent
│
├── Dialog & Quest
│   ├── DialogComponent
│   ├── QuestComponent
│   ├── QuestGiverComponent
│   └── InventoryComponent
│
└── AI Components
    ├── PathfindingComponent
    └── PerceptionComponent
```

---

## 🌍 World-Level-Scene Struktur

```
World (Hela spelet)
│
├── Level (Chapter 1)
│   ├── Scene (Tavern)
│   ├── Scene (Town Square)
│   └── Scene (Shop)
│
├── Level (Chapter 2)
│   ├── Scene (Forest)
│   └── Scene (Cave)
│
└── Level (Chapter 3)
    └── ...
```

---

## 🎮 Actor med Components

```
ActorObjectExtended
│
├── Component: SpriteComponent
│   └── renderar sprite
│
├── Component: HealthComponent
│   └── hanterar HP
│
├── Component: MovementComponent
│   └── hanterar rörelse
│
└── Component: CollisionComponent
    └── hanterar kollision
```

---

## 🎯 Pawn-Controller System

```
PlayerController ──possess──> Player Pawn
     │                             │
     │                             │
  Input från                   Utför
  keyboard/mouse               actions
     │                             │
     └──moveForward()─────────────>│
     └──moveRight()───────────────>│
     └──interact()────────────────>│


AIController ──possess──> NPC Pawn
     │                        │
     │                        │
  AI Logic              Utför actions
  Behaviors                  │
     │                        │
     └──wander()─────────────>│
     └──follow()─────────────>│
```

---

## 🔄 Game Loop

```
Game Loop
    │
    ├─> World::update(deltaTime)
    │      │
    │      ├─> Level::update(deltaTime)
    │      │      │
    │      │      └─> Scene::update(deltaTime)
    │      │             │
    │      │             ├─> Actor::update(deltaTime)
    │      │             │      │
    │      │             │      └─> Component::update(deltaTime)
    │      │             │
    │      │             └─> Legacy Nodes::update(deltaTime)
    │      │
    │      └─> [repeat för alla levels]
    │
    └─> World::render(renderer)
           │
           └─> [samma struktur som update]
```

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────┐
│           RetroCore.lib (ENGINE)                │
│                                                  │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │  Core    │  │  Actors  │  │Components│     │
│  │ Classes  │  │  Pawn    │  │  Sprite  │     │
│  │          │  │Controller│  │  Health  │     │
│  └──────────┘  └──────────┘  └──────────┘     │
│                                                  │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
│  │ Systems  │  │ Managers │  │  World   │     │
│  │ Dialog   │  │ Texture  │  │  Level   │     │
│  │ Quest    │  │  Audio   │  │  Scene   │     │
│  └──────────┘  └──────────┘  └──────────┘     │
└─────────────────────────────────────────────────┘
                    ▲         ▲
                    │         │
          ┌─────────┘         └─────────┐
          │                              │
┌─────────────────────┐    ┌─────────────────────┐
│ RetroGame.exe       │    │ RetroEditor.exe     │
│                     │    │                     │
│ ┌─────────────┐    │    │ ┌─────────────┐    │
│ │ Game Loop   │    │    │ │ ImGui       │    │
│ │ States      │    │    │ │ Panels      │    │
│ │ Content     │    │    │ │ Tools       │    │
│ └─────────────┘    │    │ └─────────────┘    │
└─────────────────────┘    └─────────────────────┘
```

---

## 🔄 Data Flow: JSON → Runtime

```
JSON Files                DataLoader              Runtime Objects
    │                          │                         │
    ├─ rooms.json ────────────>├─> RoomData ──────────> Scene
    │                          │                         │
    ├─ dialogs.json ──────────>├─> DialogData ────────> DialogSystem
    │                          │                         │
    ├─ quests.json ───────────>├─> QuestData ─────────> QuestSystem
    │                          │                         │
    └─ items.json ────────────>└─> ItemData ──────────> ItemActor
```

---

## 🎯 Skapa NPC (Exempel)

```
1. new ActorObjectExtended("Tavern Keeper")
        │
        ├─> addComponent<SpriteComponent>()
        │        └─> setTexture("tavern_keeper.png")
        │
        ├─> addComponent<HealthComponent>()
        │        └─> setMaxHealth(100)
        │
        ├─> addComponent<DialogComponent>()
        │        └─> setDialogId("tavern_keeper_dialog")
        │
        └─> addComponent<QuestGiverComponent>()
                 └─> addQuest("main_quest")

2. new AIController()
        │
        └─> possess(npc_actor)
                 └─> setBehavior(AIBehaviorType::Idle)

3. scene->addActor(npc_actor)
        │
        └─> NPC är nu i världen och kan interageras med
```

---

## 🔄 Legacy vs New System

```
LEGACY (Phase Out)              NEW (UE5-style)
    │                               │
    │                               │
  Node ────────────────────────> Object
    │                 alias         │
    │                               │
  Node2D ──────────────────────> ActorObject
    │                 alias         │
    │                               │
  Sprite ───────────X────────────> ActorObject + SpriteComponent
  (inheritance)              (composition)
```

---

## 📊 Component Lifecycle

```
Actor skapad
    │
    ├─> addComponent<HealthComponent>()
    │        │
    │        └─> Component::initialize()
    │                │
    │                └─> Setup component state
    │
    └─> Game Loop börjar
            │
            ├─> varje frame: Component::update(deltaTime)
            │        │
            │        └─> Update logic
            │
            ├─> varje frame: Component::render(renderer)
            │        │
            │        └─> Render visuals
            │
            └─> Actor förstörs
                    │
                    └─> Component::shutdown()
                            │
                            └─> Cleanup
```

---

## 🎮 Gameplay Flow

```
Player ────> PlayerController ────> Player Pawn ────> World
   │               │                    │               │
   │               │                    │               │
Press W      moveForward()         update pos      collision?
   │               │                    │               │
   │               │                    │               │
Press E       interact()          check nearby     find NPC
   │               │                    │               │
   │               │                    │               │
   └──────────> NPC found ──────> DialogComponent ──> DialogSystem
                                         │               │
                                         │               │
                                    start dialog    show UI
                                         │               │
                                         │               │
                                  select option   update quest
```

---

## 🏛️ Architecture Layers

```
┌───────────────────────────────────────────────────┐
│  LAYER 4: Game Specific                           │
│  Player, NPCs, TavernKeeper, MagicDoor            │
└───────────────────────────────────────────────────┘
                        │
┌───────────────────────────────────────────────────┐
│  LAYER 3: Specialized Types                       │
│  Pawn, Character, Controller, Items               │
└───────────────────────────────────────────────────┘
                        │
┌───────────────────────────────────────────────────┐
│  LAYER 2: Core Types                              │
│  ActorObject, ActorComponent, WorldContext        │
└───────────────────────────────────────────────────┘
                        │
┌───────────────────────────────────────────────────┐
│  LAYER 1: Foundation                              │
│  Object (Universal bas)                           │
└───────────────────────────────────────────────────┘
```

---

## ✅ Implementation Status

```
COMPLETED ✅
│
├── Core Classes (Object, ActorObject, Component)
├── Actor Hierarchy (Pawn, Controller, Items, Visual, Interactive)
├── All Components (15 st: Sprite, Movement, Health, Dialog, etc)
├── World/Level/Scene structure
├── Pawn + Controller system
└── Migration strategy (Strangler Fig)

ALREADY EXISTS ✅
│
├── DialogSystem
├── QuestSystem
├── InventorySystem
├── AISystem
├── SaveSystem
└── All Managers (Texture, Audio, Input, Font)
```

---

## 🎯 Key Concepts

**Composition over Inheritance**
```
Actor + Components = Flexibel funktionalitet
vs
Djup arv-hierarki = Stel struktur
```

**Separation of Concerns**
```
Pawn       = Fysisk representation (kropp)
Controller = Logik och input (hjärna)
Component  = Specifikt beteende (organ)
```

**World Organization**
```
World → Level → Scene → Actors
(Game)  (Chapter) (Room)  (Objects)
```
