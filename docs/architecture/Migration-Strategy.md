# Pragmatisk Migrations-Strategi
## Från Nuvarande System → UE5-liknande Hierarki

---

## 🎯 MÅL: Minimal Breaking Change

**Princip:** Behåll befintlig kod, lägg till nya koncept gradvis

**Strategi:**
1. **Alias och Wrappers** - Byt namn genom typedef/using
2. **Gradvis Refactoring** - En modul i taget
3. **Backward Compatibility** - Gamla klasser fungerar parallellt
4. **Incremental Migration** - Migrera när vi ändå rör koden

---

## 📊 NUVARANDE vs MÅLBILD

### Nuvarande System
```
Node (Bas)
├─ Node2D (Position)
│  ├─ Sprite
│  ├─ AnimatedSprite
│  ├─ InteractiveArea
│  └─ WalkArea

Entity (Separat hierarki!)
├─ Character
│  ├─ PlayerCharacter
│  └─ NPC

Scene (Container)
Level (Container)
World (Container)
```

### Målbild (UE5-stil)
```
Object (Bas)
├─ ActorObject
│  ├─ Pawn
│  │  └─ Character
│  ├─ Controller
│  ├─ VisualActor
│  └─ InteractiveActor
│
├─ ActorComponent
│  ├─ SceneComponent
│  └─ GameplayComponent
│
└─ WorldContext
   ├─ World
   ├─ Level
   └─ Scene
```

---

## 🔄 FAS 1: ALIAS OCH TYPEDEF (Noll Breaking Change)

### Steg 1.1: Skapa Object som Alias för Node

```cpp
// engine/core/Object.h (NY FIL)
#pragma once
#include "Node.h"

namespace engine {

// Object är bara ett alias för Node (för nu)
using Object = Node;

} // namespace engine
```

**Resultat:**
- ✅ Befintlig kod fungerar (Node finns kvar)
- ✅ Ny kod kan använda Object
- ✅ Noll breaking changes

### Steg 1.2: Skapa ActorObject som Alias för Node2D

```cpp
// engine/core/ActorObject.h (NY FIL)
#pragma once
#include "Node2D.h"

namespace engine {

// ActorObject är bara ett alias för Node2D (för nu)
using ActorObject = Node2D;

} // namespace engine
```

**Resultat:**
- ✅ Node2D fungerar som innan
- ✅ Ny kod kan använda ActorObject
- ✅ Noll breaking changes

### Steg 1.3: Skapa ActorComponent som Wrapper

```cpp
// engine/core/ActorComponent.h (NY FIL)
#pragma once
#include "Object.h"

namespace engine {

class ActorObject; // Forward declaration

// Ny bas-klass för components
class ActorComponent : public Object {
public:
    ActorComponent(const std::string& name) : Object(name) {}
    virtual ~ActorComponent() = default;
    
    void setOwner(ActorObject* owner) { m_owner = owner; }
    ActorObject* getOwner() const { return m_owner; }
    
    virtual void update(float deltaTime) {}
    virtual void render(SDL_Renderer* renderer) {}
    
protected:
    ActorObject* m_owner = nullptr;
};

// SceneComponent = Component med transform
class SceneComponent : public ActorComponent {
public:
    SceneComponent(const std::string& name) : ActorComponent(name) {}
    
    Vec2 getPosition() const { return m_position; }
    void setPosition(Vec2 pos) { m_position = pos; }
    
protected:
    Vec2 m_position{0, 0};
    float m_rotation = 0.0f;
    Vec2 m_scale{1.0f, 1.0f};
};

} // namespace engine
```

**Resultat:**
- ✅ Nya komponenter kan skapas
- ✅ Befintlig kod påverkas inte
- ✅ Gradvis migration möjlig

---

## 🔄 FAS 2: MIGRERA SPRITE → COMPONENT (Exempel)

### Nuvarande Sprite (Behåll som är)

```cpp
// engine/nodes/Sprite.h (BEFINTLIG - ÄNDRA INTE)
class Sprite : public Node2D {
    SDL_Texture* m_texture;
    // ... befintlig kod
};
```

### Ny SpriteComponent (Lägg till parallellt)

