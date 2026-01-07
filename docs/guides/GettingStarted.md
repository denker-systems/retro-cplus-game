# Getting Started with Node Scene Graph

**Audience:** New users  
**Time:** 15 minutes  
**Difficulty:** Beginner

---

## What Is Node Scene Graph?

Node Scene Graph is a system for building 2D games by organizing all objects in a hierarchical tree structure - just like folders on your computer!

### Core Concept

**Think of a tree:**
```
Game
├── Background (sky)
├── GameWorld
│   ├── Player
│   ├── Enemy 1
│   └── Enemy 2
└── UI
    ├── Health Bar
    └── Score
```

Each "node" in the tree is an object in your game. Just like in a family, children inherit properties from their parents!

---

## Your First Scene

### Step 1: Create a Scene

A **Scene** is like a level in your game.

```cpp
auto scene = std::make_unique<Scene>("Level1");
```

**What does this mean?**
- `Scene` = A container for all objects in a level
- `"Level1"` = The name of your level
- `std::make_unique` = C++ way to create objects (ignore the details for now!)

---

### Step 2: Add a Background

```cpp
auto background = std::make_unique<Sprite>("Sky");
background->setTexture(skyTexture);
background->setPosition(0, 0);
scene->addChild(std::move(background));
```

**What's happening here?**
1. Creates a sprite (image) named "Sky"
2. Sets which image it should display
3. Places it in the top-left corner (0, 0)
4. Adds it to the scene

---

### Step 3: Add a Player

```cpp
auto player = std::make_unique<Sprite>("Player");
player->setTexture(playerTexture);
player->setPosition(100, 200);
scene->addChild(std::move(player));
```

Now you have a background AND a player!

---

## Hierarchy - Children Follow Parents

The cool thing about Node Scene Graph is that children **follow** their parents.

### Example: Weapon Following the Player

```cpp
// Create the player
auto player = std::make_unique<Sprite>("Player");
player->setPosition(100, 200);

// Create a weapon
auto weapon = std::make_unique<Sprite>("Sword");
weapon->setPosition(10, 5);  // Relative to the player!

// Make the weapon a child of the player
player->addChild(std::move(weapon));

// Add the player to the scene
scene->addChild(std::move(player));
```

**Result:**
- Player is at position (100, 200)
- Weapon is at position (110, 205) - 10 pixels right and 5 pixels down from the player
- When the player moves, the weapon follows automatically! ✨

---

## Common Node Types

### Sprite - Display an Image

```cpp
auto sprite = std::make_unique<Sprite>("MyImage");
sprite->setTexture(texture);
sprite->setPosition(x, y);
```

**Usage:** Characters, objects, backgrounds

---

### AnimatedSprite - Animated Image

```cpp
auto character = std::make_unique<AnimatedSprite>("Hero");
character->addAnimation("walk", {0, 1, 2, 3}, 0.1f);
character->play("walk");
```

**Usage:** Walking characters, waving flags, etc.

---

### Label - Text

```cpp
auto text = std::make_unique<Label>("Score");
text->setText("Score: 100");
text->setFontSize(24);
text->setColor({255, 255, 0, 255});  // Yellow text
```

**Usage:** Score, dialogues, menus

---

### TileMapLayer - Grid of Tiles

```cpp
auto tilemap = std::make_unique<TileMapLayer>("Ground", 20, 15, 32);
tilemap->setTile(5, 10, 1);  // Set tile number 1 at position (5, 10)
```

**Usage:** Game world, walls, floors

---

## Z-Index - What Renders First?

**Problem:** How does the game know what should be in front and behind?

**Solution:** Z-Index!

```cpp
background->setZIndex(-100);  // Furthest back
player->setZIndex(0);         // Middle
ui->setZIndex(1000);          // Furthest front
```

**Rule:** Lower number = further back, higher number = further front

---

## Camera - What Does the Player See?

A **Camera** determines which part of the world is shown on screen.

```cpp
auto camera = std::make_unique<Camera2D>("MainCamera");
camera->setTarget(player.get());  // Follow the player
camera->setZoom(2.0f);            // Zoom in 2x
scene->setCamera(std::move(camera));
```

**Result:** The camera follows the player and shows everything 2x larger!

---

## Practical Example: A Complete Game

```cpp
// 1. Create the scene
auto scene = std::make_unique<Scene>("MyGame");

// 2. Add background
auto sky = std::make_unique<Sprite>("Sky");
sky->setTexture(skyTexture);
sky->setZIndex(-100);
scene->addChild(std::move(sky));

// 3. Add game world
auto ground = std::make_unique<TileMapLayer>("Ground", 30, 20, 32);
ground->setZIndex(-50);
scene->addChild(std::move(ground));

// 4. Add player
auto player = std::make_unique<Sprite>("Player");
player->setTexture(playerTexture);
player->setPosition(100, 300);
player->setZIndex(0);
scene->addChild(std::move(player));

// 5. Add UI
auto scoreLabel = std::make_unique<Label>("Score");
scoreLabel->setText("Score: 0");
scoreLabel->setPosition(10, 10);
scoreLabel->setZIndex(1000);
scene->addChild(std::move(scoreLabel));

// 6. Add camera
auto camera = std::make_unique<Camera2D>("MainCamera");
camera->setTarget(player.get());
scene->setCamera(std::move(camera));

// 7. Run the scene!
world.addScene("game", std::move(scene));
world.changeScene("game");
```

**Congratulations! You've created a complete game!** 🎉

---

## Frequently Asked Questions

### Why `std::move()`?

**Short answer:** It's C++ syntax to "move" ownership of an object.

**Longer answer:** When you create a node, YOU own it. When you do `addChild(std::move(node))` you give ownership to the parent. Now the parent owns the node and takes care of it.

---

### What is `get()`?

```cpp
camera->setTarget(player.get());
```

**Answer:** `get()` gives you a "pointer" to the object without giving away ownership. The camera just wants to LOOK at the player, not OWN it.

---

### Can I remove a node?

Yes! Parents can remove their children:

```cpp
parent->removeChild(child);  // Removes the child
```

---

### How do I update all nodes?

The scene does it automatically!

```cpp
scene->update(deltaTime);  // Updates ALL nodes in the tree
scene->render(renderer);   // Renders ALL nodes in correct order
```

---

## Next Steps

Now that you understand the basics, try:

1. **[Editor Workflow](EditorWorkflow.md)** - Learn to use the editor
2. **[Scene Building](SceneBuilding.md)** - Build more complex scenes
3. **[TileMap Creation](TileMapCreation.md)** - Create game worlds with tiles

---

## Quick Reference

| What | Code |
|------|------|
| Create sprite | `auto s = std::make_unique<Sprite>("Name");` |
| Set position | `node->setPosition(x, y);` |
| Set z-index | `node->setZIndex(100);` |
| Add child | `parent->addChild(std::move(child));` |
| Remove child | `parent->removeChild(child);` |
| Hide node | `node->setVisible(false);` |
| Disable node | `node->setActive(false);` |

---

**Good luck with your game!** 🚀
