# Scene Building Guide

**Audience:** Intermediate users  
**Time:** 30 minutes  
**Difficulty:** Intermediate

---

## What You'll Build

A complete 2D platformer scene with:
- Parallax background (3 layers)
- TileMap game world
- Animated player character
- Camera that follows the player
- UI overlay

---

## Prerequisites

- Completed [Getting Started](GettingStarted.md)
- Familiar with [Editor Workflow](EditorWorkflow.md)
- Have texture assets ready

---

## Step 1: Plan Your Scene

### Scene Structure

```
Scene: "Level1"
├── BackgroundLayers (container)
│   ├── Sky (ParallaxLayer, z: -300)
│   ├── Mountains (ParallaxLayer, z: -200)
│   └── Trees (ParallaxLayer, z: -100)
├── GameWorld (container, z: 0)
│   ├── Ground (TileMapLayer)
│   ├── Player (AnimatedSprite)
│   └── Enemies (container)
│       ├── Enemy1
│       └── Enemy2
└── UI (container, z: 1000)
    ├── HealthBar
    └── ScoreLabel
```

### Layer Plan

| Layer | Type | Z-Index | Purpose |
|-------|------|---------|---------|
| Sky | Background | -300 | Slowest parallax |
| Mountains | Background | -200 | Medium parallax |
| Trees | Background | -100 | Fast parallax |
| Main | Main | 0 | Gameplay |
| UI | UI | 1000 | Always on top |

---

## Step 2: Create the Scene

```cpp
auto scene = std::make_unique<Scene>("Level1");
```

**In editor:**
1. File → New Scene
2. Name: "Level1"

---

## Step 3: Build Background Layers

### 3.1 Create Layer Manager

```cpp
auto layerMgr = std::make_unique<LayerManager>();
```

### 3.2 Add Layers

**In Layer Editor:**

1. **Add Layer:**
   - Name: "Sky"
   - Type: Background
   - Z-Index: -300

2. **Add Layer:**
   - Name: "Mountains"
   - Type: Background
   - Z-Index: -200

3. **Add Layer:**
   - Name: "Trees"
   - Type: Background
   - Z-Index: -100

4. **Add Layer:**
   - Name: "Main"
   - Type: Main
   - Z-Index: 0

5. **Add Layer:**
   - Name: "UI"
   - Type: UI
   - Z-Index: 1000

### 3.3 Create Parallax Sprites

**Sky layer:**
```cpp
auto sky = std::make_unique<ParallaxLayer>("Sky");
sky->setTexture(skyTexture);
sky->setParallaxFactor(0.1f, 0.1f);  // Very slow
sky->setRepeat(true, false);  // Repeat horizontally
sky->setZIndex(-300);
scene->addChild(std::move(sky));
```

**Mountains layer:**
```cpp
auto mountains = std::make_unique<ParallaxLayer>("Mountains");
mountains->setTexture(mountainsTexture);
mountains->setParallaxFactor(0.3f, 0.2f);  // Slow
mountains->setRepeat(true, false);
mountains->setZIndex(-200);
scene->addChild(std::move(mountains));
```

**Trees layer:**
```cpp
auto trees = std::make_unique<ParallaxLayer>("Trees");
trees->setTexture(treesTexture);
trees->setParallaxFactor(0.6f, 0.4f);  // Fast
trees->setRepeat(true, false);
trees->setZIndex(-100);
scene->addChild(std::move(trees));
```

**Parallax factors:**
- Lower = slower movement (further away)
- Higher = faster movement (closer)
- 1.0 = moves with camera (no parallax)

---

## Step 4: Create Game World

### 4.1 Add TileMap

```cpp
auto ground = std::make_unique<TileMapLayer>("Ground", 60, 40, 32);
ground->setTexture(tilesetTexture);
ground->setZIndex(0);
scene->addChild(std::move(ground));
```

**In TileMap Editor:**

1. Select "Ground" tilemap
2. Paint the floor:
   - Select grass tile
   - Paint bottom 5 rows
3. Paint platforms:
   - Select platform tile
   - Paint floating platforms
4. Paint walls:
   - Select wall tile
   - Paint boundaries
5. Enable collision:
   - Check "Collision Mode"
   - Click solid tiles

**Auto-tiling creates corners automatically!**

### 4.2 Add Player