```cpp
// engine/components/SpriteComponent.h (NY FIL)
#pragma once
#include "engine/core/ActorComponent.h"

namespace engine {

class SpriteComponent : public SceneComponent {
public:
    SpriteComponent(const std::string& name = "SpriteComponent") 
        : SceneComponent(name) {}
    
    void setTexture(SDL_Texture* texture) { m_texture = texture; }
    SDL_Texture* getTexture() const { return m_texture; }
    
    void render(SDL_Renderer* renderer) override {
        if (!m_texture || !m_owner) return;
        
        // Använd owner's position
        Vec2 pos = m_owner->getPosition();
        SDL_Rect dest = {(int)pos.x, (int)pos.y, m_width, m_height};
        SDL_RenderCopy(renderer, m_texture, nullptr, &dest);
    }
    
private:
    SDL_Texture* m_texture = nullptr;
    int m_width = 32;
    int m_height = 32;
};

} // namespace engine
```

**Resultat:**
- ✅ Gamla Sprite fungerar som innan
- ✅ Nya SpriteComponent kan användas parallellt
- ✅ Gradvis migration när vi ändå rör koden

---

## 🔄 FAS 3: MIGRERA CHARACTER → PAWN (Gradvis)

### Steg 3.1: Skapa Pawn som Wrapper

```cpp
// engine/actors/Pawn.h (NY FIL)
#pragma once
#include "engine/core/ActorObject.h"

namespace engine {

class Controller; // Forward declaration

class Pawn : public ActorObject {
public:
    Pawn(const std::string& name) : ActorObject(name) {}
    
    void possess(Controller* controller);
    void unpossess();
    Controller* getController() const { return m_controller; }
    
    // Input från controller
    virtual void moveForward(float value) {}
    virtual void moveRight(float value) {}
    virtual void interact() {}
    
protected:
    Controller* m_controller = nullptr;
};

} // namespace engine
```

### Steg 3.2: Migrera Character att ärva från Pawn

```cpp
// engine/entities/Character.h (MODIFIERA GRADVIS)
#pragma once
#include "engine/actors/Pawn.h" // Ändra från Entity

namespace engine {

// INNAN: class Character : public Entity
// EFTER:  class Character : public Pawn
class Character : public Pawn {
public:
    Character(const std::string& name) : Pawn(name) {}
    
    // Behåll all befintlig funktionalitet
    void setAnimation(const std::string& anim) { /* ... */ }
    void playAnimation() { /* ... */ }
    
    // Lägg till Pawn-funktionalitet
    void moveForward(float value) override {
        // Implementera movement
    }
};

} // namespace engine
```

**Resultat:**
- ✅ Character får Pawn-funktionalitet
- ✅ Befintlig Character-kod fungerar
- ✅ Kan lägga till Controller senare

---

## 🔄 FAS 4: LÄGG TILL CONTROLLERS (Nytt Koncept)

### Skapa Controller-system (Lägg till, ändra inte befintligt)

```cpp
// engine/actors/Controller.h (NY FIL)
#pragma once
#include "engine/core/Object.h"

namespace engine {

class Pawn;

class Controller : public Object {
public:
    Controller(const std::string& name) : Object(name) {}
    
    void possess(Pawn* pawn);
    void unpossess();
    Pawn* getPawn() const { return m_possessedPawn; }
    
protected:
    Pawn* m_possessedPawn = nullptr;
};

class PlayerController : public Controller {
public:
    PlayerController() : Controller("PlayerController") {}
    
    void handleInput(const Input& input);
};

class AIController : public Controller {
public:
    AIController() : Controller("AIController") {}
    
    void update(float deltaTime);
    void setBehavior(AIBehaviorType type);
    
private:
    AIBehaviorType m_currentBehavior = AIBehaviorType::Idle;
};

} // namespace engine
```

**Användning (Ny kod):**
```cpp
// game/states/PlayState.cpp
auto player = std::make_unique<PlayerCharacter>();
auto controller = std::make_unique<PlayerController>();
controller->possess(player.get());

// I game loop
controller->handleInput(input);
```

**Resultat:**
- ✅ Nytt Controller-system
- ✅ Befintlig kod påverkas inte
- ✅ Kan användas gradvis

---

## 🔄 FAS 5: ORGANISERA MAPPAR (Gradvis Flyttning)

### Nuvarande Struktur (Behåll)
```
engine/
├─ core/Node.h, Node2D.h
├─ entities/Entity.h, Character.h, NPC.h
├─ nodes/Sprite.h, AnimatedSprite.h
└─ world/Scene.h, Level.h, World.h
```

