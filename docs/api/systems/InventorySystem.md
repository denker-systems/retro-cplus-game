# InventorySystem

> Singleton som hanterar spelarens inventory och items

## Includes
```cpp
#include "systems/InventorySystem.h"
```

## Översikt

`InventorySystem` hanterar items, pickup, kombination och användning. Stödjer max antal slots och item-stacking.

## Singleton Access

```cpp
InventorySystem& InventorySystem::instance();
```

## Datastrukturer

### Item
```cpp
struct Item {
    std::string id;
    std::string name;
    std::string description;
    std::string icon;        // Path till ikon
    bool stackable = false;
    int quantity = 1;
    ItemType type = ItemType::Generic;
};
```

### ItemType
```cpp
enum class ItemType {
    Generic,
    Key,
    Tool,
    Consumable,
    QuestItem
};
```

## Publika Metoder

### addItem
```cpp
bool addItem(const std::string& itemId);
```
**Beskrivning:** Lägger till ett item i inventory.

**Parametrar:**
- `itemId` - ID på item att lägga till

**Returnerar:** `true` om item lades till, `false` om inventory är fullt

**Exempel:**
```cpp
if (InventorySystem::instance().addItem("rusty_key")) {
    LOG_INFO("Picked up rusty key!");
}
```

### removeItem
```cpp
bool removeItem(const std::string& itemId, int quantity = 1);
```
**Beskrivning:** Tar bort ett item från inventory.

**Parametrar:**
- `itemId` - ID på item att ta bort
- `quantity` - Antal att ta bort (för stackable items)

**Returnerar:** `true` om item togs bort

**Exempel:**
```cpp
InventorySystem::instance().removeItem("health_potion");
```

### hasItem
```cpp
bool hasItem(const std::string& itemId) const;
```
**Beskrivning:** Kontrollerar om spelaren har ett item.

**Parametrar:**
- `itemId` - ID på item att kolla

**Returnerar:** `true` om item finns i inventory

**Exempel:**
```cpp
if (InventorySystem::instance().hasItem("cellar_key")) {
    // Lås upp dörr
}
```

### getItemCount
```cpp
int getItemCount(const std::string& itemId) const;
```
**Beskrivning:** Hämtar antal av ett item.

**Parametrar:**
- `itemId` - ID på item

**Returnerar:** Antal (0 om item inte finns)

### useItem
```cpp
bool useItem(const std::string& itemId, const std::string& targetId = "");
```
**Beskrivning:** Använder ett item, eventuellt på ett target.

**Parametrar:**
- `itemId` - ID på item att använda
- `targetId` - ID på target (hotspot, NPC, etc.)

**Returnerar:** `true` om användning lyckades

**Exempel:**
```cpp
// Använd nyckel på dörr
if (InventorySystem::instance().useItem("rusty_key", "cellar_door")) {
    // Dörr öppnad!
}
```

### combineItems
```cpp
std::string combineItems(const std::string& item1Id, const std::string& item2Id);
```
**Beskrivning:** Kombinerar två items.

**Parametrar:**
- `item1Id` - Första item
- `item2Id` - Andra item

**Returnerar:** ID på resulterande item, eller tom sträng om kombination misslyckades

**Exempel:**
```cpp
std::string result = InventorySystem::instance().combineItems("rope", "hook");
if (result == "grappling_hook") {
    LOG_INFO("Created grappling hook!");
}
```

### getItems
```cpp
const std::vector<Item>& getItems() const;
```
**Beskrivning:** Hämtar alla items i inventory.

**Returnerar:** Vector med alla items

### getMaxSlots
```cpp
int getMaxSlots() const;
```
**Beskrivning:** Hämtar max antal inventory slots.

**Returnerar:** Max antal slots (default: 20)

### setMaxSlots
```cpp
void setMaxSlots(int slots);
```
**Beskrivning:** Sätter max antal inventory slots.

**Parametrar:**
- `slots` - Nytt max antal

### getGold
```cpp
int getGold() const;
```
**Beskrivning:** Hämtar spelarens guld.

**Returnerar:** Antal guld

### addGold
```cpp
void addGold(int amount);
```
**Beskrivning:** Lägger till guld.

**Parametrar:**
- `amount` - Antal guld att lägga till

### removeGold
```cpp
bool removeGold(int amount);
```
**Beskrivning:** Tar bort guld.

**Parametrar:**
- `amount` - Antal guld att ta bort

**Returnerar:** `true` om spelaren hade tillräckligt guld

## JSON Format

### Items Definition
```json
{
  "items": [
    {
      "id": "rusty_key",
      "name": "Rusty Key",
      "description": "An old, rusty key. Looks like it might open something.",
      "icon": "assets/icons/key_rusty.png",
      "type": "key",
      "stackable": false
    },
    {
      "id": "health_potion",
      "name": "Health Potion",
      "description": "Restores 50 HP.",
      "icon": "assets/icons/potion_red.png",
      "type": "consumable",
      "stackable": true
    }
  ]
}
```

### Item Combinations
```json
{
  "combinations": [
    {
      "item1": "rope",
      "item2": "hook",
      "result": "grappling_hook",
      "removeInputs": true
    },
    {
      "item1": "empty_bottle",
      "item2": "water_source",
      "result": "water_bottle",
      "removeInputs": false
    }
  ]
}
```

## Item Usage Callbacks

Items kan ha användnings-callbacks:

```cpp
// Registrera callback för item-användning
InventorySystem::instance().registerUseCallback("health_potion", [](const std::string& target) {
    // Läk spelaren
    PlayerCharacter* player = /* ... */;
    player->heal(50);
    return true; // Användning lyckades
});

// Registrera kombinations-callback
InventorySystem::instance().registerCombineCallback("rope", "hook", []() {
    return "grappling_hook";
});
```

## Integration med InventoryState

```cpp
// Öppna inventory
game->pushState(std::make_unique<InventoryState>());

// I InventoryState, när användaren klickar på item
if (selectedItem) {
    InventorySystem::instance().useItem(selectedItem->id);
}
```

## Exempel: Pickup Item

```cpp
// I PlayState när spelare klickar på item-hotspot
if (hotspot->type == HotspotType::Item) {
    if (InventorySystem::instance().addItem(hotspot->id)) {
        // Ta bort hotspot från rum
        room->removeHotspot(hotspot->id);
        AudioManager::instance().playSound("pickup");
    } else {
        // Inventory fullt
        showMessage("Inventory is full!");
    }
}
```

## Se även

- [InventoryState](../states/InventoryState.md)
- [QuestSystem](QuestSystem.md)
- [Item JSON Schema](../../technical/data-formats.md#items)
