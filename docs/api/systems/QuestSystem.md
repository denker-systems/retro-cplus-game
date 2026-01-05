# QuestSystem

> Singleton som hanterar quests och objectives

## Includes
```cpp
#include "systems/QuestSystem.h"
```

## Översikt

`QuestSystem` hanterar quests med objectives, auto-completion och progress tracking.

## Singleton Access

```cpp
QuestSystem& QuestSystem::instance();
```

## Datastrukturer

### QuestStatus
```cpp
enum class QuestStatus {
    NotStarted,
    Active,
    Completed,
    Failed
};
```

### ObjectiveType
```cpp
enum class ObjectiveType {
    Talk,      // Prata med NPC
    Collect,   // Samla items
    Deliver,   // Leverera item till NPC
    GoTo,      // Gå till plats
    Examine,   // Undersök objekt
    Custom     // Anpassad condition
};
```

### Objective
```cpp
struct Objective {
    std::string id;
    std::string description;
    ObjectiveType type;
    bool completed = false;
    std::function<bool()> checkCompletion;  // Custom check
};
```

### Quest
```cpp
struct Quest {
    std::string id;
    std::string title;
    std::string description;
    QuestStatus status = QuestStatus::NotStarted;
    std::vector<Objective> objectives;
    std::function<void()> onComplete;  // Callback vid completion
};
```

## Publika Metoder

### addQuest
```cpp
void addQuest(const Quest& quest);
```
**Beskrivning:** Lägger till en quest.

**Parametrar:**
- `quest` - Quest att lägga till

### startQuest
```cpp
void startQuest(const std::string& questId);
```
**Beskrivning:** Startar en quest (sätter status till Active).

**Parametrar:**
- `questId` - ID på quest att starta

**Exempel:**
```cpp
QuestSystem::instance().startQuest("find_treasure");
```

### completeQuest
```cpp
void completeQuest(const std::string& questId);
```
**Beskrivning:** Markerar quest som completed.

**Parametrar:**
- `questId` - ID på quest

### failQuest
```cpp
void failQuest(const std::string& questId);
```
**Beskrivning:** Markerar quest som failed.

**Parametrar:**
- `questId` - ID på quest

### updateObjective
```cpp
void updateObjective(const std::string& questId, const std::string& objectiveId);
```
**Beskrivning:** Markerar ett objective som completed.

**Parametrar:**
- `questId` - ID på quest
- `objectiveId` - ID på objective

**Exempel:**
```cpp
// När spelaren pratar med bartender
QuestSystem::instance().updateObjective("tavern_mystery", "talk_to_bartender");
```

### update
```cpp
void update();
```
**Beskrivning:** Uppdaterar alla quests (kollar auto-completion).

Anropas varje frame från PlayState.

### getQuest
```cpp
Quest* getQuest(const std::string& questId);
```
**Beskrivning:** Hämtar en quest.

**Parametrar:**
- `questId` - ID på quest

**Returnerar:** Pekare till quest, eller `nullptr` om inte hittad

### getActiveQuests
```cpp
std::vector<Quest*> getActiveQuests() const;
```
**Beskrivning:** Hämtar alla aktiva quests.

**Returnerar:** Vector med pekare till aktiva quests

### getCompletedQuests
```cpp
std::vector<Quest*> getCompletedQuests() const;
```
**Beskrivning:** Hämtar alla completed quests.

**Returnerar:** Vector med pekare till completed quests

### isQuestActive
```cpp
bool isQuestActive(const std::string& questId) const;
```
**Beskrivning:** Kontrollerar om en quest är aktiv.

**Parametrar:**
- `questId` - ID på quest

**Returnerar:** `true` om quest är aktiv

### isQuestCompleted
```cpp
bool isQuestCompleted(const std::string& questId) const;
```
**Beskrivning:** Kontrollerar om en quest är completed.

**Parametrar:**
- `questId` - ID på quest

**Returnerar:** `true` om quest är completed

## JSON Format