### Målstruktur (Flytta gradvis)
```
engine/
├─ core/
│  ├─ Object.h (alias för Node)
│  ├─ ActorObject.h (alias för Node2D)
│  ├─ ActorComponent.h (ny)
│  └─ SceneComponent.h (ny)
│
├─ actors/ (NY MAPP - flytta gradvis)
│  ├─ Pawn.h (ny)
│  ├─ Character.h (flytta från entities/)
│  ├─ Controller.h (ny)
│  ├─ PlayerController.h (ny)
│  └─ AIController.h (ny)
│
├─ components/ (NY MAPP)
│  ├─ SpriteComponent.h (ny, parallell med nodes/Sprite.h)
│  ├─ AnimationComponent.h (ny)
│  └─ MovementComponent.h (ny)
│
├─ legacy/ (FLYTTA GAMLA HIT GRADVIS)
│  ├─ Node.h (behåll för backward compat)
│  ├─ Node2D.h (behåll för backward compat)
│  └─ Entity.h (deprecated)
│
└─ world/ (BEHÅLL)
   ├─ World.h
   ├─ Level.h
   └─ Scene.h
```

**Migration-strategi:**
1. Skapa nya mappar (actors/, components/)
2. Lägg till nya filer parallellt
3. Flytta gamla filer till legacy/ när de inte används
4. Uppdatera includes gradvis

---

## 📋 PRAKTISK MIGRATIONS-PLAN

### Vecka 1: Alias och Nya Bas-klasser
- [ ] Skapa `Object.h` (alias för Node)
- [ ] Skapa `ActorObject.h` (alias för Node2D)
- [ ] Skapa `ActorComponent.h` och `SceneComponent.h`
- [ ] Testa att allt kompilerar

### Vecka 2: Första Component
- [ ] Skapa `SpriteComponent.h`
- [ ] Testa SpriteComponent parallellt med Sprite
- [ ] Dokumentera skillnader

### Vecka 3: Pawn och Controller
- [ ] Skapa `Pawn.h`
- [ ] Skapa `Controller.h`, `PlayerController.h`, `AIController.h`
- [ ] Migrera `Character` att ärva från `Pawn`
- [ ] Testa Controller-system

### Vecka 4: Migrera Ett Rum
- [ ] Välj ett test-rum (t.ex. tavern)
- [ ] Konvertera till nya systemet
- [ ] Jämför med gamla systemet
- [ ] Dokumentera lärdomar

### Vecka 5+: Gradvis Migration
- [ ] Migrera rum för rum
- [ ] Migrera NPCs till nya systemet
- [ ] Migrera items till nya systemet
- [ ] Ta bort legacy-kod när den inte används

---

## ✅ FÖRDELAR MED DENNA STRATEGI

**1. Noll Breaking Changes Initialt**
- Befintlig kod fungerar
- Nya koncept läggs till parallellt
- Kan testa gradvis

**2. Backward Compatibility**
- Node och Node2D finns kvar
- Gamla klasser fungerar
- Migration i egen takt

**3. Testbar**
- Kan jämföra gammalt vs nytt
- Kan rulla tillbaka om problem
- Kan migrera modul för modul

**4. Lärande**
- Förstå nya systemet genom att använda det
- Hitta problem tidigt
- Anpassa efter behov

**5. Flexibel**
- Kan pausa migration när som helst
- Kan prioritera viktiga delar först
- Kan skippa delar som inte behövs

---

## 🎯 FÖRSTA STEG (Idag/Imorgon)

**1. Skapa Alias-filer (5 min)**
```bash
# Skapa nya filer
touch engine/core/Object.h
touch engine/core/ActorObject.h
touch engine/core/ActorComponent.h
```

**2. Implementera Alias (10 min)**
```cpp
// Kopiera kod från ovan
```

**3. Testa Kompilering (5 min)**
```bash
cmake --build . --config Release
```

**4. Skapa Första Component (30 min)**
```cpp
// Implementera SpriteComponent
```

**5. Testa Parallellt (30 min)**
```cpp
// Skapa test-scene med både Sprite och SpriteComponent
// Jämför resultat
```

**Total tid: ~1.5 timmar för första steget**

---

## 🤔 FRÅGOR TILL DIG

**1. Gillar du denna gradvisa approach?**
- Ja → Börja med Fas 1 (Alias)
- Nej → Vad vill du ändra?

**2. Vad vill du migrera först?**
- A) Sprite → SpriteComponent
- B) Character → Pawn + Controller
- C) Något annat?

**3. Hur snabbt vill du gå?**
- Snabbt → Migrera allt på en gång (riskabelt)
- Lagom → En modul per vecka
- Långsamt → När vi ändå rör koden

**4. Vill du behålla backward compatibility?**
- Ja → Behåll Node/Node2D/Entity
- Nej → Ta bort direkt (breaking changes)

