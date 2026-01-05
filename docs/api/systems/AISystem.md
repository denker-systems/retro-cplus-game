# AISystem

> Singleton som hanterar NPC AI och beteenden

## Includes
```cpp
#include "systems/AISystem.h"
```

## Översikt

`AISystem` hanterar NPC-beteenden, pathfinding, schedules och waypoints.

## Singleton Access

```cpp
AISystem& AISystem::instance();
```

## Datastrukturer

### BehaviorType
```cpp
enum class BehaviorType {
    Idle,      // Står still
    Patrol,    // Patrullerar waypoints
    Wander,    // Vandrar slumpmässigt
    Follow,    // Följer target
    GoTo,      // Går till specifik punkt
    Custom     // Anpassat beteende
};
```

### AIState
```cpp
enum class AIState {
    Idle,
    Walking,
    Talking,
    Working,
    Sleeping
};
```

### ScheduleEntry
```cpp
struct ScheduleEntry {
    int startHour;
    int endHour;
    Vector2 location;
    AIState state;
    std::string roomId;
};
```

### Waypoint
```cpp
struct Waypoint {
    float x, y;
    float waitTime = 0.0f;  // Sekunder att vänta vid waypoint
};
```

## Publika Metoder

### registerNPC
```cpp
void registerNPC(NPC* npc);
```
**Beskrivning:** Registrerar en NPC för AI-hantering.

**Parametrar:**
- `npc` - Pekare till NPC

**Exempel:**
```cpp
auto npc = std::make_unique<NPC>(100, 200, "Guard");
AISystem::instance().registerNPC(npc.get());
```

### unregisterNPC
```cpp
void unregisterNPC(NPC* npc);
```
**Beskrivning:** Avregistrerar en NPC.

**Parametrar:**
- `npc` - Pekare till NPC

### setBehavior
```cpp
void setBehavior(const std::string& npcName, BehaviorType behavior);
```
**Beskrivning:** Sätter beteende för en NPC.

**Parametrar:**
- `npcName` - Namn på NPC
- `behavior` - Beteende att sätta

**Exempel:**
```cpp
AISystem::instance().setBehavior("Guard", BehaviorType::Patrol);
```

### setWaypoints
```cpp
void setWaypoints(const std::string& npcName, const std::vector<Waypoint>& waypoints);
```
**Beskrivning:** Sätter waypoints för patrol-beteende.

**Parametrar:**
- `npcName` - Namn på NPC
- `waypoints` - Vector med waypoints

**Exempel:**
```cpp
std::vector<Waypoint> waypoints = {
    {100, 200, 2.0f},  // Vänta 2 sek
    {300, 200, 1.0f},
    {300, 300, 0.0f},
    {100, 300, 1.5f}
};
AISystem::instance().setWaypoints("Guard", waypoints);
```

### setSchedule
```cpp
void setSchedule(const std::string& npcName, const std::vector<ScheduleEntry>& schedule);
```
**Beskrivning:** Sätter tidschema för NPC.

**Parametrar:**
- `npcName` - Namn på NPC
- `schedule` - Vector med schema-entries

**Exempel:**
```cpp
std::vector<ScheduleEntry> schedule = {
    {6, 12, {100, 200}, AIState::Working, "shop"},
    {12, 13, {200, 250}, AIState::Idle, "tavern"},
    {13, 18, {100, 200}, AIState::Working, "shop"},
    {18, 22, {150, 300}, AIState::Idle, "home"},
    {22, 6, {150, 300}, AIState::Sleeping, "home"}
};
AISystem::instance().setSchedule("Shopkeeper", schedule);
```

### setFollowTarget
```cpp
void setFollowTarget(const std::string& npcName, Entity* target);
```
**Beskrivning:** Sätter target för Follow-beteende.

**Parametrar:**
- `npcName` - Namn på NPC
- `target` - Entity att följa

**Exempel:**
```cpp
// NPC följer spelaren
AISystem::instance().setBehavior("Companion", BehaviorType::Follow);
AISystem::instance().setFollowTarget("Companion", player);
```

### setGoToTarget
```cpp
void setGoToTarget(const std::string& npcName, float x, float y);
```
**Beskrivning:** Sätter mål för GoTo-beteende.

**Parametrar:**
- `npcName` - Namn på NPC
- `x` - X-koordinat
- `y` - Y-koordinat

**Exempel:**
```cpp
AISystem::instance().setBehavior("Messenger", BehaviorType::GoTo);
AISystem::instance().setGoToTarget("Messenger", 400, 300);
```

### update
```cpp
void update(float deltaTime);
```
**Beskrivning:** Uppdaterar alla NPCs AI.

