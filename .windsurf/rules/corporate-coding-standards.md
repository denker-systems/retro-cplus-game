---
trigger: always_on
description: Corporate-Grade C++ Coding Standards for Game Engine Development (50+ team)
---

# Corporate C++ Coding Standards

> Inspirerat av Unreal Engine, Unity, och branschstandarder för 50+ utvecklarteam

## 1. FILSTRUKTUR

### 1.1 Fil-header (OBLIGATORISK)
```cpp
/**
 * @file FileName.cpp
 * @brief Kort beskrivning av filens syfte
 * 
 * @details
 * Detaljerad beskrivning av:
 * - Huvudsaklig funktionalitet
 * - Vilka klasser/system som implementeras
 * - Beroenden och relationer
 * 
 * @author [Utvecklarnamn eller Team]
 * @date YYYY-MM-DD
 * @version X.Y.Z
 * 
 * @copyright Copyright (c) 2024-2026 Retro Adventure Team
 * @license MIT License
 * 
 * @see RelatedClass.h
 * @see docs/architecture/module-name.md
 */
```

### 1.2 Include-ordning (Unreal-stil)
```cpp
// 1. Motsvarande header (för .cpp filer)
#include "ThisFile.h"

// 2. Engine headers (alfabetiskt inom grupp)
#include "engine/core/ActorObject.h"
#include "engine/components/SpriteComponent.h"

// 3. Third-party libraries
#include <SDL.h>
#include <nlohmann/json.hpp>

// 4. Standard library
#include <memory>
#include <string>
#include <vector>
```

## 2. NAMNGIVNING (Unreal Engine-stil)

| Typ | Konvention | Exempel |
|-----|------------|---------|
| Klasser | PascalCase | `PlayerCharacter` |
| Interfaces | I-prefix | `IRenderable`, `ISerializable` |
| Abstract classes | A-prefix (valfritt) | `ACharacter` |
| Komponenter | Component-suffix | `SpriteComponent` |
| Managers | Manager-suffix | `TextureManager` |
| Systems | System-suffix | `PhysicsSystem` |
| Metoder | camelCase | `getPosition()`, `setHealth()` |
| Medlemsvariabler | m_-prefix | `m_position`, `m_health` |
| Statiska variabler | s_-prefix | `s_instance` |
| Konstanter | SCREAMING_SNAKE | `MAX_HEALTH`, `TILE_SIZE` |
| Enums | PascalCase | `enum class BodyType` |
| Enum values | PascalCase | `BodyType::Dynamic` |
| Namespaces | lowercase | `engine`, `editor` |
| Template params | T-prefix | `typename TComponent` |

## 3. DOKUMENTATION (Doxygen)

### 3.1 Klass-dokumentation
```cpp
/**
 * @class PlayerCharacter
 * @brief Spelarens karaktär med input och rörelse
 * 
 * @details
 * PlayerCharacter är huvudkaraktären som spelaren styr.
 * Hanterar:
 * - Keyboard/gamepad input
 * - Point-and-click navigation
 * - Inventory management
 * - Dialog interaction
 * 
 * @note Endast EN PlayerCharacter ska existera per Scene
 * 
 * @par Exempel:
 * @code
 * auto player = std::make_unique<PlayerCharacter>();
 * player->setPosition(100.0f, 200.0f);
 * scene->addActor(std::move(player));
 * @endcode
 * 
 * @see CharacterActor
 * @see MovementComponent
 * @see docs/api/PlayerCharacter.md
 */
class PlayerCharacter : public CharacterActor {
```

