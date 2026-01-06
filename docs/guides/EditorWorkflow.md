# Editor Workflow Guide

**Audience:** Users familiar with Node Scene Graph basics  
**Time:** 20 minutes  
**Difficulty:** Intermediate

---

## Overview

The Node Scene Graph Editor provides three powerful panels for visual editing:

1. **SceneGraphPanel** - Hierarchical tree view with drag-and-drop
2. **LayerEditorPanel** - Layer management with parallax preview
3. **TileMapEditorPanel** - Interactive tilemap painting

---

## Opening the Editor

```bash
cd build/Release
./RetroEditor.exe
```

The editor opens in windowed mode (1600x900) with a docking layout.

---

## Panel Layout

```
┌─────────────────────────────────────────────────────────┐
│ Menu Bar: File | Edit | View | Tools                    │
├──────────┬────────────────────────────┬─────────────────┤
│          │                            │                 │
│ Hierarchy│      Viewport              │   Properties    │
│          │                            │                 │
│  Scene   │   [Visual Preview]         │   Inspector     │
│  Graph   │                            │                 │
│          │                            │                 │
│  Layers  ├────────────────────────────┤   Asset         │
│          │                            │   Browser       │
│  TileMap │   Graph Editors            │                 │
│          │   (Dialog/Quest/Behavior)  │                 │
├──────────┴────────────────────────────┴─────────────────┤
│ Console: Logs and messages                              │
└─────────────────────────────────────────────────────────┘
```

All panels are **dockable** - drag the title bar to reposition!

---

## SceneGraphPanel - Node Tree Editor

### Opening the Panel

**View → Scene Graph** or click the Scene Graph tab in the center dock.

### Features

#### 1. Hierarchical Tree View

The panel shows all nodes in your scene as an expandable tree:

```
🌳 Scene: Level1
├── 🖼️ Sky (inactive)
├── 🎮 GameWorld
│   ├── 👤 Player
│   ├── ⚔️ Sword (hidden)
│   └── 👹 Enemy
└── 📊 UI
    └── 📝 Score
```

**Icons:**
- 🌳 = Scene/Container
- 🖼️ = Sprite
- 🎮 = Node2D
- 👤 = Character
- 📝 = Label

#### 2. Node Selection

**Click** any node to select it. Selected nodes are highlighted.

**Result:** Properties panel updates to show node properties.

#### 3. Drag-and-Drop Re-parenting

**To move a node to a new parent:**

1. Click and hold on the node you want to move
2. Drag it over the new parent node
3. Release to drop

**Example:** Move "Sword" from Scene root to Player
```
Before:                After:
Scene                  Scene
├── Player             ├── Player
└── Sword              │   └── Sword  ← Now follows player!
```

**Important:** The node will now move with its new parent!

#### 4. Context Menu

**Right-click** any node for options:

| Option | Description |
|--------|-------------|
| Rename | Change node name (TODO) |
| Duplicate | Copy the node (TODO) |
| Active | Toggle active state ✓ |
| Visible | Toggle visibility ✓ |
| Delete | Remove node (TODO) |

**Active vs Visible:**
- **Active = false** → Node doesn't update (frozen)
- **Visible = false** → Node doesn't render (invisible)

#### 5. Filter Options

**Show Inactive** checkbox - Toggle to hide/show inactive nodes.

**Use case:** Hide disabled nodes to declutter the tree.

---

## LayerEditorPanel - Layer Management

### Opening the Panel

**View → Layer Editor** or click the Layer Editor tab.

### What Are Layers?

Layers organize rendering order and parallax effects:

```
Layer Stack (bottom to top):
┌─────────────────────────┐
│ UI (z: 1000)            │ ← Always on top
├─────────────────────────┤
│ Foreground (z: 100)     │ ← In front of player
├─────────────────────────┤
│ Main (z: 0)             │ ← Player layer
├─────────────────────────┤
│ Background (z: -100)    │ ← Behind everything
└─────────────────────────┘
```

### Features

#### 1. Layer List

Shows all layers with type icons:

- **B** = Background (parallax)
- **M** = Main (gameplay)
- **F** = Foreground (overlay)
- **U** = UI (screen-space)

**Example:**
```
B Sky (z: -100)
M GameWorld (z: 0)
F Clouds (z: 100)
U HUD (z: 1000)
```

