# Final OOP-Hierarki - UE5-Inspirerad

## 🌟 KOMPLETT HIERARKI (UE5-stil)

```
Object (Universal bas - som UObject)
│
├─ ActorObject (Objekt i världen - som AActor)
│  │
│  ├─ Pawn (Kontrollerbar entity - som APawn)
│  │  ├─ Character (Humanoid - som ACharacter)
│  │  │  ├─ Player
│  │  │  └─ NPC
│  │  └─ Vehicle (Framtida)
│  │
│  ├─ Controller (Kontrollerar Pawns - som AController)
│  │  ├─ PlayerController (som APlayerController)
│  │  └─ AIController (som AAIController)
│  │
│  ├─ VisualActor (Synliga objekt)
│  │  ├─ Sprite
│  │  ├─ AnimatedSprite
│  │  ├─ Label
│  │  ├─ TileMapLayer
│  │  └─ ParallaxLayer
│  │
│  ├─ InteractiveActor (Interaktiva objekt)
│  │  ├─ InteractiveArea (Hotspots)
│  │  ├─ WalkArea
│  │  └─ Marker
│  │
│  ├─ ItemActor (Plockas upp, används)
│  │  ├─ InventoryItem
│  │  ├─ Weapon
│  │  └─ Consumable
│  │
│  └─ CameraActor (Kamera - som ACameraActor)
│
├─ ActorComponent (Återanvändbar funktionalitet - som UActorComponent)
│  │
│  ├─ SceneComponent (Med transform - som USceneComponent)
│  │  ├─ SpriteComponent
│  │  ├─ MeshComponent
│  │  └─ CameraComponent
│  │
│  ├─ VisualComponent (Rendering)
│  │  ├─ AnimationComponent
│  │  └─ ParticleComponent
│  │
│  ├─ GameplayComponent (Spellogik)
│  │  ├─ MovementComponent (som UCharacterMovementComponent)
│  │  ├─ CollisionComponent
│  │  ├─ HealthComponent
│  │  └─ InteractionComponent
│  │
│  ├─ AIComponent (AI-beteende)
│  │  ├─ BehaviorComponent
│  │  ├─ PathfindingComponent
│  │  └─ PerceptionComponent
│  │
│  └─ GameComponent (Game-specifik)
│     ├─ DialogComponent
│     ├─ QuestGiverComponent
│     └─ InventoryComponent
│
├─ WorldContext (Världsorganisation)
│  ├─ World (Hela spelet - som UWorld)
│  ├─ Level (Kapitel/akt - som ULevel)
│  └─ Scene (Ett rum/location)
│
├─ GameSubsystem (Globala system - som UGameInstanceSubsystem)
│  │
│  ├─ GameplaySubsystem
│  │  ├─ DialogSystem
│  │  ├─ QuestSystem
│  │  ├─ InventorySystem
│  │  └─ SaveSystem
│  │
│  ├─ AISubsystem
│  │  ├─ AISystem
│  │  ├─ PathfindingSystem
│  │  └─ PerceptionSystem
│  │
│  ├─ StateSubsystem
│  │  ├─ WorldState
│  │  ├─ ConditionSystem
│  │  └─ EventBus
│  │
│  └─ SpecialSubsystem
│     ├─ CutsceneSystem
│     ├─ HintSystem
│     ├─ JournalSystem
│     └─ RecapSystem
│
├─ GameSingleton (Managers - som Singletons)
│  ├─ TextureManager
│  ├─ FontManager
│  ├─ AudioManager
│  └─ InputManager
│
└─ DataAsset (POD - Plain Old Data)
   ├─ GameData
   │  ├─ RoomData
   │  ├─ DialogData
   │  ├─ QuestData
   │  ├─ ItemData
   │  └─ NPCData
   │
   └─ MathTypes
      ├─ Vec2
      ├─ Rect
      ├─ Color
      └─ Transform
```

---

## 📋 NAMNKONVENTIONER (UE5-stil)

### Prefix-system
```cpp
// Object-typer
class Object { };           // Bas för allt (som UObject)

// Actor-typer (A-prefix som UE5)
class ActorObject { };      // Som AActor
class Pawn { };             // Som APawn
class Character { };        // Som ACharacter
class Player { };           // Som APlayerCharacter
class Controller { };       // Som AController
class PlayerController { }; // Som APlayerController
class AIController { };     // Som AAIController

// Component-typer (Component-suffix som UE5)
class ActorComponent { };         // Som UActorComponent
class SceneComponent { };         // Som USceneComponent
class SpriteComponent { };        // Som UStaticMeshComponent
class MovementComponent { };      // Som UCharacterMovementComponent
class AnimationComponent { };     // Custom component

// Subsystem-typer (System-suffix)
class GameSubsystem { };    // Som UGameInstanceSubsystem
class DialogSystem { };     // Custom subsystem
class QuestSystem { };      // Custom subsystem

// Singleton-typer (Manager-suffix)
class GameSingleton { };    // Bas för singletons
class TextureManager { };   // Singleton manager
class AudioManager { };     // Singleton manager

// Data-typer (Data-suffix)
class DataAsset { };        // Bas för data
class RoomData { };         // POD från JSON
class DialogData { };       // POD från JSON
```

