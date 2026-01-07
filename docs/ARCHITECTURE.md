# Retro Adventure - Komplett Arkitektur

## Översikt

Ett skalbart point-and-click adventure-spel byggt med modern C++17 och SDL2.

```
┌─────────────────────────────────────────────────────────────┐
│                         Game                                 │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐           │
│  │ States  │ │ Systems │ │Managers │ │  Core   │           │
│  └─────────┘ └─────────┘ └─────────┘ └─────────┘           │
└─────────────────────────────────────────────────────────────┘
```

---

## Modulstruktur

### Implementerat ✅

```
src/
├── Game.cpp/h            # Huvudloop, SDL init, state koordinering
├── Input.cpp/h           # Tangentbordshantering
├── Room.cpp/h            # Rum med hotspots och NPCs (legacy - deprecated)
│
├── states/               # Game states (State Pattern) 
│   ├── StateManager.cpp/h  # Push/pop/change (deferred) 
│   ├── IState.h            # Abstract interface 
│   ├── MenuState.cpp/h     # Huvudmeny 
│   ├── PlayState.cpp/h     # Gameplay 
│   ├── PauseState.cpp/h    # Pausmeny (overlay) 
│   ├── OptionsState.cpp/h  # Inställningar
│   ├── DialogState.cpp/h   # Dialog overlay
│   ├── InventoryState.cpp/h # Inventory overlay
│   └── QuestLogState.cpp/h # Quest log overlay
│
├── systems/              # Spelsystem (singletons)
│   ├── RoomManager.cpp/h    # Rumhantering och transitions (legacy)
│   ├── SceneManager.cpp/h   # Scenehantering och transitions (nytt)
│   ├── DialogSystem.cpp/h   # Dialog-träd och val
│   ├── InventorySystem.cpp/h # Items och kombination
│   ├── QuestSystem.cpp/h    # Quests och objectives
│   ├── AISystem.cpp/h       # NPC-beteenden och scheman
│   ├── SaveSystem.cpp/h     # JSON save/load
│   └── CutsceneSystem.cpp/h # Scripted sequences
│
├── data/                 # Data-laddning
│   ├── GameData.h           # Datastrukturer med JSON-makron
│   ├── DataLoader.cpp/h     # JSON-filläsning
│   └── GameDataLoader.h     # Integration med spelsystem
│
├── graphics/             # Grafiksystem 
│   ├── TextureManager.cpp/h  # Singleton, texture caching 
│   ├── SpriteSheet.cpp/h     # Frame-baserad rendering 
│   ├── Animation.cpp/h       # Tidsbaserad animation
│   ├── FontManager.cpp/h     # SDL_ttf text rendering
│   └── Transition.cpp/h      # Fade transitions
│
├── audio/                # Ljudsystem 
│   └── AudioManager.cpp/h    # Music, SFX, volume 
│
├── actors/               # Actor system (nytt)
│   ├── ActorObjectExtended.cpp/h # Actor base class
│   ├── CharacterActor.cpp/h      # Character implementation
│   ├── NPC.cpp/h                 # Non-player characters
│   └── [fler actors...]
│
├── entities/             # Entity hierarki (OOP) (legacy)
│   ├── Entity.cpp/h          # Abstract base 
│   ├── Character.cpp/h       # Character base 
│   ├── PlayerCharacter.cpp/h # Spelaren
│   └── NPC.cpp/h             # Non-player characters (borttagen)
│
├── ui/                   # Widget-system
│   └── Widget.cpp/h          # Label, Button, Panel, ProgressBar
│
├── editor/               # ImGui-baserad editor (2026-01-06)
│   ├── ImGuiManager.cpp/h    # ImGui lifecycle management
│   ├── EditorContext.cpp/h   # Shared state mellan paneler
│   ├── EditorState.cpp/h     # Main editor state (211 rader, refaktorerad)
│   ├── IEditorPanel.h        # Panel interface
│   ├── VisualRoomEditor.cpp/h # Visuell rum-redigering
│   ├── RoomDataManager.cpp/h  # Room data persistence
│   ├── EditorTabRenderer.cpp/h # Legacy tab rendering
│   ├── TiledIntegration.cpp/h # Tiled import/export
│   ├── managers/              # Manager classes (SoC) ✨ NEW
│   │   ├── EditorPanelManager.cpp/h    # Panel creation & lifecycle
│   │   ├── EditorWorldManager.cpp/h    # World/Level/Scene setup
│   │   └── EditorEventDispatcher.cpp/h # Event handling & shortcuts
│   ├── input/                 # Input handling ✨ NEW
│   │   └── EditorInputHandler.cpp/h    # Centralized shortcuts (Del, Ctrl+Z/Y/D/S)
│   ├── data/                  # Editor data serialization ✨ NEW
│   │   ├── EditorDataManager.cpp/h  # Central data coordinator
│   │   ├── ISerializer.h            # Serialization interface
│   │   ├── SceneSerializer.cpp/h    # Scene→JSON serialization
│   │   ├── WorldSerializer.cpp/h    # World→JSON serialization
│   │   └── EditorDataTypes.h        # Editor-specific data types
│   ├── panels/               # ImGui panels
│   │   ├── HierarchyPanel.cpp/h      # Objektträd
│   │   ├── PropertiesPanel.cpp/h     # Inspector
│   │   ├── ViewportPanel.h           # Viewport header (132 rader)
│   │   ├── ViewportPanel_Core.cpp    # Constructor, update, render, toolbar
│   │   ├── ViewportPanel_World.cpp   # World view rendering
│   │   ├── ViewportPanel_Level.cpp   # Level view rendering
│   │   ├── ViewportPanel_Scene.cpp   # Scene view rendering + drag-drop
│   │   ├── ViewportPanel_Actors.cpp  # Actor rendering helpers
│   │   ├── ViewportPanel_Input.cpp   # Mouse/keyboard input handling
│   │   ├── AssetBrowserPanel.cpp/h   # Asset browser med drag-source
│   │   ├── PlaceActorsPanel.cpp/h    # UE5-style actor placement ✨ NEW
│   │   ├── ConsolePanel.cpp/h        # Logg console
│   │   ├── SceneGraphPanel.cpp/h     # Node tree med drag-drop
│   │   ├── LayerEditorPanel.cpp/h    # Layer management
│   │   ├── WorldViewPanel.cpp/h      # World/Level navigation
│   │   ├── LevelViewPanel.cpp/h      # Level/Scene navigation
│   │   └── TileMapEditorPanel.cpp/h  # TileMap painting
│   ├── graphs/               # Node graph editors
│   │   ├── INodeGraphPanel.cpp/h     # Base för graph panels
│   │   ├── dialog/
│   │   │   ├── DialogNode.cpp/h      # Dialog node
│   │   │   └── DialogGraphPanel.cpp/h # Dialog graph editor
│   │   ├── quest/
│   │   │   ├── QuestNode.cpp/h       # Quest node
│   │   │   └── QuestGraphPanel.cpp/h # Quest graph editor
│   │   └── npc/
│   │       ├── BehaviorNode.cpp/h    # Behavior node
│   │       └── BehaviorGraphPanel.cpp/h # NPC behavior editor
│   └── properties/           # Property editors
│       ├── IPropertyEditor.h         # Base interface
│       ├── RoomPropertyEditor.cpp/h
│       ├── HotspotPropertyEditor.cpp/h
│       ├── DialogPropertyEditor.cpp/h
│       ├── QuestPropertyEditor.cpp/h
│       ├── ItemPropertyEditor.cpp/h
│       └── NPCPropertyEditor.cpp/h
│
├── engine/               # RetroCore static library ✨ NEW
│   ├── core/             # Core node system
│   │   ├── Vec2.cpp/h           # 2D vector math
│   │   ├── Node.cpp/h           # Scene graph base
│   │   └── Node2D.cpp/h         # 2D transform
│   ├── nodes/            # Visual nodes
│   │   ├── Sprite.cpp/h         # Texture rendering
│   │   ├── AnimatedSprite.cpp/h # Animation state machine
│   │   ├── TileMapLayer.cpp/h   # Grid-based tilemap
│   │   ├── ParallaxLayer.cpp/h  # Parallax scrolling
│   │   └── Label.cpp/h          # Text rendering
│   ├── world/            # World management
│   │   ├── WorldContainer.h    # Abstract base for World/Level/Scene
│   │   ├── World.cpp/h         # Multi-scene manager
│   │   ├── Level.cpp/h         # Scene container
│   │   ├── Scene.cpp/h         # Unified Scene class (consolidated 2026-01-06)
│   │   ├── RoomToSceneConverter.cpp/h # Migration tool (deprecated)
│   │   ├── Camera2D.cpp/h       # Viewport med follow/zoom/shake
│   │   ├── LayerManager.cpp/h   # Layer system
│   │   └── SceneLoader.cpp/h    # JSON serialization
│   └── physics/          # Physics system
│       ├── CollisionShape.cpp/h # AABB, Circle, Polygon
│       ├── PhysicsBody.cpp/h    # Velocity, acceleration, friction
│       ├── KinematicBody.cpp/h  # moveAndSlide()
│       └── SpatialGrid.cpp/h    # Spatial partitioning ✨ NEW
│
└── vendor/               # Vendored dependencies
    ├── imgui_impl_sdl2.cpp/h         # SDL2 backend
    ├── imgui_impl_sdlrenderer2.cpp/h # SDL2 renderer backend
    └── imnodes/                      # Node graph library
        ├── imnodes.h
        └── imnodes.cpp
```


