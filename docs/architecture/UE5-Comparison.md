# Unreal Engine 5 vs Vårt System - OOP Hierarki

## 🎮 Unreal Engine 5 Hierarki (Förebild)

### Kärnhierarki

```
UObject (Bas för ALLT)
├─ AActor (Objekt i världen)
│  ├─ APawn (Kan kontrolleras av spelare/AI)
│  │  └─ ACharacter (Humanoid med movement)
│  └─ AController (Kontrollerar Pawns)
│     ├─ APlayerController (Spelare)
│     └─ AAIController (AI)
│
└─ UActorComponent (Återanvändbara komponenter)
   └─ USceneComponent (Med transform)
      └─ UPrimitiveComponent (Med rendering/collision)
```

### Nyckelkoncept från UE5

**1. UObject - Universal Bas**
- **Allt** ärver från UObject
- Ger: Garbage collection, reflection, serialization, networking
- Används för: Data-objekt, managers, systems

**2. AActor - Världsobjekt**
- Objekt som kan placeras i världen
- Har position via RootComponent
- Kan ta skada (TakeDamage)
- Kan replikeras för multiplayer
- Byggs upp av ActorComponents

**3. UActorComponent - Modulär Funktionalitet**
- Återanvändbara bitar av funktionalitet
- Exempel: MeshComponent, MovementComponent, CameraComponent
- **Composition over inheritance!**
- Skapas i constructor med `CreateDefaultSubobject<T>()`

**4. APawn vs ACharacter**
- **APawn** = Generisk kontrollerbar entity (fordon, drone, block)
- **ACharacter** = Humanoid med CharacterMovementComponent
- Båda kan kontrolleras av Controller

**5. AController - Separation of Concerns**
- **Separat** från Pawn!
- PlayerController = Mänsklig input
- AIController = AI-logik
- En Controller "possessar" en Pawn

---

## 🔍 Vårt Nuvarande System

### Nuvarande Hierarki

```
Node (Förvirrande namn!)
├─ Node2D (Position, rotation, scale)
│  ├─ Sprite
│  ├─ AnimatedSprite
│  ├─ InteractiveArea
│  └─ WalkArea

Entity (SEPARAT hierarki!)
├─ Character
│  ├─ PlayerCharacter
│  └─ NPC
```

### Problem

**1. Dubbla hierarkier**
- Node och Entity är separata
- Character använder inte Node-systemet
- Ingen komponentbaserad arkitektur

**2. Förvirrande namn**
- "Node" säger ingenting om vad det är
- "Scene" vs "Room" - samma sak
- "Level" - oklart

**3. Ingen component-system**
- Allt är hårdkodat i klasser
- Svårt att återanvända funktionalitet
- Tight coupling

---

## 💡 FÖRESLAGEN NY HIERARKI (Inspirerad av UE5)

### Hierarki

```
Object (Bas för allt - som UObject)
│
├─ GameObject (Som AActor - objekt i världen)
│  │
│  ├─ Sprite (Visuellt objekt)
│  ├─ AnimatedSprite
│  ├─ InteractiveArea (Hotspot)
│  ├─ WalkArea
│  ├─ Marker
│  │
│  ├─ Pawn (Kontrollerbar entity - som APawn)
│  │  ├─ Character (Humanoid - som ACharacter)
│  │  │  ├─ Player
│  │  │  └─ NPC
│  │  └─ Vehicle (Framtida)
│  │
│  └─ Controller (Separat från Pawn!)
│     ├─ PlayerController
│     └─ AIController
│
└─ Component (Som UActorComponent)
   ├─ SpriteComponent
   ├─ AnimationComponent
   ├─ MovementComponent
   ├─ CollisionComponent
   ├─ DialogComponent
   └─ InventoryComponent
```

### Nyckelförändringar

**1. Object som Universal Bas**
```cpp
class Object {
    // Bas för ALLT i spelet
    // Ger: Serialization, reflection (framtida)
    std::string m_name;
    bool m_isActive = true;
};
```

**2. GameObject (istället för Node)**
```cpp
class GameObject : public Object {
    // Objekt i världen (som AActor)
    Vec2 m_position;
    float m_rotation = 0.0f;
    Vec2 m_scale{1.0f, 1.0f};
    
    std::vector<std::unique_ptr<Component>> m_components;
    std::vector<std::unique_ptr<GameObject>> m_children;
    
    void addComponent(std::unique_ptr<Component> comp);
    template<typename T> T* getComponent();
};
```

**3. Component System**
```cpp
class Component : public Object {
    GameObject* m_owner = nullptr;
    
    virtual void update(float deltaTime) {}
    virtual void render(SDL_Renderer* renderer) {}
};

class SpriteComponent : public Component {
    SDL_Texture* m_texture = nullptr;
    SDL_Rect m_sourceRect;
};

class MovementComponent : public Component {
    Vec2 m_velocity;
    float m_speed = 100.0f;
    
    void update(float deltaTime) override {
        m_owner->m_position += m_velocity * m_speed * deltaTime;
    }
};
```

