# Komplett OOP-Hierarki - Retro Adventure Game
## Systematisk Arkitektur (UE5-inspirerad)

---

## 🌟 KOMPLETT HIERARKI

```
Object (Universal bas - som UObject i UE5)
│
├─ GameObject (Objekt i världen - som AActor)
│  ├─ Visual Objects (Synliga objekt)
│  │  ├─ Sprite
│  │  ├─ AnimatedSprite
│  │  ├─ Label
│  │  ├─ TileMapLayer
│  │  └─ ParallaxLayer
│  │
│  ├─ Interactive Objects (Interaktiva objekt)
│  │  ├─ InteractiveArea (Hotspots - dörrar, items, NPCs)
│  │  ├─ WalkArea (Walkable polygon)
│  │  └─ Marker (Spawn points, waypoints)
│  │
│  ├─ Pawns (Kontrollerbara entities)
│  │  ├─ Character (Humanoid)
│  │  │  ├─ Player
│  │  │  └─ NPC
│  │  └─ Vehicle (Framtida - båt, häst, etc)
│  │
│  ├─ Controllers (Kontrollerar Pawns)
│  │  ├─ PlayerController (Input från spelare)
│  │  └─ AIController (AI-logik)
│  │
│  └─ Items (Plockas upp, används)
│     ├─ InventoryItem (I inventory)
│     ├─ Weapon (Vapen)
│     └─ Consumable (Förbrukningsvaror)
│
├─ Component (Återanvändbar funktionalitet)
│  ├─ Visual Components
│  │  ├─ SpriteComponent
│  │  ├─ AnimationComponent
│  │  └─ ParticleComponent
│  │
│  ├─ Gameplay Components
│  │  ├─ MovementComponent
│  │  ├─ CollisionComponent
│  │  ├─ HealthComponent
│  │  └─ InteractionComponent
│  │
│  ├─ AI Components
│  │  ├─ AIComponent (Beteende)
│  │  ├─ PathfindingComponent
│  │  └─ PerceptionComponent
│  │
│  └─ Dialog & Quest Components
│     ├─ DialogComponent
│     ├─ QuestGiverComponent
│     └─ InventoryComponent
│
├─ World Structure (Världsorganisation)
│  ├─ World (Hela spelet)
│  ├─ Chapter (Stor del av spelet - som Level i UE5)
│  └─ Room (En location/scene)
│
├─ Systems (Globala system - som Subsystems i UE5)
│  ├─ Game Systems
│  │  ├─ DialogSystem
│  │  ├─ QuestSystem
│  │  ├─ InventorySystem
│  │  └─ SaveSystem
│  │
│  ├─ AI Systems
│  │  ├─ AISystem (Scheduler, behavior)
│  │  ├─ PathfindingSystem
│  │  └─ PerceptionSystem
│  │
│  ├─ State Systems
│  │  ├─ WorldState (Global flags, variables)
│  │  ├─ ConditionSystem (Evaluerar villkor)
│  │  └─ EventBus (Event messaging)
│  │
│  └─ Special Systems
│     ├─ CutsceneSystem
│     ├─ HintSystem
│     ├─ JournalSystem
│     └─ RecapSystem
│
├─ Managers (Singletons - som Managers i UE5)
│  ├─ TextureManager
│  ├─ FontManager
│  ├─ AudioManager
│  └─ InputManager
│
└─ Data (POD - Plain Old Data)
   ├─ GameData
   │  ├─ RoomData (JSON → Room)
   │  ├─ DialogData (JSON → DialogSystem)
   │  ├─ QuestData (JSON → QuestSystem)
   │  ├─ ItemData (JSON → Item)
   │  └─ NPCData (JSON → NPC)
   │
   └─ Utilities
      ├─ Vec2, Rect (Math)
      ├─ Color
      └─ Transform
```

---

## 📋 DETALJERAD BESKRIVNING

### 1. Object (Universal Bas)

