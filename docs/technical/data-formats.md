# Data Formats & JSON Schemas

Komplett referens för alla JSON-format i spelet.

---

## Rooms

### Schema
```json
{
  "id": "string (unique)",
  "name": "string",
  "background": "string (path)",
  "layers": [
    {
      "image": "string (path)",
      "zIndex": "number",
      "parallaxX": "number (0.0-1.0)",
      "parallaxY": "number (0.0-1.0)",
      "opacity": "number (0.0-1.0)"
    }
  ],
  "walkArea": {
    "minX": "number",
    "maxX": "number",
    "minY": "number",
    "maxY": "number",
    "scaleTop": "number (0.1-2.0)",
    "scaleBottom": "number (0.1-2.0)"
  },
  "hotspots": [
    {
      "id": "string (unique)",
      "name": "string",
      "type": "npc|item|exit|examine",
      "x": "number",
      "y": "number",
      "w": "number",
      "h": "number",
      "dialogId": "string (optional)",
      "targetRoom": "string (for exits)",
      "examineText": "string (optional)",
      "funnyFails": ["string", "..."]
    }
  ]
}
```

### Exempel
```json
{
  "id": "tavern",
  "name": "The Rusty Anchor",
  "background": "assets/backgrounds/tavern-inside.png",
  "layers": [
    {
      "image": "assets/backgrounds/tavern-inside.png",
      "zIndex": -1,
      "parallaxX": 1.0,
      "parallaxY": 1.0,
      "opacity": 1.0
    }
  ],
  "walkArea": {
    "minX": 0,
    "maxX": 640,
    "minY": 220,
    "maxY": 380,
    "scaleTop": 0.6,
    "scaleBottom": 1.0
  },
  "hotspots": [
    {
      "id": "bartender",
      "name": "Bartender",
      "type": "npc",
      "x": 80,
      "y": 270,
      "w": 40,
      "h": 60,
      "dialogId": "bartender_intro"
    },
    {
      "id": "door_street",
      "name": "Exit to Street",
      "type": "exit",
      "x": 580,
      "y": 260,
      "w": 50,
      "h": 90,
      "targetRoom": "street"
    }
  ]
}
```

---

## Dialogs

### Schema
```json
{
  "id": "string (unique)",
  "nodes": [
    {
      "id": "number",
      "speaker": "string",
      "text": "string",
      "choices": [
        {
          "text": "string",
          "next": "number (-1 = end)"
        }
      ]
    }
  ]
}
```

### Exempel
```json
{
  "id": "bartender_intro",
  "nodes": [
    {
      "id": 0,
      "speaker": "Bartender",
      "text": "Welcome to The Rusty Anchor! What can I get ya?",
      "choices": [
        {
          "text": "I'm looking for information.",
          "next": 1
        },
        {
          "text": "Just passing through.",
          "next": 2
        },
        {
          "text": "Goodbye.",
          "next": -1
        }
      ]
    },
    {
      "id": 1,
      "speaker": "Bartender",
      "text": "Information costs coin around here. 10 gold pieces.",
      "choices": [
        {
          "text": "Here's the gold.",
          "next": 3
        },
        {
          "text": "Never mind.",
          "next": -1
        }
      ]
    }
  ]
}
```

---

## Quests

### Schema
```json
{
  "id": "string (unique)",
  "title": "string",
  "description": "string",
  "objectives": [
    {
      "id": "string (unique)",
      "description": "string",
      "type": "talk|collect|deliver|goto|examine|custom",
      "target": "string (optional)"
    }
  ]
}
```

### Exempel
```json
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
    }
  ]
}
```

---

## Items

### Schema
```json
{
  "id": "string (unique)",
  "name": "string",
  "description": "string",
  "icon": "string (path)",
  "type": "generic|key|tool|consumable|quest",
  "stackable": "boolean"
}
```

### Exempel
```json
{
  "id": "rusty_key",
  "name": "Rusty Key",
  "description": "An old, rusty key. Looks like it might open something.",
  "icon": "assets/icons/key_rusty.png",
  "type": "key",
  "stackable": false
}
```

---

## NPCs