## Entity Hierarki (OOP)

### Principer
- **Composition over Inheritance** - Komponenter för delad funktionalitet
- **Max 3 nivåer** - Entity → Character → PlayerCharacter
- **Single Responsibility** - En klass = ett ansvar

### Arvshierarki

```
Entity (abstract)
│   - position, size, active
│   - update(), render()
│
├── Character (abstract)
│   │   - health, name
│   │   - MovementComponent
│   │   - AnimationComponent
│   │
│   ├── PlayerCharacter
│   │       - inventory
│   │       - point-and-click movement
│   │
│   └── AICharacter (abstract)
│       │   - AI behavior
│       │   - pathfinding
│       │
│       └── NPCCharacter
│               - DialogComponent
│               - schedule
│
├── Item
│       - pickupable
│       - combinable
│
└── Hotspot
        - interactable area
        - trigger actions
```

### Entity Base Class

```cpp
class Entity {
public:
    virtual ~Entity() = default;
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    bool isActive() const { return m_active; }
    
protected:
    float m_x = 0, m_y = 0;
    int m_width = 0, m_height = 0;
    bool m_active = true;
};
```

### Character Base Class

```cpp
class Character : public Entity {
public:
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    const std::string& getName() const { return m_name; }
    int getHealth() const { return m_health; }
    
protected:
    std::string m_name;
    int m_health = 100;
    std::unique_ptr<MovementComponent> m_movement;
    std::unique_ptr<AnimationComponent> m_animation;
};
```