```cpp
auto player = std::make_unique<AnimatedSprite>("Player");
player->setTexture(playerTexture);
player->setPosition(100, 300);
player->setZIndex(10);  // Above ground

// Add animations
player->addAnimation("idle", {0, 1, 2, 3}, 0.15f);
player->addAnimation("walk", {4, 5, 6, 7}, 0.1f);
player->addAnimation("jump", {8, 9}, 0.1f);
player->play("idle");

scene->addChild(std::move(player));
```

**Animation frames:**
- Frame 0-3: Idle animation
- Frame 4-7: Walking animation
- Frame 8-9: Jumping animation

### 4.3 Add Enemies (Optional)

```cpp
// Create container
auto enemies = std::make_unique<Node>("Enemies");
enemies->setZIndex(10);

// Add enemy 1
auto enemy1 = std::make_unique<AnimatedSprite>("Enemy1");
enemy1->setTexture(enemyTexture);
enemy1->setPosition(400, 300);
enemy1->addAnimation("patrol", {0, 1, 2, 3}, 0.12f);
enemy1->play("patrol");
enemies->addChild(std::move(enemy1));

// Add enemy 2
auto enemy2 = std::make_unique<AnimatedSprite>("Enemy2");
enemy2->setTexture(enemyTexture);
enemy2->setPosition(800, 300);
enemy2->addAnimation("patrol", {0, 1, 2, 3}, 0.12f);
enemy2->play("patrol");
enemies->addChild(std::move(enemy2));

scene->addChild(std::move(enemies));
```

**Why use a container?**
- Organize related nodes
- Move/hide all enemies at once
- Cleaner scene tree

---

## Step 5: Add Camera

```cpp
auto camera = std::make_unique<Camera2D>("MainCamera");

// Follow the player
camera->setTarget(player.get());

// Set zoom (1.0 = normal, 2.0 = 2x closer)
camera->setZoom(1.5f);

// Set camera limits (don't show outside world)
camera->setLimits({0, 0, 1920, 1080});

// Smooth following
camera->setFollowSmooth(0.1f);  // 0 = instant, 1 = very slow

scene->setCamera(std::move(camera));
```

**Camera limits:**
```
World bounds:
┌─────────────────────┐
│                     │ ← Camera can't go outside
│   [Camera View]     │
│                     │
└─────────────────────┘
  (0,0)        (1920,1080)
```

---

## Step 6: Add UI Layer

### 6.1 Health Bar

```cpp
auto healthBar = std::make_unique<Sprite>("HealthBar");
healthBar->setTexture(healthBarTexture);
healthBar->setPosition(20, 20);  // Top-left corner
healthBar->setZIndex(1000);
scene->addChild(std::move(healthBar));
```

### 6.2 Score Label

```cpp
auto scoreLabel = std::make_unique<Label>("Score");
scoreLabel->setText("Score: 0");
scoreLabel->setFontSize(24);
scoreLabel->setColor({255, 255, 255, 255});  // White
scoreLabel->setPosition(20, 60);
scoreLabel->setZIndex(1000);
scene->addChild(std::move(scoreLabel));
```

**UI positioning:**
- Use screen coordinates (not world)
- Top-left = (0, 0)
- Always set z-index high (1000+)

---

## Step 7: Test Parallax Effect

### In Layer Editor:

1. Select "Sky" layer
2. Check "Parallax Preview"
3. Move scroll slider
4. Observe: Sky moves slowly
5. Select "Mountains" layer
6. Observe: Mountains move faster
7. Select "Trees" layer
8. Observe: Trees move fastest

**Adjust parallax factors if needed:**
- Too slow? Increase factor
- Too fast? Decrease factor
- Want no parallax? Set to 1.0

---

## Step 8: Organize Scene Tree

### Before (messy):
```
Scene
├── Sky
├── Mountains
├── Trees
├── Ground
├── Player
├── Enemy1
├── Enemy2
├── HealthBar
└── ScoreLabel
```

### After (organized):
```
Scene
├── BackgroundLayers
│   ├── Sky
│   ├── Mountains
│   └── Trees
├── GameWorld
│   ├── Ground
│   ├── Player
│   └── Enemies
│       ├── Enemy1
│       └── Enemy2
└── UI
    ├── HealthBar
    └── ScoreLabel
```