```json
{
  "quests": [
    {
      "id": "find_treasure",
      "title": "The Lost Treasure",
      "description": "Find the legendary treasure hidden in the tavern cellar.",
      "objectives": [
        {
          "id": "talk_to_bartender",
          "description": "Talk to the bartender",
          "type": "talk",
          "target": "bartender"
        },
        {
          "id": "find_key",
          "description": "Find the cellar key",
          "type": "collect",
          "target": "cellar_key"
        },
        {
          "id": "enter_cellar",
          "description": "Enter the cellar",
          "type": "goto",
          "target": "cellar"
        },
        {
          "id": "open_chest",
          "description": "Open the treasure chest",
          "type": "examine",
          "target": "treasure_chest"
        }
      ]
    }
  ]
}
```

## Auto-Completion

Objectives kan auto-completeas baserat på typ:

```cpp
void QuestSystem::update() {
    for (auto& quest : m_quests) {
        if (quest.status != QuestStatus::Active) continue;
        
        for (auto& obj : quest.objectives) {
            if (obj.completed) continue;
            
            // Kolla auto-completion baserat på typ
            switch (obj.type) {
                case ObjectiveType::Collect:
                    if (InventorySystem::instance().hasItem(obj.target)) {
                        obj.completed = true;
                    }
                    break;
                    
                case ObjectiveType::GoTo:
                    if (RoomManager::instance().getCurrentRoom()->getId() == obj.target) {
                        obj.completed = true;
                    }
                    break;
                    
                case ObjectiveType::Custom:
                    if (obj.checkCompletion && obj.checkCompletion()) {
                        obj.completed = true;
                    }
                    break;
            }
        }
        
        // Kolla om alla objectives är completed
        if (allObjectivesCompleted(quest)) {
            completeQuest(quest.id);
        }
    }
}
```

## Custom Objectives

För komplexa objectives, använd Custom type med lambda:

```cpp
Quest quest;
quest.id = "mystery_quest";

Objective obj;
obj.id = "solve_puzzle";
obj.description = "Solve the ancient puzzle";
obj.type = ObjectiveType::Custom;
obj.checkCompletion = []() {
    // Komplex logik här
    return WorldQuery::getFlag("puzzle_solved") && 
           InventorySystem::instance().hasItem("ancient_artifact");
};

quest.objectives.push_back(obj);
QuestSystem::instance().addQuest(quest);
```

## Quest Completion Callbacks

```cpp
Quest quest;
quest.id = "first_quest";
quest.onComplete = []() {
    // Ge reward
    InventorySystem::instance().addGold(100);
    InventorySystem::instance().addItem("reward_sword");
    
    // Starta nästa quest
    QuestSystem::instance().startQuest("second_quest");
    
    // Sätt flag
    WorldQuery::setFlag("completed_first_quest", true);
    
    // Visa meddelande
    LOG_INFO("Quest completed! Received 100 gold and a sword.");
};
```

## Integration med QuestLogState

```cpp
// Öppna quest log (J-tangent)
game->pushState(std::make_unique<QuestLogState>());

// QuestLogState visar alla aktiva quests
auto activeQuests = QuestSystem::instance().getActiveQuests();
for (auto* quest : activeQuests) {
    renderQuestInfo(quest);
}
```

## Exempel: Komplett Quest

```cpp
Quest createTavernQuest() {
    Quest quest;
    quest.id = "tavern_mystery";
    quest.title = "Mystery at the Tavern";
    quest.description = "Investigate strange noises in the cellar.";
    
    Objective obj1;
    obj1.id = "talk_to_bartender";
    obj1.description = "Talk to the bartender";
    obj1.type = ObjectiveType::Talk;
    obj1.target = "bartender";
    quest.objectives.push_back(obj1);
    
    Objective obj2;
    obj2.id = "get_key";
    obj2.description = "Find the cellar key";
    obj2.type = ObjectiveType::Collect;
    obj2.target = "cellar_key";
    quest.objectives.push_back(obj2);
    
    Objective obj3;
    obj3.id = "investigate_cellar";
    obj3.description = "Investigate the cellar";
    obj3.type = ObjectiveType::GoTo;
    obj3.target = "cellar";
    quest.objectives.push_back(obj3);
    
    quest.onComplete = []() {
        InventorySystem::instance().addGold(50);
        WorldQuery::setFlag("tavern_mystery_solved", true);
    };
    
    return quest;
}
```

## Se även

- [QuestLogState](../states/QuestLogState.md)
- [InventorySystem](InventorySystem.md)
- [DialogSystem](DialogSystem.md)