---

## Klassdiagram

### Core Classes

```
┌──────────────────────────────────────────┐
│                  Game                     │
├──────────────────────────────────────────┤
│ - m_window: unique_ptr<Window>           │
│ - m_renderer: unique_ptr<Renderer>       │
│ - m_stateManager: unique_ptr<StateManager>│
│ - m_audioManager: unique_ptr<AudioManager>│
│ - m_running: bool                        │
├──────────────────────────────────────────┤
│ + init(): bool                           │
│ + run(): void                            │
│ + quit(): void                           │
└──────────────────────────────────────────┘
```

### State Pattern

```
┌─────────────────┐
│    IState       │ (interface)
├─────────────────┤
│ + enter()       │
│ + exit()        │
│ + update(dt)    │
│ + render()      │
│ + handleEvent() │
└────────┬────────┘
         │
    ┌────┴────┬──────────┬──────────┐
    ▼         ▼          ▼          ▼
┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐
│MenuState│ │PlayState│ │PauseState│ │DialogState│
└─────────┘ └─────────┘ └─────────┘ └─────────┘
```

### Entity Hierarchy

```
┌─────────────────────────────────┐
│            Entity               │
├─────────────────────────────────┤
│ # m_id: int                     │
│ # m_position: Vector2           │
│ # m_sprite: Sprite*             │
│ # m_active: bool                │
├─────────────────────────────────┤
│ + update(dt): virtual           │
│ + render(): virtual             │
│ + getPosition(): Vector2        │
└────────────┬────────────────────┘
             │
    ┌────────┼────────┬──────────┐
    ▼        ▼        ▼          ▼
┌────────┐ ┌─────┐ ┌──────┐ ┌────────┐
│ Player │ │ NPC │ │ Item │ │Hotspot │
└────────┘ └─────┘ └──────┘ └────────┘
```

