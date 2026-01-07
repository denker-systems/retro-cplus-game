---
trigger: glob
globs: ["assets/data/*.json"]
description: JSON data formats for game content
---

# JSON Formats

> Data-driven game content

## Room Format

```json
{
  "id": "tavern",
  "name": "The Rusty Anchor",
  "background": "backgrounds/tavern.png",
  "music": "music/tavern_theme.ogg",
  "walkArea": {
    "minX": 50, "maxX": 590,
    "minY": 260, "maxY": 380
  },
  "hotspots": [...],
  "actors": [...]
}
```

---

## Hotspot Format

```json
{
  "id": "door_exit",
  "name": "Door",
  "type": "exit",
  "rect": { "x": 100, "y": 250, "w": 40, "h": 80 },
  "targetRoom": "street",
  "targetSpawn": "from_tavern"
}
```

**Types:** `exit`, `item`, `examine`, `npc`

---

## Dialog Format

```json
{
  "id": "bartender_intro",
  "nodes": [
    {
      "id": 0,
      "speaker": "Bartender",
      "text": "Welcome, stranger!",
      "choices": [
        { "text": "Hello", "next": 1 },
        { "text": "Goodbye", "next": -1 }
      ]
    }
  ]
}
```

---

## Item Format

```json
{
  "id": "rusty_key",
  "name": "Rusty Key",
  "description": "An old key covered in rust.",
  "icon": "sprites/items/key.png",
  "combinable": ["oil_can"],
  "usableOn": ["locked_door"]
}
```

---

## Quest Format

```json
{
  "id": "find_key",
  "title": "Find the Key",
  "description": "Search the tavern for a key.",
  "objectives": [
    { "id": "obj1", "text": "Find the key", "type": "has_item", "target": "rusty_key" }
  ],
  "reward": { "xp": 50 }
}
```
