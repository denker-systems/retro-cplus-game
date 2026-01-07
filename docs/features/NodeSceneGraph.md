# Node Scene Graph System

**Status:** ✅ Komplett (Fas 4A-4D)  
**Version:** 1.0  
**Datum:** 2026-01-05

---

## Översikt

Node Scene Graph är ett komplett OOP-baserat scene management system inspirerat av Godot, Cocos2d-x och SFML. Systemet tillhandahåller hierarkisk node-struktur, transform inheritance, physics, och visuella editor-verktyg.

---

## Arkitektur

### Core Hierarchy

```
Node (base class)
├── Node2D (2D transform)
│   ├── Sprite
│   ├── AnimatedSprite
│   ├── TileMapLayer
│   ├── ParallaxLayer
│   ├── Label
│   ├── Camera2D
│   └── PhysicsBody
│       └── KinematicBody
└── Scene (container)
```

### Modulstruktur

```
src/engine/
├── core/           # Vec2, Node, Node2D
├── nodes/          # Visual nodes (Sprite, AnimatedSprite, etc.)
├── world/          # Camera2D, Scene, World, LayerManager
└── physics/        # CollisionShape, PhysicsBody, SpatialGrid

src/editor/panels/
├── SceneGraphPanel      # Hierarkisk node tree viewer
├── LayerEditorPanel     # Layer management
└── TileMapEditorPanel   # TileMap painting
```

---

## Core Components

### 1. Node System

**Node** är basklassen för alla objekt i scene graph.

#### Features:
- **Hierarki:** Parent/child-relationer med unique_ptr ownership
- **Z-Index:** Rendering order control
- **Lifecycle:** onEnter(), onExit(), update(), render()
- **Active/Visible:** Toggles för aktivering och synlighet
- **Name:** String identifier för debugging

#### Användning:
```cpp
auto root = std::make_unique<Node>("Root");
auto child = std::make_unique<Node>("Child");
root->addChild(std::move(child));

// Hierarki traversering
for (size_t i = 0; i < root->getChildCount(); i++) {
    Node* child = root->getChildAt(i);
    // Process child
}
```

---

### 2. Node2D - 2D Transform

**Node2D** extends Node med 2D transform (position, rotation, scale).

#### Features:
- **Position:** Vec2 world position
- **Rotation:** Rotation i radianer
- **Scale:** Vec2 scale factor
- **Transform Inheritance:** Children ärver parent transform
- **Local/Global Space:** Konvertering mellan local och global koordinater

#### Användning:
```cpp
auto sprite = std::make_unique<Sprite>("Player");
sprite->setPosition(100, 200);
sprite->setRotation(0.5f);  // radianer
sprite->setScale(2.0f, 2.0f);

// Transform inheritance
auto weapon = std::make_unique<Sprite>("Weapon");
sprite->addChild(std::move(weapon));  // Weapon följer sprite
```

---

### 3. Visual Nodes

#### Sprite
Enkel textur-rendering med tint, flip, source rect.

```cpp
auto sprite = std::make_unique<Sprite>("Background");
sprite->setTexture(texture);
sprite->setSourceRect({0, 0, 64, 64});
sprite->setTint({255, 255, 255, 200});  // Semi-transparent
sprite->setFlipX(true);
```

#### AnimatedSprite
Animation state machine med frame control.

```cpp
auto player = std::make_unique<AnimatedSprite>("Player");
player->addAnimation("idle", {0, 1, 2, 3}, 0.1f);
player->addAnimation("walk", {4, 5, 6, 7}, 0.08f);
player->play("idle");

// Callbacks
player->onAnimationFinished = [](const std::string& name) {
    std::cout << "Animation " << name << " finished\n";
};
```

#### TileMapLayer
Grid-baserad tilemap med collision detection.

```cpp
auto tilemap = std::make_unique<TileMapLayer>("Ground", 20, 15, 32);
tilemap->setTile(5, 10, 1);  // Set tile at grid position
tilemap->setCollision(5, 10, true);  // Mark as solid

// Check collision
if (tilemap->isCollisionAt(worldX, worldY)) {
    // Handle collision
}
```

#### ParallaxLayer
Parallax scrolling för depth effect.

```cpp
auto background = std::make_unique<ParallaxLayer>("Sky");
background->setParallaxFactor(0.5f, 0.3f);  // Slower than camera
background->setRepeat(true, false);  // Repeat horizontally
```

