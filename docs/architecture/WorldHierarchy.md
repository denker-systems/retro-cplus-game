# World Hierarchy Architecture

## Structure

```
World (Whole Game)
├─ Level 1: "Act 1 - The Tavern District"
│  ├─ Scene: "tavern_interior"
│  │  ├─ Background (Sprite)
│  │  ├─ WalkArea (CollisionShape)
│  │  ├─ Hotspots/
│  │  │  ├─ door_hotspot (InteractiveArea)
│  │  │  └─ bartender_hotspot (InteractiveArea)
│  │  ├─ Objects/
│  │  │  └─ mug (Sprite)
│  │  ├─ NPCs/
│  │  │  └─ bartender (AnimatedSprite)
│  │  ├─ PlayerSpawn (Marker)
│  │  └─ UI/
│  │     └─ room_label (Label)
│  │
│  ├─ Scene: "tavern_exterior"
│  └─ Scene: "market_square"
│
├─ Level 2: "Act 2 - The Forest"
│  ├─ Scene: "forest_entrance"
│  └─ Scene: "forest_clearing"
│
└─ Level 3: "Act 3 - The Castle"
   ├─ Scene: "castle_gate"
   └─ Scene: "throne_room"
```

## Classes

### World
- Contains multiple Levels
- Manages global state (inventory, quests, flags)
- Handles level transitions

### Level
- Logical grouping of related Scenes
- Contains multiple Scenes
- Handles scene transitions within level
- Can have level-specific music, lighting

### Scene (Room)
- Actual playable area
- Contains all game objects as Nodes
- Has camera, player spawn, walkable area
- Rendered by viewport

## Node Types Within Scene

| Node Type | Purpose | Example |
|-----------|---------|---------|
| `Sprite` | Static images | Background, objects |
| `AnimatedSprite` | Animated images | NPCs, animated objects |
| `InteractiveArea` | Clickable hotspots | Doors, items, NPCs |
| `WalkArea` | Walkable polygon | Where player can move |
| `PlayerSpawn` | Spawn marker | Initial player position |
| `Label` | Text | Room name, debug info |
| `TileMapLayer` | Tile-based terrain | Ground tiles |

## Data Flow

```
JSON Data (rooms.json)
    ↓
RoomData struct
    ↓
SceneLoader::convertRoomToScene()
    ↓
Scene with Node hierarchy
    ↓
ViewportPanel renders Scene
```

## Implementation Plan

### Phase 1: Converter (Immediate)
- Keep RoomData as source of truth
- Create `SceneLoader::convertRoomToScene(RoomData)`
- Convert on-the-fly for editor

### Phase 2: Native Scene Format (Future)
- Save Scenes directly as JSON
- Deprecate RoomData
- Use Scene serialization

## Example Scene JSON (Future)

```json
{
  "scene": "tavern_interior",
  "level": "act1_tavern_district",
  "nodes": [
    {
      "type": "Sprite",
      "name": "Background",
      "texture": "tavern_bg.png",
      "position": [0, 0],
      "z_index": -300
    },
    {
      "type": "InteractiveArea",
      "name": "door_hotspot",
      "position": [50, 100],
      "size": [80, 120],
      "action": "goto:tavern_exterior",
      "cursor": "exit"
    },
    {
      "type": "WalkArea",
      "name": "WalkArea",
      "polygon": [
        [100, 300], [500, 300], [500, 380], [100, 380]
      ],
      "z_index": -50
    },
    {
      "type": "AnimatedSprite",
      "name": "bartender",
      "texture": "bartender_sprite.png",
      "position": [300, 200],
      "frame_size": [32, 48],
      "animations": {
        "idle": {"frames": [0,1,2,3], "fps": 4}
      }
    },
    {
      "type": "Marker",
      "name": "PlayerSpawn",
      "position": [320, 300]
    }
  ]
}
```

## Benefits

1. **Unified System** - One way to represent everything
2. **Editor Sees Game** - No disconnect between editor and runtime
3. **Flexible** - Easy to add new node types
4. **Hierarchical** - Parent-child relationships work naturally
5. **Transformable** - Use Node2D transforms everywhere

## Migration Path

1. ✅ Keep RoomData unchanged
2. ✅ Create Scene converter
3. ✅ Update ViewportPanel to render Scene
4. ✅ Update game to use Scenes
5. ⏳ Add Level management
6. ⏳ Add World management
7. ⏳ Eventually: native Scene JSON format