```cpp
class Object {
public:
    Object(const std::string& name) : m_name(name) {}
    virtual ~Object() = default;
    
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    bool isActive() const { return m_isActive; }
    void setActive(bool active) { m_isActive = active; }
    
    // Framtida: Serialization, reflection
    virtual void serialize(/* ... */) {}
    virtual void deserialize(/* ... */) {}
    
protected:
    std::string m_name;
    bool m_isActive = true;
};
```

**Används av:** ALLT i spelet
**Ger:** Namn, active state, framtida serialization

---

### 2. GameObject (Objekt i Världen)

```cpp
class GameObject : public Object {
public:
    GameObject(const std::string& name) : Object(name) {}
    
    // Transform
    Vec2 getPosition() const { return m_position; }
    void setPosition(Vec2 pos) { m_position = pos; }
    float getRotation() const { return m_rotation; }
    Vec2 getScale() const { return m_scale; }
    
    // Hierarchy
    void addChild(std::unique_ptr<GameObject> child);
    void removeChild(GameObject* child);
    const std::vector<std::unique_ptr<GameObject>>& getChildren() const;
    GameObject* getParent() const { return m_parent; }
    
    // Components
    void addComponent(std::unique_ptr<Component> component);
    template<typename T> T* getComponent();
    template<typename T> void removeComponent();
    
    // Lifecycle
    virtual void update(float deltaTime);
    virtual void render(SDL_Renderer* renderer);
    
    // Visibility
    bool isVisible() const { return m_isVisible; }
    void setVisible(bool visible) { m_isVisible = visible; }
    
    // Z-order for rendering
    int getZIndex() const { return m_zIndex; }
    void setZIndex(int z) { m_zIndex = z; }
    
protected:
    Vec2 m_position{0, 0};
    float m_rotation = 0.0f;
    Vec2 m_scale{1.0f, 1.0f};
    
    GameObject* m_parent = nullptr;
    std::vector<std::unique_ptr<GameObject>> m_children;
    std::vector<std::unique_ptr<Component>> m_components;
    
    bool m_isVisible = true;
    int m_zIndex = 0;
};
```

**Används av:** Allt som finns i världen
**Ger:** Position, hierarchy, components, rendering

---

### 3. Component (Återanvändbar Funktionalitet)

```cpp
class Component : public Object {
public:
    Component(const std::string& name) : Object(name) {}
    
    void setOwner(GameObject* owner) { m_owner = owner; }
    GameObject* getOwner() const { return m_owner; }
    
    virtual void update(float deltaTime) {}
    virtual void render(SDL_Renderer* renderer) {}
    
protected:
    GameObject* m_owner = nullptr;
};
```

**Exempel: SpriteComponent**
```cpp
class SpriteComponent : public Component {
public:
    void setTexture(SDL_Texture* texture) { m_texture = texture; }
    void setSourceRect(SDL_Rect rect) { m_sourceRect = rect; }
    
    void render(SDL_Renderer* renderer) override {
        if (!m_texture || !m_owner) return;
        
        SDL_Rect destRect = {
            (int)m_owner->getPosition().x,
            (int)m_owner->getPosition().y,
            m_sourceRect.w,
            m_sourceRect.h
        };
        
        SDL_RenderCopy(renderer, m_texture, &m_sourceRect, &destRect);
    }
    
private:
    SDL_Texture* m_texture = nullptr;
    SDL_Rect m_sourceRect{0, 0, 32, 32};
};
```

**Exempel: MovementComponent**
```cpp
class MovementComponent : public Component {
public:
    void setVelocity(Vec2 vel) { m_velocity = vel; }
    void setSpeed(float speed) { m_speed = speed; }
    
    void update(float deltaTime) override {
        if (!m_owner) return;
        
        Vec2 pos = m_owner->getPosition();
        pos += m_velocity * m_speed * deltaTime;
        m_owner->setPosition(pos);
    }
    
private:
    Vec2 m_velocity{0, 0};
    float m_speed = 100.0f;
};
```

---