### Schema
```json
{
  "id": "string (unique)",
  "name": "string",
  "room": "string (room id)",
  "x": "number",
  "y": "number",
  "spriteSheet": "string (path)",
  "canMove": "boolean",
  "moveSpeed": "number",
  "dialogId": "string",
  "behavior": "idle|patrol|wander|schedule",
  "waypoints": [
    {
      "x": "number",
      "y": "number",
      "wait": "number (seconds)"
    }
  ],
  "schedule": [
    {
      "startHour": "number (0-23)",
      "endHour": "number (0-23)",
      "room": "string",
      "x": "number",
      "y": "number",
      "state": "idle|working|sleeping"
    }
  ]
}
```

### Exempel
```json
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
    {"x": 500, "y": 280, "wait": 1.0}
  ]
}
```

---

## Save Files

### Schema
```json
{
  "version": "string",
  "timestamp": "string (ISO 8601)",
  "playTime": "number (seconds)",
  "currentRoom": "string",
  "player": {
    "x": "number",
    "y": "number"
  },
  "inventory": ["string", "..."],
  "quests": {
    "quest_id": "not_started|active|completed|failed"
  },
  "flags": {
    "flag_name": "boolean"
  },
  "counters": {
    "counter_name": "number"
  }
}
```

### Exempel
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
  "inventory": ["rusty_key", "health_potion"],
  "quests": {
    "find_treasure": "active",
    "tavern_mystery": "completed"
  },
  "flags": {
    "talked_to_bartender": true,
    "cellar_unlocked": true
  },
  "counters": {
    "enemies_defeated": 5,
    "gold_collected": 250
  }
}
```

---

## Validering

### C++ Validation
```cpp
bool validateRoomData(const json& data) {
    if (!data.contains("id") || !data["id"].is_string()) {
        LOG_ERROR("Room missing 'id' field");
        return false;
    }
    
    if (!data.contains("walkArea")) {
        LOG_ERROR("Room missing 'walkArea'");
        return false;
    }
    
    auto& wa = data["walkArea"];
    if (!wa.contains("minX") || !wa.contains("maxX") ||
        !wa.contains("minY") || !wa.contains("maxY")) {
        LOG_ERROR("WalkArea missing coordinates");
        return false;
    }
    
    return true;
}
```

### JSON Schema (för externa verktyg)
```json
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "title": "Room",
  "type": "object",
  "required": ["id", "name", "walkArea"],
  "properties": {
    "id": {"type": "string"},
    "name": {"type": "string"},
    "walkArea": {
      "type": "object",
      "required": ["minX", "maxX", "minY", "maxY"],
      "properties": {
        "minX": {"type": "number"},
        "maxX": {"type": "number"},
        "minY": {"type": "number"},
        "maxY": {"type": "number"},
        "scaleTop": {"type": "number", "minimum": 0.1, "maximum": 2.0},
        "scaleBottom": {"type": "number", "minimum": 0.1, "maximum": 2.0}
      }
    }
  }
}
```

---

## Filplatser

```
assets/data/
├── rooms.json       # Alla rum
├── dialogs.json     # Alla dialoger
├── quests.json      # Alla quests
├── items.json       # Alla items
└── npcs.json        # Alla NPCs

assets/saves/
├── slot1.json
├── slot2.json
├── slot3.json
├── quicksave.json
└── autosave.json
```

---

## Best Practices

### 1. Använd Unique IDs
Alla IDs måste vara unika inom sin kategori:
```json
"id": "tavern_bartender"  // ✓ Bra
"id": "bartender"          // ✗ Kan kollidera
```

### 2. Relativa Paths
Använd relativa paths från projektrot:
```json
"background": "assets/backgrounds/tavern.png"  // ✓
"background": "C:/Users/.../tavern.png"        // ✗
```

### 3. Default Values
Ange default values för optionella fält:
```cpp
float scaleTop = data.value("scaleTop", 0.5f);
float scaleBottom = data.value("scaleBottom", 1.0f);
```

### 4. Kommentarer (inte standard JSON)
För utveckling, använd JSONC eller separata kommentarer:
```json
{
  "id": "tavern",
  "_comment": "Main tavern room, starting location",
  "name": "The Rusty Anchor"
}
```

### 5. Versionering
Inkludera version i data-filer:
```json
{
  "version": "1.0",
  "rooms": [...]
}
```

---

## Se även

- [DataLoader](../api/data/DataLoader.md)
- [GameData.h](../api/data/GameData.md)
- [SaveSystem](../api/systems/SaveSystem.md)
