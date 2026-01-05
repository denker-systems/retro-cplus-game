---
trigger: glob
globs: ["assets/data/*.json"]
---

# JSON Data Format

<room_format>
`json
{
  "id": "room_id",
  "name": "Display Name",
  "background": "backgrounds/room.png",
  "walkArea": { "x": 0, "y": 200, "w": 640, "h": 175 },
  "hotspots": [
    {
      "id": "hotspot_id",
      "type": "item|npc|exit|examine",
      "rect": { "x": 100, "y": 250, "w": 40, "h": 60 },
      "name": "Display name",
      "dialog": "dialog_id",
      "item": "item_id",
      "targetRoom": "other_room"
    }
  ],
  "npcs": ["npc_id1", "npc_id2"],
  "music": "sounds/room_music.ogg"
}
`
</room_format>

<dialog_format>
`json
{
  "id": "dialog_id",
  "nodes": [
    {
      "id": 0,
      "speaker": "NPC Name",
      "text": "Dialog text here.",
      "choices": [
        { "text": "Choice 1", "next": 1 },
        { "text": "Choice 2", "next": 2, "condition": "has_item:key" },
        { "text": "Goodbye", "next": -1 }
      ]
    }
  ]
}
`
</dialog_format>

<quest_format>
`json
{
  "id": "quest_id",
  "title": "Quest Title",
  "description": "What to do.",
  "objectives": [
    { "id": "obj1", "text": "Find the key", "type": "has_item", "target": "key" },
    { "id": "obj2", "text": "Talk to bartender", "type": "dialog_complete", "target": "bartender_intro" }
  ],
  "reward": { "item": "gold_coin", "xp": 100 }
}
`
</quest_format>

<item_format>
`json
{
  "id": "item_id",
  "name": "Item Name",
  "description": "Description shown when examined.",
  "icon": "sprites/items/item.png",
  "combinable": ["other_item_id"],
  "usableOn": ["hotspot_id"]
}
`
</item_format>
