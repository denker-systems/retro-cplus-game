# SaveSystem

> Singleton som hanterar save/load av speldata

## Includes
```cpp
#include "systems/SaveSystem.h"
```

## Översikt

`SaveSystem` hanterar JSON-serialization av speldata till save slots.

## Singleton Access

```cpp
SaveSystem& SaveSystem::instance();
```

## Datastrukturer

### SaveData
```cpp
struct SaveData {
    std::string currentRoom;
    float playerX, playerY;
    std::vector<std::string> inventory;
    std::unordered_map<std::string, QuestStatus> quests;
    std::unordered_map<std::string, bool> flags;
    std::unordered_map<std::string, int> counters;
    float playTime;  // Sekunder
    std::string timestamp;
};
```

## Publika Metoder

### save
```cpp
bool save(const std::string& slotName);
```
**Beskrivning:** Sparar nuvarande speldata till en slot.

**Parametrar:**
- `slotName` - Namn på save slot (t.ex. "slot1", "quicksave")

**Returnerar:** `true` om sparning lyckades

**Exempel:**
```cpp
if (SaveSystem::instance().save("slot1")) {
    LOG_INFO("Game saved!");
}
```

### load
```cpp
bool load(const std::string& slotName);
```
**Beskrivning:** Laddar speldata från en slot.

**Parametrar:**
- `slotName` - Namn på save slot

**Returnerar:** `true` om laddning lyckades

**Exempel:**
```cpp
if (SaveSystem::instance().load("slot1")) {
    // Spel laddat, byt till PlayState
    game->changeState(std::make_unique<PlayState>());
}
```

### deleteSave
```cpp
bool deleteSave(const std::string& slotName);
```
**Beskrivning:** Raderar en save slot.

**Parametrar:**
- `slotName` - Namn på save slot

**Returnerar:** `true` om radering lyckades

### getSaveSlots
```cpp
std::vector<std::string> getSaveSlots() const;
```
**Beskrivning:** Hämtar alla tillgängliga save slots.

**Returnerar:** Vector med slot-namn

**Exempel:**
```cpp
auto slots = SaveSystem::instance().getSaveSlots();
for (const auto& slot : slots) {
    std::cout << "Found save: " << slot << std::endl;
}
```

### getSaveInfo
```cpp
SaveInfo getSaveInfo(const std::string& slotName) const;
```
**Beskrivning:** Hämtar metadata om en save.

**Parametrar:**
- `slotName` - Namn på save slot

**Returnerar:** SaveInfo struct med metadata

```cpp
struct SaveInfo {
    std::string slotName;
    std::string timestamp;
    std::string currentRoom;
    float playTime;
    bool exists;
};
```

### quickSave
```cpp
bool quickSave();
```
**Beskrivning:** Snabbsparar till "quicksave" slot.

**Returnerar:** `true` om sparning lyckades

### quickLoad
```cpp
bool quickLoad();
```
**Beskrivning:** Snabbladdar från "quicksave" slot.

**Returnerar:** `true` om laddning lyckades

### autoSave
```cpp
bool autoSave();
```
**Beskrivning:** Auto-sparar till "autosave" slot.

**Returnerar:** `true` om sparning lyckades

## JSON Format

```json
{
  "version": "1.0",
  "timestamp": "2026-01-05T12:30:45",
  "playTime": 3600.5,
  "currentRoom": "tavern",
  "player": {
    "x": 320.0,
    "y": 300.0
  },
  "inventory": [
    "rusty_key",
    "health_potion",
    "map"
  ],
  "quests": {
    "find_treasure": "active",
    "tavern_mystery": "completed",
    "forest_quest": "not_started"
  },
  "flags": {
    "talked_to_bartender": true,
    "cellar_unlocked": true,
    "found_secret_passage": false
  },
  "counters": {
    "enemies_defeated": 5,
    "gold_collected": 250,
    "rooms_visited": 8
  }
}
```

## Save Location

Saves sparas i:
```
assets/saves/
├── slot1.json
├── slot2.json
├── slot3.json
├── quicksave.json
└── autosave.json
```