**Click** a layer to select it.

#### 2. Layer Properties

When a layer is selected, you can edit:

**Z-Index** - Rendering order (drag to change)
```cpp
-100 = Back
   0 = Middle
1000 = Front
```

**Opacity** - Transparency (0.0 - 1.0)
```cpp
0.0 = Fully transparent
0.5 = Semi-transparent
1.0 = Fully opaque
```

**Active** - Enable/disable layer updates

**Visible** - Show/hide layer

#### 3. Add New Layer

**Click "Add Layer"** button to open the dialog:

1. **Name:** Enter layer name (e.g., "Mountains")
2. **Type:** Select from dropdown
   - Background (for parallax)
   - Main (for gameplay)
   - Foreground (for overlays)
   - UI (for interface)
3. **Z-Index:** Set rendering order
4. **Click "Create"**

**Result:** New layer appears in the list!

#### 4. Delete Layer

1. Select the layer
2. Scroll down in Properties
3. Click **"Delete Layer"** button

**Warning:** This removes the layer and all its nodes!

#### 5. Parallax Preview

**For Background layers only:**

1. Select a background layer
2. Check **"Parallax Preview"**
3. Use the **Scroll Position** slider

**What you see:**
- Visual representation of how the layer scrolls
- Different layers move at different speeds
- Simulates the parallax effect in-game

**Example:**
```
Scroll = 0:     Scroll = 500:
[Sky    ]       [    Sky]      ← Moves slower (0.3x)
[Mountains]     [  Mountains]  ← Moves medium (0.5x)
[Trees  ]       [ Trees ]      ← Moves faster (0.7x)
```

---

## TileMapEditorPanel - Interactive Painting

### Opening the Panel

**View → TileMap Editor** or click the TileMap Editor tab.

### What Is a TileMap?

A grid of tiles (small images) that form your game world:

```
Grid (20x15, 32px tiles):
┌─┬─┬─┬─┬─┬─┬─┬─┐
│1│1│1│1│1│1│1│1│  1 = Grass
├─┼─┼─┼─┼─┼─┼─┼─┤
│0│0│2│2│2│0│0│0│  2 = Wall
├─┼─┼─┼─┼─┼─┼─┼─┤
│0│0│2│0│2│0│0│0│  0 = Empty
└─┴─┴─┴─┴─┴─┴─┴─┘
```

### Features

#### 1. Toolbar

**Paint Mode** - Click tiles to paint
**Erase Mode** - Click tiles to erase
**Collision Mode** - Mark tiles as solid (for physics)

**Toggle between modes:**
```
○ Paint   ● Erase   ☐ Collision Mode
```

#### 2. Tile Palette

4x4 grid showing available tiles:

```
┌──┬──┬──┬──┐
│1 │2 │3 │4 │
├──┼──┼──┼──┤
│5 │6 │7 │8 │
├──┼──┼──┼──┤
│9 │10│11│12│
├──┼──┼──┼──┤
│13│14│15│16│
└──┴──┴──┴──┘
```

**Click** a tile to select it. Selected tile is highlighted.

#### 3. Interactive Grid

The main editing area shows your tilemap:

**Visual:**
- Each cell shows the tile number
- Empty cells are dark gray
- Filled cells are blue
- Grid lines separate cells

**Painting:**

1. Select a tile from the palette
2. Choose Paint or Erase mode
3. **Click** on the grid to paint one tile
4. **Click and drag** to paint multiple tiles

**Example workflow:**
```
1. Select tile #1 (grass)
2. Click Paint mode
3. Drag across bottom row
   → Creates grass floor!

4. Select tile #2 (wall)
5. Drag to create walls
   → Creates boundaries!
```

#### 4. Auto-Tiling

**Automatic tile variant selection based on neighbors!**

When you paint a tile, the editor checks its neighbors:

```
Neighbors:
  ↑
← T →
  ↓
```

**Algorithm:**
- Checks 4 cardinal directions (up, down, left, right)
- Calculates neighbor bitmask
- Adjusts tile variant automatically

