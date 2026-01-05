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

```
src/
├── core/                 # Kärnfunktionalitet
│   ├── Game.cpp/h        # Huvudloop, koordinering
│   ├── Window.cpp/h      # SDL fönsterhantering
│   ├── Renderer.cpp/h    # Rendering, texturer
│   ├── Input.cpp/h       # Tangentbord, mus
│   └── Timer.cpp/h       # Delta time, FPS
│
├── states/               # Game states (State Pattern)
│   ├── StateManager.cpp/h
│   ├── IState.h          # Interface
│   ├── MenuState.cpp/h
│   ├── PlayState.cpp/h
│   ├── PauseState.cpp/h
│   ├── InventoryState.cpp/h
│   └── DialogState.cpp/h
│
├── entities/             # Spelobjekt (Entity-Component)
│   ├── Entity.cpp/h      # Basklass
│   ├── Player.cpp/h
│   ├── NPC.cpp/h
│   ├── Item.cpp/h
│   └── Hotspot.cpp/h
│
├── systems/              # Spelsystem
│   ├── RoomSystem.cpp/h      # Rum och navigation
│   ├── DialogSystem.cpp/h    # Dialogträd
│   ├── QuestSystem.cpp/h     # Quests och mål
│   ├── InventorySystem.cpp/h # Inventory
│   ├── AISystem.cpp/h        # NPC beteende
│   └── SaveSystem.cpp/h      # Spara/Ladda
│
├── audio/                # Ljudsystem
│   ├── AudioManager.cpp/h
│   ├── Music.cpp/h
│   └── SoundEffect.cpp/h
│
├── graphics/             # Grafiksystem
│   ├── TextureManager.cpp/h
│   ├── SpriteSheet.cpp/h
│   ├── Animation.cpp/h
│   ├── ParticleSystem.cpp/h
│   └── Camera.cpp/h
│
├── ui/                   # Användargränssnitt
│   ├── UIManager.cpp/h
│   ├── Widget.cpp/h      # Basklass
│   ├── Button.cpp/h
│   ├── Label.cpp/h
│   ├── TextBox.cpp/h
│   └── InventoryUI.cpp/h
│
├── data/                 # Datastrukturer
│   ├── Room.cpp/h
│   ├── Quest.cpp/h
│   ├── Dialog.cpp/h
│   ├── Character.cpp/h
│   └── ItemData.cpp/h
│
└── utils/                # Hjälpklasser
    ├── ResourcePath.cpp/h
    ├── Logger.cpp/h
    ├── Config.cpp/h
    └── Math.cpp/h
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
class Room {
    std::string m_id, m_name;
    SDL_Texture* m_background;
    SDL_Rect m_walkArea;
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
public:
    void loadSound(const std::string& id, const std::string& path);
    void loadMusic(const std::string& id, const std::string& path);
    void playSound(const std::string& id);
    void playMusic(const std::string& id, bool loop = true);
    void stopMusic();
    void setMusicVolume(float volume);
    void setSFXVolume(float volume);
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
  "walkArea": { "x": 0, "y": 200, "w": 640, "h": 150 },
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