---

## 🎯 RELATIONER (UE5-stil)

```
ActorObject
├─ owns ActorComponents (composition)
├─ has Transform (via RootComponent)
└─ can be placed in Scene

Pawn
├─ is an ActorObject
├─ can be possessed by Controller
└─ receives input from Controller

Controller
├─ is an ActorObject
├─ possesses one Pawn
└─ sends input to Pawn

Scene
├─ contains ActorObjects
└─ is part of Level

Level
├─ contains Scenes
└─ is part of World

World
├─ contains Levels
└─ is singleton (one per game)

GameSubsystem
├─ is global service
└─ accessed via World or GameInstance
```

---

## 💡 EXEMPEL PÅ ANVÄNDNING

### Skapa Player med Components
```cpp
// Skapa player actor
auto player = std::make_unique<Player>("MainPlayer");

// Lägg till components (UE5-stil)
player->addComponent<SpriteComponent>("player_sprite");
player->addComponent<MovementComponent>();
player->addComponent<HealthComponent>();
player->addComponent<InventoryComponent>();

// Skapa controller och possess
auto controller = std::make_unique<PlayerController>();
controller->possess(player.get());

// Placera i scene
scene->addActor(std::move(player));
```

### Skapa NPC med AI
```cpp
// Skapa NPC actor
auto npc = std::make_unique<NPC>("Bartender");
npc->setPosition({300, 250});

// Lägg till components
npc->addComponent<SpriteComponent>("bartender_sprite");
npc->addComponent<AnimationComponent>();
npc->addComponent<DialogComponent>("bartender_dialog");
npc->addComponent<BehaviorComponent>(AIBehaviorType::Wander);

// Skapa AI controller
auto aiController = std::make_unique<AIController>();
aiController->possess(npc.get());

// Placera i scene
scene->addActor(std::move(npc));
```

### Använda Systems (Subsystems)
```cpp
// Få tillgång till subsystems via World
auto* dialogSystem = world->getSubsystem<DialogSystem>();
dialogSystem->startDialog("greeting_001");

auto* questSystem = world->getSubsystem<QuestSystem>();
questSystem->startQuest("main_quest_01");

auto* inventorySystem = world->getSubsystem<InventorySystem>();
inventorySystem->addItem("key_tavern");
```

---

## 🔄 JÄMFÖRELSE MED UE5

| UE5 | Vårt System | Beskrivning |
|-----|-------------|-------------|
| UObject | Object | Universal bas |
| AActor | ActorObject | Objekt i världen |
| APawn | Pawn | Kontrollerbar entity |
| ACharacter | Character | Humanoid med movement |
| AController | Controller | Kontrollerar Pawn |
| APlayerController | PlayerController | Spelare-input |
| AAIController | AIController | AI-logik |
| UActorComponent | ActorComponent | Återanvändbar funktionalitet |
| USceneComponent | SceneComponent | Component med transform |
| UCharacterMovementComponent | MovementComponent | Movement-logik |
| UWorld | World | Världscontainer |
| ULevel | Level | Kapitel/akt |
| UGameInstanceSubsystem | GameSubsystem | Globalt system |

---

---

## 🏗️ ENGINE / EDITOR / GAME SEPARATION

### 📦 ENGINE (RetroCore.lib - Delad kod)

