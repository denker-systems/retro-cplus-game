---
description: Uppdatera kod-kommentarer enligt Doxygen-standard
auto_execution_mode: 1
---

# Update Commentary Workflow

> Kod-dokumentation för Retro Engine

## Principer

1. **Dokumentera VARFÖR, inte VAD**
2. **Håll kommentarer aktuella**
3. **Använd Doxygen-format**
4. **Engelska i kod, svenska i docs**

---

## 1. Fil-header (Obligatorisk)

### Engine/Game kod
```cpp
/**
 * @file PlayerActor.cpp
 * @brief Player character with input handling and movement
 * 
 * @details
 * Handles keyboard/mouse input and point-and-click
 * movement within walk areas.
 * 
 * @author Retro Team
 * @date YYYY-MM-DD
 * 
 * @see CharacterActor
 */
```

### Editor kod
```cpp
/**
 * @file ViewportPanel.cpp
 * @brief Scene viewport with visual editing
 * 
 * @details
 * ImGui panel for scene rendering and hotspot editing.
 * Supports zoom, pan, and drag-drop operations.
 */
```

---

## 2. Actor/Component Dokumentation

### Actor Klass
```cpp
/**
 * @class PlayerActor
 * @brief Player-controlled character
 * 
 * @details
 * Responsibilities:
 * - Input processing (keyboard/mouse)
 * - Point-and-click movement
 * - Walk area collision
 * - Inventory interaction
 * 
 * @par Components
 * - SpriteComponent for rendering
 * - AnimationComponent for animations
 * 
 * @par Usage
 * @code
 * auto player = std::make_unique<PlayerActor>("Player");
 * player->setPosition(100, 200);
 * scene->addActor(std::move(player));
 * @endcode
 * 
 * @see CharacterActor, SpriteComponent
 */
class PlayerActor : public CharacterActor {
```

### Component Klass
```cpp
/**
 * @class SpriteComponent
 * @brief Renders textured sprites with transform
 * 
 * @details
 * Features:
 * - Texture rendering via SDL2
 * - Flip (horizontal/vertical)
 * - Origin/pivot point
 * - Tint and opacity
 * 
 * @note Requires SDL_Renderer for rendering
 */
class SpriteComponent : public SceneComponent {
```

---

## 3. Metod-dokumentation

### Enkel metod
```cpp
/**
 * @brief Move actor toward target position
 * @param target Target position in world space
 * @return true if path found, false if blocked
 */
bool moveTo(Vec2 target);
```

### Komplex metod
```cpp
/**
 * @brief Load scene from JSON data
 * 
 * @param[in] json Scene data from scenes.json
 * @param[in] renderer SDL renderer for textures
 * @param[out] scene Populated scene object
 * 
 * @return true on success
 * @retval false if JSON invalid or textures missing
 * 
 * @throws std::runtime_error if critical data missing
 * 
 * @pre renderer must be valid
 * @post scene contains all actors from JSON
 */
bool loadScene(const nlohmann::json& json, 
               SDL_Renderer* renderer,
               Scene& scene);
```

---

## 4. Medlemsvariabler

```cpp
private:
    // Transform
    Vec2 m_position{0, 0};      ///< World position in pixels
    float m_rotation = 0.0f;    ///< Rotation in degrees
    Vec2 m_scale{1, 1};         ///< Scale factor
    
    // State
    bool m_active = true;       ///< Whether actor updates
    bool m_visible = true;      ///< Whether actor renders
    
    // Movement
    float m_moveSpeed = 100.0f; ///< Speed in pixels/second
    Vec2 m_targetPos;           ///< Current movement target
    
    // Components
    std::vector<std::unique_ptr<ActorComponent>> m_components;
```

---

## 5. Inline-kommentarer (Förklara VARFÖR)

### Bra exempel
```cpp
// Box2D uses meters, we use pixels (PIXELS_PER_METER = 32)
b2Vec2 worldPos = toBox2D(m_position);

// Clamp to walk area to prevent walking through walls
m_position.x = std::clamp(m_position.x, m_walkMinX, m_walkMaxX);

// Using squared distance avoids expensive sqrt() in hot path
float distSq = (dx * dx) + (dy * dy);
if (distSq < INTERACTION_RADIUS_SQ) {
    // Close enough for interaction
}

// Scene owns actors - transfer ownership explicitly
scene->addActor(std::move(actor));
```

### Undvik (självklara kommentarer)
```cpp
// ❌ Increment counter
i++;

// ❌ Create new actor
auto actor = std::make_unique<Actor>();

// ❌ Loop through actors
for (auto& actor : m_actors) {
```

---

## 6. TODO/FIXME Format

```cpp
// TODO(team): Add pathfinding - Issue #45
// FIXME(team): Memory leak in texture loading - Bug #67
// HACK: Workaround for SDL2 render order bug
// NOTE: Walk area coordinates are in world space
// [HOT PATH] - Called every frame, keep optimized
```

---

## 7. Enum Dokumentation

```cpp
/**
 * @enum HotspotType
 * @brief Types of interactive hotspots in scenes
 */
enum class HotspotType {
    Exit,       ///< Leads to another scene
    Item,       ///< Pickable item
    Examine,    ///< Examinable object
    NPC,        ///< Non-player character
    Trigger     ///< Scripted trigger area
};

/**
 * @enum BodyType
 * @brief Box2D physics body types
 */
enum class BodyType {
    Static,     ///< Never moves (walls, ground)
    Kinematic,  ///< Moves but not affected by forces
    Dynamic     ///< Full physics simulation
};
```

---

## 8. Sektion-kommentarer

```cpp
// ============================================================
// LIFECYCLE
// ============================================================

void onActorBegin() { ... }
void onActorEnd() { ... }

// ============================================================
// UPDATE
// ============================================================

void update(float deltaTime) { ... }

// ============================================================
// RENDERING
// ============================================================

void render(SDL_Renderer* renderer) { ... }
```

---

## 9. Verifiera

// turbo
```powershell
cd build; cmake --build . --config Release
```

---

## Checklista

### Obligatoriskt
- [ ] Fil-headers i alla nya filer
- [ ] Alla klasser dokumenterade
- [ ] Alla publika metoder dokumenterade
- [ ] Medlemsvariabler med `///<` kommentarer

### Kvalitet
- [ ] Kommentarer förklarar VARFÖR
- [ ] Inga självklara kommentarer
- [ ] TODOs har issue-referenser
- [ ] Hot paths markerade

### Verifiering
- [ ] Koden kompilerar utan fel
- [ ] Ingen duplicerad dokumentation