### 4. Pawn (Kontrollerbar Entity)

```cpp
class Pawn : public GameObject {
public:
    void possess(Controller* controller);
    void unpossess();
    Controller* getController() const { return m_controller; }
    
    // Input från controller
    virtual void moveForward(float value) {}
    virtual void moveRight(float value) {}
    virtual void interact() {}
    
protected:
    Controller* m_controller = nullptr;
};
```

---

### 5. Character (Humanoid Pawn)

```cpp
class Character : public Pawn {
public:
    Character(const std::string& name) : Pawn(name) {
        // Lägg till standard-komponenter
        addComponent<SpriteComponent>();
        addComponent<AnimationComponent>();
        addComponent<MovementComponent>();
        addComponent<CollisionComponent>();
    }
    
    void moveForward(float value) override {
        auto* movement = getComponent<MovementComponent>();
        if (movement) {
            movement->setVelocity(Vec2(0, value));
        }
    }
    
    void moveRight(float value) override {
        auto* movement = getComponent<MovementComponent>();
        if (movement) {
            movement->setVelocity(Vec2(value, 0));
        }
    }
};
```

---

### 6. Player (Spelarkaraktär)

```cpp
class Player : public Character {
public:
    Player() : Character("Player") {
        addComponent<InventoryComponent>();
        addComponent<HealthComponent>();
    }
    
    void interact() override {
        // Kolla om vi står vid något interaktivt
        // Trigga dialog, plocka upp item, etc.
    }
};
```

---

### 7. NPC (Non-Player Character)

```cpp
class NPC : public Character {
public:
    NPC(const std::string& name) : Character(name) {
        addComponent<AIComponent>();
        addComponent<DialogComponent>();
    }
    
    void setDialog(const std::string& dialogId) {
        auto* dialog = getComponent<DialogComponent>();
        if (dialog) {
            dialog->setDialogId(dialogId);
        }
    }
    
    void setBehavior(AIBehaviorType type) {
        auto* ai = getComponent<AIComponent>();
        if (ai) {
            ai->setBehavior(type);
        }
    }
};
```

---

### 8. Controller (Kontrollerar Pawns)

```cpp
class Controller : public Object {
public:
    void possess(Pawn* pawn) {
        if (m_possessedPawn) {
            m_possessedPawn->unpossess();
        }
        m_possessedPawn = pawn;
        pawn->possess(this);
    }
    
    void unpossess() {
        if (m_possessedPawn) {
            m_possessedPawn->unpossess();
            m_possessedPawn = nullptr;
        }
    }
    
    Pawn* getPawn() const { return m_possessedPawn; }
    
protected:
    Pawn* m_possessedPawn = nullptr;
};
```

---

### 9. PlayerController (Spelar-input)

```cpp
class PlayerController : public Controller {
public:
    void handleInput(const Input& input) {
        if (!m_possessedPawn) return;
        
        // WASD movement
        float horizontal = 0.0f;
        float vertical = 0.0f;
        
        if (input.isKeyDown(SDL_SCANCODE_W)) vertical -= 1.0f;
        if (input.isKeyDown(SDL_SCANCODE_S)) vertical += 1.0f;
        if (input.isKeyDown(SDL_SCANCODE_A)) horizontal -= 1.0f;
        if (input.isKeyDown(SDL_SCANCODE_D)) horizontal += 1.0f;
        
        m_possessedPawn->moveForward(vertical);
        m_possessedPawn->moveRight(horizontal);
        
        // E to interact
        if (input.isKeyPressed(SDL_SCANCODE_E)) {
            m_possessedPawn->interact();
        }
    }
};
```

---

### 10. AIController (AI-logik)