---

## System Detaljer

### 1. State Manager
```cpp
class StateManager {
    std::stack<std::unique_ptr<IState>> m_states;
public:
    void pushState(std::unique_ptr<IState> state);
    void popState();
    void changeState(std::unique_ptr<IState> state);
    IState* getCurrentState();
};
```

### 2. Dialog System
```cpp
struct DialogNode {
    int id;
    std::string speaker;
    std::string text;
    std::vector<DialogChoice> choices;
};

struct DialogChoice {
    std::string text;
    int nextNodeId;
    std::function<bool()> condition;  // Villkor för att visa
    std::function<void()> action;     // Vad händer vid val
};

class DialogSystem {
    std::unordered_map<std::string, DialogTree> m_dialogs;
public:
    void startDialog(const std::string& dialogId);
    void selectChoice(int choiceIndex);
    bool isActive() const;
};
```

### 3. Quest System
```cpp
enum class QuestStatus { NotStarted, Active, Completed, Failed };

struct QuestObjective {
    std::string description;
    bool completed;
    std::function<bool()> checkCompletion;
};

class Quest {
    std::string m_id, m_title, m_description;
    QuestStatus m_status;
    std::vector<QuestObjective> m_objectives;
public:
    void activate();
    void update();
    bool isCompleted() const;
};

class QuestSystem {
    std::vector<std::unique_ptr<Quest>> m_quests;
public:
    void addQuest(std::unique_ptr<Quest> quest);
    void updateQuests();
    Quest* getQuest(const std::string& id);
};
```

### 4. Inventory System
```cpp
class InventorySystem {
    std::vector<Item> m_items;
    int m_maxSlots = 20;
    Item* m_selectedItem = nullptr;
public:
    bool addItem(const Item& item);
    bool removeItem(const std::string& itemId);
    bool hasItem(const std::string& itemId) const;
    bool useItem(const std::string& itemId, Entity* target);
    bool combineItems(const std::string& id1, const std::string& id2);
};
```

### 5. AI System (för NPC)
```cpp
enum class AIState { Idle, Walking, Talking, Working };

class AIBehavior {
public:
    virtual void update(NPC* npc, float dt) = 0;
    virtual ~AIBehavior() = default;
};

class ScheduledBehavior : public AIBehavior {
    struct ScheduleEntry {
        int startHour, endHour;
        Vector2 location;
        AIState state;
    };
    std::vector<ScheduleEntry> m_schedule;
};

class AISystem {
    std::vector<NPC*> m_npcs;
public:
    void update(float dt);
    void setNPCBehavior(NPC* npc, std::unique_ptr<AIBehavior> behavior);
};
```

### 6. Room System
```cpp
struct WalkArea {
    int minX, maxX, minY, maxY;
    float scaleTop = 0.5f;     // Karaktärsstorlek vid minY (långt bort)
    float scaleBottom = 1.0f;  // Karaktärsstorlek vid maxY (nära)
};

class Room {
    std::string m_id, m_name;
    SDL_Texture* m_background;
    WalkArea m_walkArea;       // Inkl perspektiv-skalning
    std::vector<Hotspot> m_hotspots;
    std::vector<Exit> m_exits;
    std::vector<NPC*> m_npcs;
public:
    void enter();
    void exit();
    void update(float dt);
    void render(Renderer* renderer);
    Hotspot* getHotspotAt(int x, int y);
};

class RoomSystem {
    std::unordered_map<std::string, std::unique_ptr<Room>> m_rooms;
    Room* m_currentRoom = nullptr;
public:
    void loadRoom(const std::string& roomId);
    void changeRoom(const std::string& roomId, const std::string& exitId);
    Room* getCurrentRoom();
};
```