**How to organize:**

1. **Scene Graph → Right-click root → Add Node**
2. Name: "BackgroundLayers"
3. **Drag Sky, Mountains, Trees into BackgroundLayers**
4. Repeat for GameWorld and UI

**Benefits:**
- Easier to find nodes
- Can hide/show entire groups
- Can move entire groups together

---

## Step 9: Save the Scene

### Option 1: Code

```cpp
SceneLoader loader;
loader.saveScene(scene.get(), "assets/scenes/level1.json");
```

### Option 2: Editor

1. **File → Save Scene**
2. Name: "level1.json"
3. Location: "assets/scenes/"

**JSON format:**
```json
{
  "name": "Level1",
  "camera": {
    "zoom": 1.5,
    "limits": [0, 0, 1920, 1080]
  },
  "nodes": [
    {
      "type": "ParallaxLayer",
      "name": "Sky",
      "texture": "assets/sky.png",
      "parallax": [0.1, 0.1],
      "zIndex": -300
    },
    ...
  ]
}
```

---

## Step 10: Load and Run

```cpp
// Load the scene
SceneLoader loader;
auto scene = loader.loadScene("assets/scenes/level1.json");

// Add to world
World world;
world.addScene("level1", std::move(scene));
world.changeScene("level1");

// Game loop
while (running) {
    world.update(deltaTime);
    world.render(renderer);
}
```

---

## Advanced Techniques

### Technique 1: Dynamic Z-Index

**Problem:** Player should appear behind trees when walking behind them

**Solution:** Adjust z-index based on Y position

```cpp
void Player::update(float deltaTime) {
    // Higher Y = further down screen = in front
    setZIndex(static_cast<int>(getY()));
}
```

**Result:** Automatic depth sorting!

### Technique 2: Multiple Cameras

**Use case:** Split-screen multiplayer

```cpp
auto camera1 = std::make_unique<Camera2D>("Player1Camera");
camera1->setTarget(player1.get());
camera1->setViewport({0, 0, 640, 480});  // Left half

auto camera2 = std::make_unique<Camera2D>("Player2Camera");
camera2->setTarget(player2.get());
camera2->setViewport({640, 0, 640, 480});  // Right half
```

### Technique 3: Camera Shake

**Use case:** Explosion or impact effect

```cpp
// On explosion
camera->shake(0.5f, 20.0f);  // 0.5s duration, 20px intensity

// Camera shakes for 0.5 seconds then returns to normal
```

### Technique 4: Layer Opacity for Fog

**Use case:** Fade in/out layers for atmosphere

```cpp
// Morning fog (high opacity)
fogLayer->setOpacity(0.8f);

// Midday (no fog)
fogLayer->setOpacity(0.0f);

// Evening fog (medium opacity)
fogLayer->setOpacity(0.5f);
```

---

## Performance Tips

### Tip 1: Cull Off-Screen Nodes

```cpp
void Scene::render(Renderer* renderer, Camera2D* camera) {
    for (auto& node : m_children) {
        if (camera->isVisible(node->getBounds())) {
            node->render(renderer);
        }
    }
}
```

### Tip 2: Batch Similar Sprites

**Problem:** 100 grass tiles = 100 draw calls

**Solution:** Use TileMapLayer (batches automatically)

### Tip 3: Limit Parallax Layers

**Problem:** 10 parallax layers = slow

**Solution:** Use 3-4 layers maximum

### Tip 4: Use Sprite Atlases

**Problem:** Loading 50 separate textures is slow

**Solution:** Combine into one atlas texture

---

## Checklist

Before finalizing your scene:

- [ ] All nodes have meaningful names
- [ ] Z-index is correct (background < game < UI)
- [ ] Camera limits prevent showing outside world
- [ ] Parallax effect looks smooth
- [ ] TileMap collision is marked correctly
- [ ] UI is positioned correctly
- [ ] Scene is organized with containers
- [ ] Scene is saved to JSON
- [ ] Tested in-game (not just editor)

---

## Next Steps

1. **[TileMap Creation](TileMapCreation.md)** - Advanced tilemap techniques
2. **[Physics Basics](PhysicsBasics.md)** - Add collision and movement
3. **[Animation Guide](AnimationGuide.md)** - Advanced animation techniques

---

**Build amazing scenes!** 🎮