**Bas-klasser och Framework:**
```
engine/core/
├─ Object.h/cpp                    // Universal bas
├─ ActorObject.h/cpp               // Actor bas
├─ ActorComponent.h/cpp            // Component bas
├─ SceneComponent.h/cpp            // Component med transform
└─ Vec2.h, Transform.h, Color.h    // Math utilities

engine/actors/
├─ Pawn.h/cpp                      // Kontrollerbar entity
├─ Character.h/cpp                 // Humanoid
├─ Controller.h/cpp                // Controller bas
├─ PlayerController.h/cpp          // Spelare-input
├─ AIController.h/cpp              // AI-logik
├─ VisualActor.h/cpp               // Bas för visuella actors
├─ InteractiveActor.h/cpp          // Bas för interaktiva actors
└─ ItemActor.h/cpp                 // Bas för items

engine/components/
├─ SpriteComponent.h/cpp           // Sprite rendering
├─ AnimationComponent.h/cpp        // Animation
├─ MovementComponent.h/cpp         // Movement
├─ CollisionComponent.h/cpp        // Collision
├─ HealthComponent.h/cpp           // Health
├─ AIComponent.h/cpp               // AI behavior
└─ DialogComponent.h/cpp           // Dialog

engine/world/
├─ World.h/cpp                     // World container
├─ Level.h/cpp                     // Level/Chapter
├─ Scene.h/cpp                     // Room/Scene
└─ Camera2D.h/cpp                  // Camera

engine/systems/
├─ GameSubsystem.h/cpp             // Subsystem bas
├─ DialogSystem.h/cpp              // Dialog system
├─ QuestSystem.h/cpp               // Quest system
├─ InventorySystem.h/cpp           // Inventory
├─ AISystem.h/cpp                  // AI scheduler
├─ SaveSystem.h/cpp                // Save/load
└─ EventBus.h/cpp                  // Event messaging

engine/managers/
├─ TextureManager.h/cpp            // Texture loading
├─ FontManager.h/cpp               // Font loading
├─ AudioManager.h/cpp              // Audio playback
└─ InputManager.h/cpp              // Input handling

engine/data/
├─ DataAsset.h/cpp                 // Data bas
├─ RoomData.h                      // Room POD
├─ DialogData.h                    // Dialog POD
├─ QuestData.h                     // Quest POD
├─ ItemData.h                      // Item POD
├─ NPCData.h                       // NPC POD
└─ DataLoader.h/cpp                // JSON loading
```

**Vad ENGINE innehåller:**
- ✅ Alla bas-klasser (Object, ActorObject, Component)
- ✅ Alla framework-klasser (Pawn, Character, Controller)
- ✅ Alla systems (Dialog, Quest, AI, Save)
- ✅ Alla managers (Texture, Audio, Input)
- ✅ Data-strukturer och loaders
- ✅ Math utilities (Vec2, Transform, Color)
- ❌ INGEN game-specifik logik
- ❌ INGEN editor-UI

---

### 🎮 GAME (RetroGame.exe - Spellogik)

**Game-specifika implementationer:**
```
game/
├─ main.cpp                        // Entry point
├─ Game.h/cpp                      // Game loop
│
├─ actors/                         // Game-specifika actors
│  ├─ TavernKeeper.h/cpp           // Specifik NPC
│  ├─ MagicDoor.h/cpp              // Specifik interaktiv dörr
│  └─ HealthPotion.h/cpp           // Specifikt item
│
├─ components/                     // Game-specifika components
│  ├─ MagicComponent.h/cpp         // Magi-system
│  └─ CraftingComponent.h/cpp      // Crafting
│
├─ controllers/                    // Game-specifika controllers
│  └─ TutorialController.h/cpp     // Tutorial AI
│
├─ states/                         // Game states
│  ├─ MenuState.h/cpp              // Main menu
│  ├─ PlayState.h/cpp              // Gameplay
│  └─ PauseState.h/cpp             // Pause menu
│
└─ content/                        // Content scripts
   ├─ Quests.cpp                   // Quest definitions
   ├─ Dialogs.cpp                  // Dialog trees
   └─ Items.cpp                    // Item definitions
```

**Vad GAME innehåller:**
- ✅ Game loop och states
- ✅ Game-specifika actors (TavernKeeper, MagicDoor)
- ✅ Game-specifika components (Magic, Crafting)
- ✅ Content definitions (quests, dialogs, items)
- ✅ Main menu, pause menu
- ❌ INGEN editor-UI
- ❌ INGEN generisk engine-kod

---

### 🛠️ EDITOR (RetroEditor.exe - Editor-UI)

**Editor-specifik kod:**
```
editor/
├─ main.cpp                        // Editor entry point
├─ EditorApp.h/cpp                 // Editor application
├─ EditorContext.h/cpp             // Shared editor state
│
├─ panels/                         // ImGui panels
│  ├─ ViewportPanel.h/cpp          // Scene viewport
│  ├─ HierarchyPanel.h/cpp         // World/Level/Scene tree
│  ├─ InspectorPanel.h/cpp         // Property editor
│  ├─ ConsolePanel.h/cpp           // Log console
│  └─ AssetBrowserPanel.h/cpp      // Asset browser
│
├─ graphs/                         // Node editors
│  ├─ DialogGraphPanel.h/cpp       // Dialog tree editor
│  ├─ QuestGraphPanel.h/cpp        // Quest editor
│  └─ BehaviorGraphPanel.h/cpp     // AI behavior editor
│
├─ tools/                          // Editor tools
│  ├─ ActorSpawner.h/cpp           // Spawn actors
│  ├─ TransformGizmo.h/cpp         // Move/rotate/scale
│  └─ TerrainEditor.h/cpp          // Terrain painting
│
├─ commands/                       // Undo/redo
│  ├─ IEditorCommand.h             // Command interface
│  ├─ ActorCommands.h/cpp          // Actor operations
│  └─ ComponentCommands.h/cpp      // Component operations
│
├─ importers/                      // Asset pipeline
│  ├─ TiledImporter.h/cpp          // Tiled map import
│  ├─ SpriteImporter.h/cpp         // Sprite import
│  └─ DialogImporter.h/cpp         // Dialog import
│
└─ validators/                     // Data validation
   ├─ RoomValidator.h/cpp          // Validate rooms
   ├─ QuestValidator.h/cpp         // Validate quests
   └─ DialogValidator.h/cpp        // Validate dialogs
```