```cpp
class AIController : public Controller {
public:
    void update(float deltaTime) {
        if (!m_possessedPawn) return;
        
        // Kör AI-beteende
        switch (m_currentBehavior) {
            case AIBehaviorType::Idle:
                // Stå still
                break;
                
            case AIBehaviorType::Wander:
                // Vandra runt
                wanderBehavior(deltaTime);
                break;
                
            case AIBehaviorType::Follow:
                // Följ target
                followBehavior(deltaTime);
                break;
        }
    }
    
    void setBehavior(AIBehaviorType type) {
        m_currentBehavior = type;
    }
    
private:
    AIBehaviorType m_currentBehavior = AIBehaviorType::Idle;
    
    void wanderBehavior(float deltaTime) {
        // Implementera wander-logik
    }
    
    void followBehavior(float deltaTime) {
        // Implementera follow-logik
    }
};
```

---

### 11. World Structure (Världsorganisation)

```cpp
class World : public Object {
public:
    void addChapter(std::unique_ptr<Chapter> chapter);
    Chapter* getChapter(const std::string& id);
    Chapter* getActiveChapter() { return m_activeChapter; }
    
    void setActiveChapter(const std::string& id);
    void transitionToChapter(const std::string& id);
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    
private:
    std::vector<std::unique_ptr<Chapter>> m_chapters;
    std::unordered_map<std::string, Chapter*> m_chapterMap;
    Chapter* m_activeChapter = nullptr;
};

class Chapter : public Object {
public:
    void addRoom(std::unique_ptr<Room> room);
    Room* getRoom(const std::string& id);
    Room* getActiveRoom() { return m_activeRoom; }
    
    void setActiveRoom(const std::string& id);
    
private:
    std::vector<std::unique_ptr<Room>> m_rooms;
    std::unordered_map<std::string, Room*> m_roomMap;
    Room* m_activeRoom = nullptr;
};

class Room : public Object {
public:
    void addGameObject(std::unique_ptr<GameObject> obj);
    void removeGameObject(GameObject* obj);
    
    const std::vector<std::unique_ptr<GameObject>>& getGameObjects() const;
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    
private:
    std::vector<std::unique_ptr<GameObject>> m_gameObjects;
};
```

---

### 12. Systems (Globala System)

```cpp
class System : public Object {
public:
    virtual void initialize() {}
    virtual void update(float deltaTime) {}
    virtual void shutdown() {}
};

class DialogSystem : public System {
public:
    void startDialog(const std::string& dialogId);
    void selectOption(int optionIndex);
    bool isDialogActive() const;
    
private:
    std::unordered_map<std::string, DialogData> m_dialogs;
    DialogData* m_currentDialog = nullptr;
};

class QuestSystem : public System {
public:
    void startQuest(const std::string& questId);
    void completeObjective(const std::string& objectiveId);
    bool isQuestActive(const std::string& questId) const;
    
private:
    std::unordered_map<std::string, QuestData> m_quests;
    std::vector<std::string> m_activeQuests;
};

class InventorySystem : public System {
public:
    void addItem(const std::string& itemId);
    void removeItem(const std::string& itemId);
    bool hasItem(const std::string& itemId) const;
    
private:
    std::vector<std::string> m_items;
};
```

---

## 🎯 KONKRETA ANVÄNDNINGSEXEMPEL

### Exempel 1: Skapa en Tavern med NPC

```cpp
// Skapa rummet
auto tavern = std::make_unique<Room>("tavern");

// Lägg till bakgrund
auto background = std::make_unique<GameObject>("Background");
background->addComponent<SpriteComponent>("tavern_bg.png");
background->setZIndex(-100);
tavern->addGameObject(std::move(background));

// Lägg till walk area
auto walkArea = std::make_unique<WalkArea>("WalkArea");
walkArea->setBounds({100, 500, 200, 400});
tavern->addGameObject(std::move(walkArea));

// Skapa bartender NPC
auto bartender = std::make_unique<NPC>("Bartender");
bartender->setPosition({300, 250});
bartender->setDialog("bartender_greeting");
bartender->setBehavior(AIBehaviorType::Idle);
tavern->addGameObject(std::move(bartender));

// Skapa AI controller för bartender
auto aiController = std::make_unique<AIController>();
aiController->possess(bartender.get());

// Lägg till hotspot för dörr
auto door = std::make_unique<InteractiveArea>("Door", 450, 200, 50, 100);
door->setAction("exit_to_street");
tavern->addGameObject(std::move(door));
```