**Parametrar:**
- `deltaTime` - Tid sedan förra frame

Anropas varje frame från PlayState.

### pauseAI
```cpp
void pauseAI(bool paused);
```
**Beskrivning:** Pausar/återupptar all AI.

**Parametrar:**
- `paused` - true för att pausa

### isNPCBusy
```cpp
bool isNPCBusy(const std::string& npcName) const;
```
**Beskrivning:** Kontrollerar om NPC är upptagen.

**Parametrar:**
- `npcName` - Namn på NPC

**Returnerar:** `true` om NPC är upptagen (pratar, arbetar, etc.)

## JSON Format

```json
{
  "npcs": [
    {
      "id": "guard_01",
      "name": "Town Guard",
      "room": "street",
      "x": 200,
      "y": 300,
      "spriteSheet": "assets/sprites/guard.png",
      "canMove": true,
      "moveSpeed": 80.0,
      "dialogId": "guard_greeting",
      "behavior": "patrol",
      "waypoints": [
        {"x": 100, "y": 280, "wait": 2.0},
        {"x": 500, "y": 280, "wait": 1.0},
        {"x": 500, "y": 320, "wait": 0.0},
        {"x": 100, "y": 320, "wait": 1.5}
      ]
    },
    {
      "id": "shopkeeper",
      "name": "Shopkeeper",
      "room": "shop",
      "x": 400,
      "y": 270,
      "canMove": true,
      "moveSpeed": 60.0,
      "dialogId": "shopkeeper_intro",
      "behavior": "schedule",
      "schedule": [
        {
          "startHour": 8,
          "endHour": 18,
          "room": "shop",
          "x": 400,
          "y": 270,
          "state": "working"
        },
        {
          "startHour": 18,
          "endHour": 22,
          "room": "tavern",
          "x": 200,
          "y": 300,
          "state": "idle"
        },
        {
          "startHour": 22,
          "endHour": 8,
          "room": "home",
          "x": 150,
          "y": 250,
          "state": "sleeping"
        }
      ]
    }
  ]
}
```

## Beteende-Implementation

### Idle
```cpp
void updateIdle(NPC* npc, float dt) {
    // Gör ingenting
}
```

### Patrol
```cpp
void updatePatrol(NPC* npc, float dt) {
    auto& waypoints = npc->getWaypoints();
    int currentWP = npc->getCurrentWaypoint();
    
    // Gå till nuvarande waypoint
    Vector2 target = waypoints[currentWP].position;
    npc->moveTo(target.x, target.y);
    
    // Om nått waypoint
    if (npc->hasReachedTarget()) {
        npc->wait(waypoints[currentWP].waitTime);
        npc->setCurrentWaypoint((currentWP + 1) % waypoints.size());
    }
}
```

### Wander
```cpp
void updateWander(NPC* npc, float dt) {
    if (!npc->hasTarget()) {
        // Välj slumpmässig punkt i walk area
        auto walkArea = RoomManager::instance().getCurrentRoom()->getWalkArea();
        float x = randomFloat(walkArea.minX, walkArea.maxX);
        float y = randomFloat(walkArea.minY, walkArea.maxY);
        npc->moveTo(x, y);
    }
}
```

### Follow
```cpp
void updateFollow(NPC* npc, float dt) {
    Entity* target = npc->getFollowTarget();
    if (!target) return;
    
    float distance = npc->distanceTo(target);
    
    // Håll avstånd
    if (distance > 100.0f) {
        npc->moveTo(target->getX(), target->getY());
    } else if (distance < 50.0f) {
        npc->stop();
    }
}
```

### Schedule
```cpp
void updateSchedule(NPC* npc, float dt) {
    int currentHour = GameTime::instance().getHour();
    auto& schedule = npc->getSchedule();
    
    // Hitta rätt schedule entry
    for (auto& entry : schedule) {
        if (currentHour >= entry.startHour && currentHour < entry.endHour) {
            // Byt rum om nödvändigt
            if (npc->getRoomId() != entry.roomId) {
                RoomManager::instance().moveNPCToRoom(npc, entry.roomId);
            }
            
            // Gå till position
            npc->moveTo(entry.location.x, entry.location.y);
            npc->setState(entry.state);
            break;
        }
    }
}
```

## Custom Behavior

```cpp
class CustomBehavior : public AIBehavior {
public:
    void update(NPC* npc, float dt) override {
        // Anpassad logik här
    }
};

// Registrera
AISystem::instance().registerCustomBehavior("Guard", std::make_unique<CustomBehavior>());
```

## Se även

- [NPC](../entities/NPC.md)
- [GameTime](GameTime.md)
- [RoomManager](RoomManager.md)