**Vad EDITOR innehåller:**
- ✅ ImGui panels och UI
- ✅ Visual editors (viewport, hierarchy, inspector)
- ✅ Node editors (dialog, quest, behavior)
- ✅ Undo/redo system
- ✅ Asset importers
- ✅ Data validators
- ✅ Transform gizmos
- ❌ INGEN gameplay-logik
- ❌ INGEN generisk engine-kod

---

## 🔗 DEPENDENCIES (Vem länkar vad?)

```
RetroCore.lib (ENGINE)
├─ SDL2, SDL2_image, SDL2_mixer
├─ nlohmann/json
└─ Inga dependencies till GAME eller EDITOR

RetroGame.exe (GAME)
├─ Länkar: RetroCore.lib
└─ Inga dependencies till EDITOR

RetroEditor.exe (EDITOR)
├─ Länkar: RetroCore.lib
├─ ImGui, ImGuizmo, imnodes, ImPlot
├─ nativefiledialog-extended
└─ Inga dependencies till GAME
```

**Viktigt:**
- ENGINE känner INTE till GAME eller EDITOR
- GAME känner INTE till EDITOR
- EDITOR känner INTE till GAME
- Alla tre kan använda ENGINE

---

## 📋 EXEMPEL PÅ SEPARATION

### Exempel 1: Skapa en NPC

**ENGINE tillhandahåller:**
```cpp
// engine/actors/Character.h
class Character : public Pawn {
    // Generisk humanoid
};

// engine/components/DialogComponent.h
class DialogComponent : public ActorComponent {
    // Generisk dialog-funktionalitet
};
```

**GAME använder:**
```cpp
// game/actors/TavernKeeper.cpp
class TavernKeeper : public Character {
public:
    TavernKeeper() : Character("TavernKeeper") {
        addComponent<SpriteComponent>("tavern_keeper.png");
        addComponent<DialogComponent>("tavern_keeper_dialog");
        addComponent<QuestGiverComponent>("main_quest");
    }
};
```

**EDITOR manipulerar:**
```cpp
// editor/panels/InspectorPanel.cpp
void InspectorPanel::renderActorProperties(ActorObject* actor) {
    // Visa och editera actor properties
    ImGui::InputText("Name", &actor->getName());
    
    // Visa components
    for (auto* comp : actor->getComponents()) {
        renderComponentProperties(comp);
    }
}
```

### Exempel 2: Dialog System

**ENGINE tillhandahåller:**
```cpp
// engine/systems/DialogSystem.h
class DialogSystem : public GameSubsystem {
public:
    void startDialog(const std::string& dialogId);
    void selectOption(int index);
    // Generisk dialog-logik
};
```

**GAME definierar content:**
```cpp
// game/content/Dialogs.cpp
void registerDialogs(DialogSystem* system) {
    system->registerDialog("greeting", {
        {"Hello traveler!", {
            {"Tell me about the quest", "quest_info"},
            {"Goodbye", "end"}
        }}
    });
}
```

**EDITOR editerar:**
```cpp
// editor/graphs/DialogGraphPanel.cpp
class DialogGraphPanel {
    // Visual node editor för dialog trees
    // Sparar till JSON som GAME laddar
};
```

---

## ✅ FÖRDELAR MED DENNA SEPARATION

**1. Modulär Arkitektur**
- ENGINE = Återanvändbar game framework
- GAME = Spelspecifik logik och content
- EDITOR = Utvecklingsverktyg

**2. Tydliga Gränser**
- Ingen circular dependency
- Lätt att testa varje del isolerat
- Kan bygga ENGINE som eget bibliotek

**3. Flexibilitet**
- Byt ut GAME utan att ändra ENGINE
- Byt ut EDITOR utan att ändra ENGINE
- Använd ENGINE i andra projekt

**4. Build-tid**
- Ändring i GAME → Bygg bara GAME
- Ändring i EDITOR → Bygg bara EDITOR
- Ändring i ENGINE → Bygg allt (men sällan)

**5. Testbarhet**
- Testa ENGINE utan GAME
- Testa GAME utan EDITOR
- Mock ENGINE för unit tests