#### Label
Text rendering med alignment och color.

```cpp
auto label = std::make_unique<Label>("Title");
label->setText("Hello World");
label->setFontSize(24);
label->setColor({255, 255, 0, 255});  // Yellow
label->setAlignment(TextAlignment::Center);
```

---

### 4. World Management

#### Camera2D
Viewport med follow, zoom, limits, screen shake.

```cpp
auto camera = std::make_unique<Camera2D>("MainCamera");
camera->setTarget(player.get());  // Follow player
camera->setZoom(2.0f);  // 2x zoom
camera->setLimits({0, 0, 1920, 1080});  // Constrain to level bounds

// Screen shake
camera->shake(0.5f, 10.0f);  // 0.5s duration, 10px intensity

// Coordinate conversion
Vec2 worldPos = camera->screenToWorld(mouseX, mouseY);
Vec2 screenPos = camera->worldToScreen(entityX, entityY);
```

#### Scene
Container för game levels med camera management.

```cpp
auto scene = std::make_unique<Scene>("Level1");
scene->setCamera(std::move(camera));

// Add nodes
scene->addChild(std::move(background));
scene->addChild(std::move(player));

// Update and render
scene->update(deltaTime);
scene->render(renderer);
```

#### World
Multi-scene manager med transitions.

```cpp
World world;
world.addScene("menu", std::move(menuScene));
world.addScene("level1", std::move(level1Scene));

// Scene transition
world.changeScene("level1", 1.0f);  // 1 second fade

// Scene stack (for pause menus)
world.pushScene("pause");  // Push on top
world.popScene();  // Return to previous
```

#### LayerManager
Layer system för rendering order.

```cpp
LayerManager layerMgr;
layerMgr.addLayer("Background", LayerType::Background, -100);
layerMgr.addLayer("Main", LayerType::Main, 0);
layerMgr.addLayer("Foreground", LayerType::Foreground, 100);
layerMgr.addLayer("UI", LayerType::UI, 1000);

// Add nodes to layers
layerMgr.addNodeToLayer("Background", std::move(skySprite));
layerMgr.addNodeToLayer("Main", std::move(player));

// Render all layers in order
layerMgr.render(renderer, camera);
```

#### SceneLoader
JSON scene serialization.

```cpp
SceneLoader loader;

// Save scene
loader.saveScene(scene.get(), "assets/scenes/level1.json");

// Load scene
auto loadedScene = loader.loadScene("assets/scenes/level1.json");
```

**JSON Format:**
```json
{
  "name": "Level1",
  "camera": {
    "zoom": 1.0,
    "limits": [0, 0, 1920, 1080]
  },
  "nodes": [
    {
      "type": "Sprite",
      "name": "Background",
      "position": [0, 0],
      "texture": "assets/bg.png"
    }
  ]
}
```

---

### 5. Physics System

#### CollisionShape
AABB, Circle, Polygon collision shapes.

```cpp
// AABB (Axis-Aligned Bounding Box)
auto aabb = std::make_unique<AABBShape>(0, 0, 32, 48);

// Circle
auto circle = std::make_unique<CircleShape>(Vec2(16, 24), 16.0f);

// Polygon
std::vector<Vec2> points = {{0,0}, {32,0}, {16,32}};
auto polygon = std::make_unique<PolygonShape>(points);

// Collision detection
if (aabb->overlaps(*circle)) {
    // Collision detected
}
```

#### PhysicsBody
Physics med velocity, acceleration, friction.

```cpp
auto body = std::make_unique<PhysicsBody>("Enemy");
body->addShape(std::make_unique<AABBShape>(0, 0, 32, 32));
body->setVelocity(100, 0);  // Move right
body->setAcceleration(0, 980);  // Gravity
body->setFriction(0.9f);
body->setMass(1.0f);

// Collision callbacks
body->onCollisionEnter = [](PhysicsBody* other) {
    std::cout << "Collision with " << other->getName() << "\n";
};
```

#### KinematicBody
Kontrollerad rörelse med moveAndSlide().

```cpp
auto player = std::make_unique<KinematicBody>("Player");
player->addShape(std::make_unique<AABBShape>(0, 0, 16, 24));

// Move with collision response
Vec2 velocity(100, 0);  // Move right
player->moveAndSlide(velocity, obstacles);

// Floor detection
if (player->isOnFloor()) {
    // Can jump
    velocity.y = -500;
}
```