## Capture Current State

```cpp
SaveData SaveSystem::captureCurrentState() {
    SaveData data;
    
    // Room och position
    auto* room = RoomManager::instance().getCurrentRoom();
    data.currentRoom = room->getId();
    
    auto* player = PlayState::getPlayer();
    data.playerX = player->getX();
    data.playerY = player->getY();
    
    // Inventory
    auto& items = InventorySystem::instance().getItems();
    for (const auto& item : items) {
        data.inventory.push_back(item.id);
    }
    
    // Quests
    auto quests = QuestSystem::instance().getAllQuests();
    for (const auto& quest : quests) {
        data.quests[quest.id] = quest.status;
    }
    
    // Flags och counters
    data.flags = WorldQuery::getAllFlags();
    data.counters = WorldQuery::getAllCounters();
    
    // Metadata
    data.playTime = GameTime::instance().getTotalPlayTime();
    data.timestamp = getCurrentTimestamp();
    
    return data;
}
```

## Restore State

```cpp
void SaveSystem::restoreState(const SaveData& data) {
    // Byt rum
    RoomManager::instance().changeRoom(data.currentRoom);
    
    // Sätt player position
    auto* player = PlayState::getPlayer();
    player->setPosition(data.playerX, data.playerY);
    
    // Återställ inventory
    InventorySystem::instance().clear();
    for (const auto& itemId : data.inventory) {
        InventorySystem::instance().addItem(itemId);
    }
    
    // Återställ quests
    for (const auto& [questId, status] : data.quests) {
        auto* quest = QuestSystem::instance().getQuest(questId);
        if (quest) {
            quest->status = status;
        }
    }
    
    // Återställ flags och counters
    WorldQuery::setAllFlags(data.flags);
    WorldQuery::setAllCounters(data.counters);
    
    // Återställ play time
    GameTime::instance().setTotalPlayTime(data.playTime);
}
```

## Auto-Save Triggers

Auto-save kan triggas vid:
- Room transitions
- Quest completion
- Efter X minuters spelande
- Före farliga situationer

```cpp
// I RoomManager::changeRoom
void RoomManager::changeRoom(const std::string& roomId) {
    // ... byt rum ...
    
    // Auto-save
    SaveSystem::instance().autoSave();
}

// I QuestSystem::completeQuest
void QuestSystem::completeQuest(const std::string& questId) {
    // ... markera som completed ...
    
    // Auto-save
    SaveSystem::instance().autoSave();
}
```

## Error Handling

```cpp
bool SaveSystem::save(const std::string& slotName) {
    try {
        SaveData data = captureCurrentState();
        json j = serializeToJson(data);
        
        std::string path = "assets/saves/" + slotName + ".json";
        std::ofstream file(path);
        
        if (!file.is_open()) {
            LOG_ERROR("Failed to open save file: " + path);
            return false;
        }
        
        file << j.dump(2); // Pretty print med indent 2
        file.close();
        
        LOG_INFO("Game saved to: " + slotName);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Save failed: " + std::string(e.what()));
        return false;
    }
}
```

## Integration med SaveLoadState

```cpp
// I SaveLoadState
void SaveLoadState::render(SDL_Renderer* renderer) {
    auto slots = SaveSystem::instance().getSaveSlots();
    
    for (size_t i = 0; i < slots.size(); i++) {
        auto info = SaveSystem::instance().getSaveInfo(slots[i]);
        
        // Rita slot info
        renderSlot(renderer, i, info);
    }
}

void SaveLoadState::handleSaveClick(const std::string& slotName) {
    if (SaveSystem::instance().save(slotName)) {
        showMessage("Game saved!");
    } else {
        showMessage("Save failed!");
    }
}

void SaveLoadState::handleLoadClick(const std::string& slotName) {
    if (SaveSystem::instance().load(slotName)) {
        game->changeState(std::make_unique<PlayState>());
    } else {
        showMessage("Load failed!");
    }
}
```

## Se även

- [SaveLoadState](../states/SaveLoadState.md)
- [WorldQuery](WorldQuery.md)
- [GameTime](GameTime.md)
