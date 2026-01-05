# SpatialGrid API Documentation

## Overview

`SpatialGrid` är en grid-baserad spatial partitioning struktur för effektiv broad-phase collision detection. Genom att dela upp världen i celler kan vi snabbt hitta potentiella kollisioner utan att behöva testa alla objekt mot varandra.

**Header:** `src/engine/physics/SpatialGrid.h`  
**Namespace:** `engine`

---

## Class Definition

```cpp
class SpatialGrid {
public:
    SpatialGrid(int cellSize = 64);
    ~SpatialGrid() = default;
    
    void clear();
    void insert(PhysicsBody* body);
    std::vector<PhysicsBody*> query(const PhysicsBody* body) const;
    std::vector<PhysicsBody*> queryRect(const SDL_Rect& rect) const;
    
private:
    int getCellKey(int x, int y) const;
    void getCellsForBody(const PhysicsBody* body, std::vector<int>& cells) const;
    
    int m_cellSize;
    std::unordered_map<int, std::vector<PhysicsBody*>> m_grid;
};
```

---

## Constructor

### `SpatialGrid(int cellSize = 64)`

Skapar ett nytt spatial grid med given cellstorlek.

**Parameters:**
- `cellSize` - Storleken på varje grid-cell i pixels (default: 64)

**Example:**
```cpp
SpatialGrid grid(64);  // 64x64 pixel cells
```

**Performance Note:**
- Mindre celler = fler celler men färre objekt per cell
- Större celler = färre celler men fler objekt per cell
- Optimal cellstorlek ≈ genomsnittlig objektstorlek

---

## Public Methods

### `void clear()`

Rensar alla bodies från gridet.

**Example:**
```cpp
grid.clear();  // Remove all bodies
```

**Use Case:**
Anropas varje frame innan nya bodies inserteras.

---

### `void insert(PhysicsBody* body)`

Inserterar en body i gridet baserat på dess bounding box.

**Parameters:**
- `body` - Pointer till PhysicsBody att insertera

**Example:**
```cpp
grid.insert(player.get());
grid.insert(enemy.get());
```

**Implementation Details:**
- Beräknar vilka celler bodyn överlappar
- Inserterar bodyn i alla överlappande celler
- En body kan finnas i flera celler om den är stor eller sträcker sig över cellgränser

---

### `std::vector<PhysicsBody*> query(const PhysicsBody* body) const`

Returnerar alla bodies som kan kollidera med given body (broad-phase).

**Parameters:**
- `body` - Body att söka kollisioner för

**Returns:**
Vector med potentiella kollisionskandidater (utan duplicates)

**Example:**
```cpp
auto candidates = grid.query(player.get());
for (PhysicsBody* other : candidates) {
    if (player->collidesWith(other)) {
        // Narrow-phase collision detected
    }
}
```

**Performance:**
- O(k) där k = antal bodies i samma celler
- Mycket snabbare än O(n) för alla bodies

---

### `std::vector<PhysicsBody*> queryRect(const SDL_Rect& rect) const`

Returnerar alla bodies inom ett rektangulärt område.

**Parameters:**
- `rect` - SDL_Rect som definierar sökområdet

**Returns:**
Vector med alla bodies i området (utan duplicates)

**Example:**
```cpp
SDL_Rect explosionArea = {100, 100, 200, 200};
auto affectedBodies = grid.queryRect(explosionArea);
for (PhysicsBody* body : affectedBodies) {
    body->applyForce(explosionForce);
}
```

**Use Cases:**
- Area-of-effect attacks
- Trigger zones
- Spatial queries för AI

---

## Usage Pattern

### Basic Collision Detection

```cpp
SpatialGrid grid(64);

// Every frame:
grid.clear();

// Insert all bodies
for (auto& body : allBodies) {
    grid.insert(body.get());
}

// Check collisions
for (auto& body : allBodies) {
    auto candidates = grid.query(body.get());
    for (PhysicsBody* other : candidates) {
        if (body.get() != other && body->collidesWith(other)) {
            handleCollision(body.get(), other);
        }
    }
}
```

### With Physics Update

```cpp
class PhysicsWorld {
    SpatialGrid m_grid;
    std::vector<std::unique_ptr<PhysicsBody>> m_bodies;
    
public:
    void update(float deltaTime) {
        // Clear grid
        m_grid.clear();
        
        // Update physics
        for (auto& body : m_bodies) {
            body->applyPhysics(deltaTime);
        }
        
        // Rebuild grid
        for (auto& body : m_bodies) {
            m_grid.insert(body.get());
        }
        
        // Collision detection
        for (auto& body : m_bodies) {
            auto candidates = m_grid.query(body.get());
            for (PhysicsBody* other : candidates) {
                if (body.get() != other && body->collidesWith(other)) {
                    resolveCollision(body.get(), other);
                }
            }
        }
    }
};
```