#### SpatialGrid
Spatial partitioning för effektiv collision detection.

```cpp
SpatialGrid grid(64);  // 64x64 cell size

// Insert bodies
grid.insert(player.get());
grid.insert(enemy.get());

// Query potential collisions (broad phase)
auto candidates = grid.query(player.get());
for (PhysicsBody* other : candidates) {
    if (player->collidesWith(other)) {
        // Narrow phase collision
    }
}

// Query rectangular area
SDL_Rect area = {100, 100, 200, 200};
auto bodiesInArea = grid.queryRect(area);
```

---

## Editor Integration

### SceneGraphPanel

Hierarkisk node tree viewer med drag-and-drop.

#### Features:
- **Tree View:** Expanderbar hierarki av alla nodes
- **Selection:** Click för att välja node
- **Drag-and-Drop Re-parenting:** Dra nodes för att ändra parent
- **Context Menu:** Rename, Duplicate, Delete, Active/Visible toggles
- **Filter:** Show/Hide inactive nodes

#### Användning:
```cpp
auto sceneGraph = std::make_unique<SceneGraphPanel>(context);
sceneGraph->setScene(currentScene);
sceneGraph->render();  // In ImGui context
```

#### Drag-and-Drop Implementation:
```cpp
// Source
if (ImGui::BeginDragDropSource()) {
    ImGui::SetDragDropPayload("SCENE_NODE", &node, sizeof(Node*));
    ImGui::EndDragDropSource();
}

// Target (re-parenting)
if (ImGui::BeginDragDropTarget()) {
    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_NODE")) {
        Node* draggedNode = *(Node**)payload->Data;
        // Transfer ownership
        auto ownership = draggedNode->getParent()->removeChild(draggedNode);
        newParent->addChild(std::move(ownership));
    }
    ImGui::EndDragDropTarget();
}
```

---

### LayerEditorPanel

Layer management med parallax preview.

#### Features:
- **Layer List:** Visar alla layers med type och z-index
- **Properties:** Edit type, z-index, opacity, active, visible
- **Add/Remove:** Dialog för att skapa nya layers
- **Parallax Preview:** Live scrolling preview för background layers

#### Parallax Preview:
```cpp
// Scroll slider
ImGui::SliderFloat("Scroll Position", &m_previewScroll, 0.0f, 1000.0f);

// Visual representation
for (const auto& layer : layers) {
    float parallaxFactor = 0.5f;  // Layer-specific
    float offset = m_previewScroll * parallaxFactor;
    float x = canvasPos.x + std::fmod(offset, canvasSize.x);
    
    // Draw layer representation
    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x+w, y+h), color);
}
```

---

### TileMapEditorPanel

Interactive tilemap painting med auto-tiling.

#### Features:
- **Tile Palette:** 4x4 grid för tile selection
- **Paint/Erase Modes:** Toggle mellan painting och erasing
- **Interactive Grid:** Click-and-drag för att måla tiles
- **Auto-tiling:** Neighbor-based tile variant selection
- **Collision Mode:** Mark tiles som solid

#### Interactive Painting:
```cpp
// Draw grid
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        int tile = tilemap->getTile(x, y);
        ImU32 color = tile > 0 ? TILE_COLOR : EMPTY_COLOR;
        drawList->AddRectFilled(p1, p2, color);
    }
}

// Handle mouse input
if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0)) {
    int gridX = (mousePos.x - canvasPos.x) / cellSize;
    int gridY = (mousePos.y - canvasPos.y) / cellSize;
    
    if (paintMode) {
        tilemap->setTile(gridX, gridY, selectedTile);
        autoTileNeighbors(gridX, gridY);
    } else {
        tilemap->setTile(gridX, gridY, 0);
    }
}
```

#### Auto-tiling Algorithm:
```cpp
void autoTileNeighbors(int x, int y) {
    int neighbors = 0;
    
    // Check 4 cardinal directions
    if (x > 0 && getTile(x-1, y) > 0) neighbors |= 1;  // Left
    if (x < width-1 && getTile(x+1, y) > 0) neighbors |= 2;  // Right
    if (y > 0 && getTile(x, y-1) > 0) neighbors |= 4;  // Up
    if (y < height-1 && getTile(x, y+1) > 0) neighbors |= 8;  // Down
    
    // Adjust tile variant based on neighbor configuration
    // Example: corners, edges, fills
    int variant = calculateVariant(neighbors);
    setTile(x, y, baseType + variant);
}
```