**4. Pawn och Controller (Separation)**
```cpp
class Pawn : public GameObject {
    Controller* m_controller = nullptr;
    
    void possess(Controller* controller) {
        m_controller = controller;
    }
};

class Controller : public Object {
    Pawn* m_possessedPawn = nullptr;
    
    void possess(Pawn* pawn) {
        m_possessedPawn = pawn;
        pawn->possess(this);
    }
};

class PlayerController : public Controller {
    void handleInput(const Input& input) {
        // Skicka input till Pawn
        if (m_possessedPawn) {
            // Move pawn based on input
        }
    }
};
```

---

## 📊 Jämförelse

| Koncept | UE5 | Vårt Gamla | Vårt Nya |
|---------|-----|------------|----------|
| Universal bas | UObject | ❌ Ingen | ✅ Object |
| Världsobjekt | AActor | Node (dåligt namn) | ✅ GameObject |
| Kontrollerbar | APawn | Entity (separat!) | ✅ Pawn (ärver GameObject) |
| Humanoid | ACharacter | Character | ✅ Character (ärver Pawn) |
| Kontroller | AController | ❌ Ingen | ✅ Controller |
| Komponenter | UActorComponent | ❌ Ingen | ✅ Component |
| Composition | ✅ Ja | ❌ Nej | ✅ Ja |

---

## 🎯 Konkreta Exempel

### Skapa en NPC (Gamla systemet)
```cpp
// Problem: Tight coupling, ingen återanvändning
class NPC : public Character {
    SDL_Texture* m_texture;
    Animation m_animation;
    AIBehavior m_ai;
    DialogTree m_dialog;
    // Allt hårdkodat!
};
```

### Skapa en NPC (Nya systemet - UE5-inspirerat)
```cpp
// Lösning: Composition, återanvändbart
auto npc = std::make_unique<Character>("Tavern_Keeper");

// Lägg till komponenter
npc->addComponent<SpriteComponent>("tavern_keeper.png");
npc->addComponent<AnimationComponent>("idle", "walk", "talk");
npc->addComponent<DialogComponent>("tavern_keeper_dialog.json");
npc->addComponent<AIComponent>(AIBehaviorType::Wander);
npc->addComponent<CollisionComponent>(32, 64);

// Controller
auto aiController = std::make_unique<AIController>();
aiController->possess(npc.get());
```

### Fördelar
- **Återanvändning:** SpriteComponent kan användas av ALLA objekt
- **Flexibilitet:** Lägg till/ta bort komponenter dynamiskt
- **Testbarhet:** Testa komponenter isolerat
- **Läsbarhet:** Tydligt vad objektet kan göra

---

## 🚀 Migration Plan

### Fas 1: Byt Namn (Minimal Breaking Change)
```
Node → GameObject
Node2D → GameObject (merge)
Scene → Room
Level → Chapter
```

### Fas 2: Lägg till Component System
```cpp
// Skapa Component bas-klass
// Lägg till addComponent/getComponent i GameObject
// Migrera befintlig funktionalitet till komponenter
```

### Fas 3: Merge Entity och GameObject
```cpp
// Flytta Character/NPC till GameObject-hierarkin
// Ta bort separat Entity-hierarki
```

### Fas 4: Lägg till Controller System
```cpp
// Skapa Controller, PlayerController, AIController
// Separera input-logik från Player
```

---

## ✅ Fördelar med UE5-inspirerad Arkitektur

**1. Tydlighet**
- GameObject = Objekt i världen (självklart!)
- Component = Återanvändbar funktionalitet
- Pawn = Kontrollerbar entity
- Controller = Logik för kontroll

**2. Flexibilitet**
- Bygg objekt från komponenter
- Lägg till/ta bort funktionalitet runtime
- Återanvänd komponenter mellan objekt

**3. Separation of Concerns**
- Rendering → SpriteComponent
- Movement → MovementComponent
- AI → AIComponent
- Dialog → DialogComponent

**4. Testbarhet**
- Testa komponenter isolerat
- Mock komponenter för unit tests

**5. Skalbarhet**
- Lägg till nya komponenter utan att ändra GameObject
- Skapa nya objekt genom att kombinera komponenter

---

## 🤔 Frågor till Dig

1. **Namn:** Gillar du GameObject, Pawn, Controller?
2. **Component System:** Vill du ha detta? (Stor refactoring)
3. **Migration:** Ska vi göra allt på en gång eller stegvis?
4. **Prioritet:** Vad är viktigast först?
   - A) Byt namn (Node → GameObject)
   - B) Lägg till Components
   - C) Merge Entity och GameObject
   - D) Lägg till Controllers