---

## Performance Analysis

### Without Spatial Partitioning

```cpp
// O(n²) - Check all pairs
for (int i = 0; i < bodies.size(); i++) {
    for (int j = i + 1; j < bodies.size(); j++) {
        if (bodies[i]->collidesWith(bodies[j])) {
            // Handle collision
        }
    }
}
```

**Complexity:** O(n²)  
**Example:** 100 bodies = 4,950 checks

### With Spatial Partitioning

```cpp
// O(n * k) where k << n
for (auto& body : bodies) {
    auto candidates = grid.query(body.get());  // k candidates
    for (auto* other : candidates) {
        if (body->collidesWith(other)) {
            // Handle collision
        }
    }
}
```

**Complexity:** O(n × k) där k << n  
**Example:** 100 bodies, k=5 average = 500 checks (10x speedup!)

---

## Implementation Details

### Cell Key Calculation

```cpp
int SpatialGrid::getCellKey(int x, int y) const {
    // Hash grid coordinates to single integer
    return (x << 16) | (y & 0xFFFF);
}
```

**Why this works:**
- Combines x and y into unique 32-bit integer
- Fast bitwise operations
- No hash collisions för reasonable coordinates

### Cell Assignment

```cpp
void SpatialGrid::getCellsForBody(const PhysicsBody* body, 
                                   std::vector<int>& cells) const {
    SDL_Rect bounds = body->getShapes()[0]->getBounds();
    
    int minX = bounds.x / m_cellSize;
    int minY = bounds.y / m_cellSize;
    int maxX = (bounds.x + bounds.w) / m_cellSize;
    int maxY = (bounds.y + bounds.h) / m_cellSize;
    
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            cells.push_back(getCellKey(x, y));
        }
    }
}
```

**Example:**
- Body at (50, 50) size (100, 100)
- Cell size 64
- Overlaps cells: (0,0), (1,0), (0,1), (1,1)

---

## Best Practices

### 1. Choose Appropriate Cell Size

```cpp
// Too small - many cells, overhead
SpatialGrid grid(8);  // ❌ For 32x32 objects

// Too large - defeats purpose
SpatialGrid grid(1024);  // ❌ For 32x32 objects

// Just right - ~object size
SpatialGrid grid(64);  // ✅ For 32x48 objects
```

### 2. Clear Every Frame

```cpp
// ✅ Correct
void update() {
    grid.clear();
    for (auto& body : bodies) {
        grid.insert(body.get());
    }
}

// ❌ Incorrect - stale data
void update() {
    // Forgot to clear - bodies accumulate!
    for (auto& body : bodies) {
        grid.insert(body.get());
    }
}
```

### 3. Avoid Duplicate Checks

```cpp
// ✅ Correct - check each pair once
std::set<std::pair<PhysicsBody*, PhysicsBody*>> checkedPairs;

for (auto& body : bodies) {
    auto candidates = grid.query(body.get());
    for (auto* other : candidates) {
        auto pair = std::minmax(body.get(), other);
        if (checkedPairs.insert(pair).second) {
            // First time checking this pair
            if (body->collidesWith(other)) {
                handleCollision(body.get(), other);
            }
        }
    }
}
```

### 4. Use for Broad Phase Only

```cpp
// ✅ Correct - two-phase detection
auto candidates = grid.query(player.get());  // Broad phase
for (auto* other : candidates) {
    if (player->collidesWith(other)) {  // Narrow phase
        handleCollision(player.get(), other);
    }
}

// ❌ Incorrect - assuming grid query = collision
auto candidates = grid.query(player.get());
for (auto* other : candidates) {
    handleCollision(player.get(), other);  // Wrong! Not all candidates collide
}
```

---

## Limitations

1. **Static Grid Size:** Cell size är konstant, kan inte anpassas runtime
2. **Uniform Distribution:** Fungerar bäst när objekt är jämnt fördelade
3. **Large Objects:** Mycket stora objekt kan spänna över många celler
4. **Memory:** Använder mer minne än naive approach (trade-off för speed)

---

## Future Improvements

- [ ] Dynamic cell size baserat på object density
- [ ] Hierarchical grid (quadtree) för bättre skalning
- [ ] Separate grids för static vs dynamic objects
- [ ] Multi-threaded queries för stora världar

---

## See Also

- [PhysicsBody](PhysicsBody.md) - Physics body class
- [CollisionShape](CollisionShape.md) - Collision shapes
- [KinematicBody](KinematicBody.md) - Kinematic movement

---

**Created:** 2026-01-05  
**Last Updated:** 2026-01-05