### 7. Audio Manager
```cpp
class AudioManager {
    std::unordered_map<std::string, Mix_Chunk*> m_sounds;
    std::unordered_map<std::string, Mix_Music*> m_music;
    float m_musicVolume = 1.0f;
    float m_sfxVolume = 1.0f;
    bool m_muted = false;
public:
    void loadSound(const std::string& id, const std::string& path);
    void loadMusic(const std::string& id, const std::string& path);
    void playSound(const std::string& id);
    void playMusic(const std::string& id, bool loop = true);
    void stopMusic();
    void setMusicVolume(float volume);
    void setSFXVolume(float volume);
    void toggleMute();     // Global mute toggle (M key)
    bool isMuted() const;
};
```

### 8. Save System
```cpp
struct SaveData {
    std::string currentRoom;
    Vector2 playerPosition;
    std::vector<std::string> inventoryItems;
    std::vector<std::pair<std::string, QuestStatus>> questStates;
    std::unordered_map<std::string, bool> flags;  // Story flags
};

class SaveSystem {
public:
    bool save(const std::string& slotName, const SaveData& data);
    bool load(const std::string& slotName, SaveData& data);
    std::vector<std::string> getSaveSlots();
    bool deleteSave(const std::string& slotName);
};
```

---

## Data Format (JSON)

### Room Definition
```json
{
  "id": "tavern",
  "name": "The Rusty Anchor",
  "background": "assets/backgrounds/tavern.png",
  "walkArea": { 
    "minX": 0, "maxX": 640, "minY": 220, "maxY": 380,
    "scaleTop": 0.6,     // Karaktär 60% storlek längst bort
    "scaleBottom": 1.0   // Karaktär 100% storlek närmast
  },
  "hotspots": [
    {
      "id": "bartender",
      "type": "npc",
      "rect": { "x": 80, "y": 250, "w": 40, "h": 60 },
      "dialog": "bartender_intro"
    }
  ],
  "exits": [
    {
      "id": "door",
      "rect": { "x": 600, "y": 280, "w": 30, "h": 70 },
      "targetRoom": "street",
      "targetExit": "tavern_door"
    }
  ]
}
```

### Dialog Definition
```json
{
  "id": "bartender_intro",
  "nodes": [
    {
      "id": 0,
      "speaker": "Bartender",
      "text": "Welcome to The Rusty Anchor! What can I get ya?",
      "choices": [
        { "text": "I'm looking for information.", "next": 1 },
        { "text": "Just passing through.", "next": 2 },
        { "text": "Goodbye.", "next": -1 }
      ]
    }
  ]
}
```

---

## Event System

```cpp
// Centralt event-system för löst kopplad kommunikation
class EventBus {
public:
    template<typename EventType>
    void subscribe(std::function<void(const EventType&)> handler);
    
    template<typename EventType>
    void publish(const EventType& event);
};

// Exempel på events
struct ItemPickedUpEvent { std::string itemId; };
struct QuestCompletedEvent { std::string questId; };
struct RoomChangedEvent { std::string fromRoom, toRoom; };
struct DialogStartedEvent { std::string dialogId, npcId; };
```

---

## Beroendegraf

```
Game
 ├── Window
 ├── Renderer ─────────┐
 ├── Input             │
 ├── Timer             │
 ├── AudioManager      │
 ├── StateManager      │
 │    └── IState ──────┤
 │         ├── MenuState
 │         └── PlayState
 │              ├── RoomSystem
 │              ├── Player
 │              ├── InventorySystem
 │              ├── QuestSystem
 │              ├── DialogSystem
 │              └── AISystem
 └── EventBus
```