---

## Best Practices

### 1. Node Ownership

Använd alltid unique_ptr för ownership:

```cpp
// ✅ Correct
auto node = std::make_unique<Node>("MyNode");
parent->addChild(std::move(node));

// ❌ Incorrect
Node* node = new Node("MyNode");
parent->addChild(node);  // Compile error
```

### 2. Transform Hierarchy

Tänk på transform inheritance:

```cpp
// Parent at (100, 100)
auto parent = std::make_unique<Node2D>("Parent");
parent->setPosition(100, 100);

// Child at local (10, 10) = world (110, 110)
auto child = std::make_unique<Node2D>("Child");
child->setPosition(10, 10);
parent->addChild(std::move(child));
```

### 3. Z-Index Rendering

Använd z-index för rendering order:

```cpp
background->setZIndex(-100);  // Render first
player->setZIndex(0);         // Middle
ui->setZIndex(1000);          // Render last (on top)
```

### 4. Lifecycle Callbacks

Använd callbacks för initialization och cleanup:

```cpp
class MyNode : public Node {
    void onEnter() override {
        // Initialize resources
    }
    
    void onExit() override {
        // Cleanup resources
    }
};
```

### 5. Spatial Partitioning

Använd SpatialGrid för många objekt:

```cpp
// Without spatial partitioning: O(n²)
for (auto& a : bodies) {
    for (auto& b : bodies) {
        if (a->collidesWith(b)) { /* ... */ }
    }
}

// With spatial partitioning: O(n)
SpatialGrid grid(64);
for (auto& body : bodies) {
    grid.insert(body.get());
}
for (auto& body : bodies) {
    auto candidates = grid.query(body.get());
    for (auto* other : candidates) {
        if (body->collidesWith(other)) { /* ... */ }
    }
}
```

---

## Performance Tips

1. **Batch Rendering:** Group nodes by texture för fewer draw calls
2. **Culling:** Skip rendering för nodes utanför camera viewport
3. **Spatial Partitioning:** Använd SpatialGrid för collision detection
4. **Object Pooling:** Återanvänd nodes istället för att skapa nya
5. **Update Optimization:** Skip inactive nodes i update loop

---

## Exempel: Komplett Scene Setup

```cpp
// Create scene
auto scene = std::make_unique<Scene>("Level1");

// Setup camera
auto camera = std::make_unique<Camera2D>("MainCamera");
camera->setZoom(1.0f);
camera->setLimits({0, 0, 1920, 1080});
scene->setCamera(std::move(camera));

// Background layer
auto background = std::make_unique<ParallaxLayer>("Sky");
background->setTexture(skyTexture);
background->setParallaxFactor(0.3f, 0.3f);
background->setZIndex(-100);
scene->addChild(std::move(background));

// Tilemap
auto tilemap = std::make_unique<TileMapLayer>("Ground", 60, 40, 32);
tilemap->setTexture(tilesetTexture);
// Load tilemap data...
tilemap->setZIndex(-50);
scene->addChild(std::move(tilemap));

// Player
auto player = std::make_unique<KinematicBody>("Player");
player->addShape(std::make_unique<AABBShape>(0, 0, 16, 24));
player->setPosition(100, 300);
player->setZIndex(0);
scene->addChild(std::move(player));

// UI
auto healthBar = std::make_unique<Sprite>("HealthBar");
healthBar->setTexture(uiTexture);
healthBar->setZIndex(1000);
scene->addChild(std::move(healthBar));

// Add to world
world.addScene("level1", std::move(scene));
world.changeScene("level1");
```

---

## Troubleshooting

### Problem: Nodes inte synliga
**Lösning:** Kontrollera z-index och camera limits

### Problem: Transform fungerar inte
**Lösning:** Säkerställ att parent har Node2D, inte bara Node

### Problem: Collision detection långsam
**Lösning:** Använd SpatialGrid för broad-phase

### Problem: Memory leaks
**Lösning:** Använd unique_ptr, aldrig raw pointers för ownership

---

## Nästa Steg

- [ ] Implementera advanced auto-tiling med 47-tile ruleset
- [ ] Lägg till node templates för common patterns
- [ ] Implementera prefab system
- [ ] Lägg till visual scripting för node behavior
- [ ] Optimera rendering med batching

---

**Dokumentation skapad:** 2026-01-05  
**Senast uppdaterad:** 2026-01-05  
**Version:** 1.0