### 3.2 Metod-dokumentation
```cpp
/**
 * @brief Flyttar karaktären mot en målposition
 * 
 * @details
 * Använder A* pathfinding för att hitta optimal väg.
 * Respekterar walk areas och undviker hinder.
 * 
 * @param[in] targetX Mål X-koordinat i world space
 * @param[in] targetY Mål Y-koordinat i world space
 * @param[in] speed Hastighet i pixels/sekund (default: 100.0f)
 * 
 * @return true om path hittades och rörelse påbörjades
 * @return false om målet är oåtkomligt
 * 
 * @throws std::invalid_argument om koordinater är utanför world bounds
 * 
 * @pre Scene måste ha en giltig WalkArea
 * @post Karaktären rör sig mot målet
 * 
 * @par Komplexitet:
 * O(n log n) där n är antal noder i pathfinding grid
 * 
 * @par Thread Safety:
 * Inte thread-safe. Anropa endast från main thread.
 * 
 * @see stopMovement()
 * @see isMoving()
 */
bool moveTo(float targetX, float targetY, float speed = 100.0f);
```

### 3.3 Inline-kommentarer (förklara VARFÖR)
```cpp
// Clamp position till walk area för att förhindra att spelaren
// går utanför tillåtna områden (t.ex. genom väggar)
m_position.x = std::clamp(m_position.x, m_walkArea.minX, m_walkArea.maxX);

// Använder squared distance för att undvika dyr sqrt()-operation
// i hot path (kallas 60+ ggr/sekund)
float distSq = (dx * dx) + (dy * dy);
if (distSq < INTERACTION_RADIUS_SQ) {
    // Nära nog för interaktion
}

// Box2D använder meter, vi använder pixels
// Konverteringsfaktor: 32 pixels = 1 meter (PIXELS_PER_METER)
b2Vec2 worldPos = toBox2D(m_position);
```

## 4. ARKITEKTUR

### 4.1 Hierarki (max 3-4 nivåer)
```
Object (abstract root)
 ActorObject (transform, lifecycle)
     ActorObjectExtended (components)
         CharacterActor
            PlayerActor
            NPCActor
         EnvironmentActor
         InteractiveActor
```

### 4.2 Component Pattern
```cpp
// Prefer composition over inheritance
class ActorObjectExtended : public ActorObject {
    template<typename T>
    T* addComponent();
    
    template<typename T>
    T* getComponent();
    
private:
    std::vector<std::unique_ptr<ActorComponent>> m_components;
};
```

## 5. MINNESHANTERING

### 5.1 Smart Pointers
```cpp
// Ownership: unique_ptr
std::unique_ptr<Scene> m_scene;

// Shared ownership (sällan): shared_ptr
std::shared_ptr<Texture> m_sharedTexture;

// Non-owning reference: raw pointer eller reference
Scene* getCurrentScene();  // Returns non-owning
const Texture& getTexture() const;  // Reference
```

### 5.2 SDL Resources (RAII)
```cpp
// Använd custom deleters för SDL-resurser
using SDLTexturePtr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
SDLTexturePtr texture(SDL_CreateTexture(...), SDL_DestroyTexture);
```

## 6. ERROR HANDLING

### 6.1 Logging Levels
```cpp
LOG_TRACE("Detailed debug info");     // Utveckling endast
LOG_DEBUG("Debug information");       // Debug builds
LOG_INFO("Normal operation");         // Alltid
LOG_WARNING("Potential problem");     // Alltid
LOG_ERROR("Error occurred");          // Alltid
LOG_FATAL("Critical failure");        // Crash imminent
```

### 6.2 Assertions
```cpp
// Debug-only assertions
ENGINE_ASSERT(ptr != nullptr, "Pointer must not be null");
ENGINE_ASSERT(index < size, "Index out of bounds: {} >= {}", index, size);

// Release assertions (kritiska)
ENGINE_VERIFY(file.is_open(), "Failed to open config file");
```

## 7. PERFORMANCE

### 7.1 Hot Path Optimization
```cpp
// Markera hot paths
// [HOT PATH] - Anropas varje frame
void update(float deltaTime) {
    // Undvik allokering
    // Använd cache-vänliga strukturer
    // Minimera branching
}
```

### 7.2 Cache-friendly Data
```cpp
// Prefer arrays of structs för cache locality
std::vector<Transform> m_transforms;  // Bra
// vs
std::vector<Actor*> m_actors;  // Pointer chasing = dåligt för cache
```