**Example:**
```
Before auto-tile:    After auto-tile:
┌─┬─┬─┐             ┌─┬─┬─┐
│ │1│ │             │ │╔│ │  ← Top edge
├─┼─┼─┤             ├─┼─┼─┤
│1│1│1│     →       │═│█│═│  ← Center + sides
├─┼─┼─┤             ├─┼─┼─┤
│ │1│ │             │ │╚│ │  ← Bottom edge
└─┴─┴─┘             └─┴─┴─┘
```

**Result:** Tiles automatically get corners, edges, and fills!

---

## Common Workflows

### Workflow 1: Create a New Scene

1. **File → New Scene**
2. **Scene Graph Panel:**
   - Right-click root → Add Node
   - Name it "Background"
3. **Properties Panel:**
   - Set texture
   - Set position (0, 0)
   - Set z-index (-100)
4. **Repeat** for player, enemies, UI
5. **File → Save Scene**

---

### Workflow 2: Build a Parallax Background

1. **Layer Editor → Add Layer**
   - Name: "Sky"
   - Type: Background
   - Z-Index: -200

2. **Add Layer**
   - Name: "Mountains"
   - Type: Background
   - Z-Index: -100

3. **Add Layer**
   - Name: "Trees"
   - Type: Background
   - Z-Index: -50

4. **Test with Parallax Preview:**
   - Select each layer
   - Check "Parallax Preview"
   - Adjust scroll to see effect

5. **Assign nodes to layers** in Scene Graph

---

### Workflow 3: Paint a TileMap Level

1. **Scene Graph → Add TileMapLayer**
   - Name: "Ground"
   - Size: 30x20
   - Tile size: 32

2. **TileMap Editor → Select tilemap**

3. **Paint the floor:**
   - Select tile #1 (grass)
   - Paint mode
   - Drag across bottom rows

4. **Paint walls:**
   - Select tile #2 (wall)
   - Paint around edges

5. **Enable collision:**
   - Check "Collision Mode"
   - Click wall tiles to mark as solid

6. **Auto-tiling adjusts corners automatically!**

7. **File → Save**

---

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| Ctrl+S | Save all |
| Ctrl+Z | Undo (TODO) |
| Ctrl+Y | Redo (TODO) |
| Delete | Delete selected node (TODO) |
| F2 | Rename selected node (TODO) |
| Ctrl+D | Duplicate selected node (TODO) |

---

## Tips & Tricks

### Tip 1: Use Z-Index for Layering

**Problem:** UI appears behind the game world

**Solution:**
```cpp
background->setZIndex(-100);
gameWorld->setZIndex(0);
ui->setZIndex(1000);  // Always on top!
```

### Tip 2: Group Related Nodes

**Problem:** Scene tree is messy

**Solution:** Create container nodes
```
Scene
└── GameWorld (container)
    ├── Player
    ├── Enemies (container)
    │   ├── Enemy1
    │   └── Enemy2
    └── Items (container)
```

### Tip 3: Use Inactive for Temporary Disable

**Problem:** Want to test without an object

**Solution:** Right-click → Uncheck "Active"
- Node stays in tree
- Doesn't update or render
- Easy to re-enable later

### Tip 4: Preview Parallax Before Building

**Problem:** Don't know if parallax looks good

**Solution:**
1. Create layers in Layer Editor
2. Use Parallax Preview
3. Adjust z-index and parallax factors
4. Build scene when satisfied

---

## Troubleshooting

### Problem: Can't see my node in Scene Graph

**Solutions:**
- Check "Show Inactive" filter
- Expand parent nodes (click arrow)
- Verify node was added to scene

### Problem: Drag-and-drop doesn't work

**Solutions:**
- Make sure you're dragging onto a valid parent
- Can't make a node its own child
- Can't make a node a child of its own child

### Problem: TileMap painting doesn't work

**Solutions:**
- Select a tilemap first (in Scene Graph)
- Make sure you selected a tile from palette
- Check if you're in Paint mode (not Erase)

### Problem: Parallax preview shows nothing

**Solutions:**
- Only works for Background type layers
- Make sure layer has nodes assigned
- Try adjusting scroll slider

---

## Next Steps

Now that you know the editor, try:

1. **[Scene Building](SceneBuilding.md)** - Build complex multi-layer scenes
2. **[TileMap Creation](TileMapCreation.md)** - Create detailed game worlds
3. **[Physics Basics](PhysicsBasics.md)** - Add collision and movement

---

**Happy editing!** 🎨