### Exempel 2: Skapa Player med Controller

```cpp
// Skapa player
auto player = std::make_unique<Player>();
player->setPosition({200, 300});

// Skapa player controller
auto playerController = std::make_unique<PlayerController>();
playerController->possess(player.get());

// I game loop
void Game::handleInput(const Input& input) {
    playerController->handleInput(input);
}
```

### Exempel 3: Ladda från JSON

```cpp
// Ladda room data från JSON
RoomData roomData = DataLoader::loadRoom("tavern.json");

// Konvertera till Room med GameObjects
auto room = RoomToGameObjectConverter::convert(roomData);

// Lägg till i Chapter
chapter->addRoom(std::move(room));
```

---

## 📊 RELATIONER OCH OWNERSHIP

```
World
├─ owns Chapters
│  └─ owns Rooms
│     └─ owns GameObjects
│        └─ owns Components

PlayerController
└─ possesses Player (Pawn)

AIController
└─ possesses NPC (Pawn)

DialogSystem
└─ references DialogData

QuestSystem
└─ references QuestData
```

---

## ✅ FÖRDELAR MED DENNA HIERARKI

**1. Systematisk och Tydlig**
- Allt har sin plats
- Tydliga relationer
- Lätt att hitta vad man söker

**2. Återanvändning**
- Components kan användas av alla GameObjects
- Systems är globala och återanvändbara
- Controllers kan byta Pawn

**3. Separation of Concerns**
- Rendering → Components
- Logic → Controllers
- Data → Systems
- Structure → World/Chapter/Room

**4. Testbarhet**
- Testa Components isolerat
- Testa Systems utan GameObject
- Mock Controllers för unit tests

**5. Skalbarhet**
- Lägg till nya GameObjects enkelt
- Lägg till nya Components utan att ändra GameObject
- Lägg till nya Systems utan att ändra World

---

## 🚀 MIGRATION PLAN

### Fas 1: Skapa Bas-klasser
- [ ] Skapa Object bas-klass
- [ ] Skapa GameObject (ersätter Node)
- [ ] Skapa Component bas-klass
- [ ] Skapa Controller bas-klass

### Fas 2: Migrera Befintliga Klasser
- [ ] Sprite → GameObject med SpriteComponent
- [ ] AnimatedSprite → GameObject med AnimationComponent
- [ ] InteractiveArea → GameObject (behåll som egen klass)
- [ ] WalkArea → GameObject (behåll som egen klass)

### Fas 3: Skapa Pawn/Character Hierarki
- [ ] Skapa Pawn bas-klass
- [ ] Skapa Character (ärver Pawn)
- [ ] Migrera PlayerCharacter → Player (ärver Character)
- [ ] Migrera NPC → NPC (ärver Character)

### Fas 4: Skapa Controllers
- [ ] Skapa Controller bas-klass
- [ ] Skapa PlayerController
- [ ] Skapa AIController
- [ ] Separera input-logik från Player

### Fas 5: Organisera World Structure
- [ ] Byt namn: Level → Chapter
- [ ] Byt namn: Scene → Room
- [ ] Uppdatera World att använda Chapter/Room

### Fas 6: Cleanup
- [ ] Ta bort gamla Entity-klasser
- [ ] Ta bort legacy Room.h
- [ ] Ta bort RoomManager
- [ ] Uppdatera alla references

---

## 🤔 NÄSTA STEG

Vad vill du göra först?

**A) Börja med Fas 1** - Skapa bas-klasserna (Object, GameObject, Component, Controller)
**B) Diskutera hierarkin mer** - Finns det något du vill ändra?
**C) Se konkreta kodexempel** - Vill du se mer detaljerad implementation?
**D) Börja migrera stegvis** - Välj en del att börja med

