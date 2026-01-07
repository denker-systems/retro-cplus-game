# Säker Migrations-Strategi (Research-baserad)
## Från Legacy System → UE5-liknande Hierarki

**Baserad på:**
- Strangler Fig Pattern (Martin Fowler)
- Unreal Engine Core Redirects
- C++ Legacy Code Best Practices

---

## 🎯 FORSKNINGS-BASERADE PRINCIPER

### 1. Strangler Fig Pattern (The Engineering Compass)

**4-stegs process:**
```
IDENTIFY → TRANSFORM → CO-EXIST → ELIMINATE
```

**Fördelar:**
- ✅ Inkrementell migration reducerar risk
- ✅ Kan pausas eller stoppas när som helst
- ✅ Varje steg är reversibelt
- ✅ Levererar värde kontinuerligt

**Varningar:**
- ⚠️ Kräver commitment - halvfärdig migration är värre än ingen
- ⚠️ Parallell körning kräver extra effort
- ⚠️ Integration mellan gammalt och nytt kräver arbete

### 2. Unreal Engine Core Redirects

**Koncept:** Automatisk ommappning av gamla namn → nya namn

```ini
[CoreRedirects]
+ClassRedirects=(OldName="Node",NewName="Object")
+PropertyRedirects=(OldName="Character.m_health",NewName="Character.m_healthPoints")
```

**Fördelar:**
- ✅ Bevara asset data vid refactoring
- ✅ Automatisk migration vid laddning
- ✅ Inget manuellt arbete per asset

**Applicerat på vårt system:**
- Vi kan använda **typedef/using** som "C++ Core Redirects"
- Behåll gamla namn, lägg till nya parallellt
- Gradvis fasning ut av gamla namn

### 3. Extract Method Refactoring (UE5 Best Practices)

**Princip:** Små inkrementella ändringar

**INNAN:**
```cpp
void BigComplexFunction() {
    // 100 lines of code
}
```

**EFTER:**
```cpp
void BigComplexFunction() {
    Step1();
    Step2();
    Step3();
}

void Step1() { /* focused logic */ }
void Step2() { /* focused logic */ }
void Step3() { /* focused logic */ }
```

**Applicerat:** Migrera en klass i taget, inte hela systemet

---

## 🔄 VÅR MIGRATIONS-STRATEGI (Förbättrad)

### Fas 1: IDENTIFY - Kartlägg Systemet

**1.1 Identifiera Moduler för Migration**

```
PRIORITY 1 (Kritisk för nya systemet):
├─ Node → Object (Bas för allt)
├─ Node2D → ActorObject (Objekt i världen)
└─ Component System (Nytt koncept)

PRIORITY 2 (Viktigt men inte kritiskt):
├─ Entity → Pawn (Kontrollerbar)
├─ Character → Character (Men ärver från Pawn)
└─ Controller System (Nytt koncept)

PRIORITY 3 (Kan vänta):
├─ Sprite → VisualActor/SpriteComponent
├─ AnimatedSprite → AnimatedSpriteComponent
└─ InteractiveArea → InteractiveActor
```

**1.2 Dependency Graph**

```
ActorObject depends on → Object
Pawn depends on → ActorObject
Character depends on → Pawn
Controller depends on → Object
SpriteComponent depends on → ActorComponent
```

**Migrationsordning:** Bottom-up (Object först, sen ActorObject, sen Pawn, etc.)

---

### Fas 2: TRANSFORM - Implementera Nytt System

**2.1 Skapa C++ "Core Redirects" (Alias)**

```cpp
// engine/core/CoreRedirects.h (NY FIL)
#pragma once

// Legacy includes
#include "Node.h"
#include "Node2D.h"

namespace engine {

// ============================================================================
// CORE REDIRECTS (Backward Compatibility)
// ============================================================================

// Object = Node (alias)
using Object = Node;

// ActorObject = Node2D (alias)  
using ActorObject = Node2D;

// Legacy typedefs (för kod som använder dessa)
using LegacyNode = Node;
using LegacyNode2D = Node2D;

} // namespace engine
```

**Varför detta fungerar:**
- ✅ Befintlig kod kompilerar utan ändringar
- ✅ Ny kod kan använda Object/ActorObject
- ✅ Noll breaking changes
- ✅ Kan fasas ut senare

**2.2 Skapa Nya Bas-klasser (Parallellt)**

```cpp
// engine/core/ActorComponent.h (NY FIL)
#pragma once
#include "CoreRedirects.h"

namespace engine {

class ActorObject;

/**
 * @brief Base class for all components (UE5-style)
 * 
 * Components are reusable pieces of functionality that can be
 * attached to ActorObjects. This is the new component system.
 */
class ActorComponent : public Object {
public:
    ActorComponent(const std::string& name) : Object(name) {}
    virtual ~ActorComponent() = default;
    
    // Owner management
    void setOwner(ActorObject* owner) { m_owner = owner; }
    ActorObject* getOwner() const { return m_owner; }
    
    // Lifecycle
    virtual void initialize() {}
    virtual void update(float deltaTime) {}
    virtual void render(SDL_Renderer* renderer) {}
    virtual void shutdown() {}
    
    // Enable/disable
    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    
protected:
    ActorObject* m_owner = nullptr;
    bool m_enabled = true;
};

/**
 * @brief Component with transform (UE5 USceneComponent equivalent)
 */
class SceneComponent : public ActorComponent {
public:
    SceneComponent(const std::string& name) : ActorComponent(name) {}
    
    // Transform
    Vec2 getPosition() const { return m_position; }
    void setPosition(Vec2 pos) { m_position = pos; }
    
    float getRotation() const { return m_rotation; }
    void setRotation(float rot) { m_rotation = rot; }
    
    Vec2 getScale() const { return m_scale; }
    void setScale(Vec2 scale) { m_scale = scale; }
    
    // Attachment (future)
    void attachTo(SceneComponent* parent) { m_parent = parent; }
    SceneComponent* getParent() const { return m_parent; }
    
protected:
    Vec2 m_position{0, 0};
    float m_rotation = 0.0f;
    Vec2 m_scale{1.0f, 1.0f};
    SceneComponent* m_parent = nullptr;
};

} // namespace engine
```

**2.3 Utöka ActorObject med Component Support**

```cpp
// engine/core/ActorObject.h (NY FIL - eller edit Node2D.h)
#pragma once
#include "CoreRedirects.h"
#include "ActorComponent.h"
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace engine {

/**
 * @brief Extended ActorObject with component support
 * 
 * This is a thin wrapper/extension of Node2D that adds
 * component functionality without breaking existing code.
 */
class ActorObjectExtended : public ActorObject {
public:
    ActorObjectExtended(const std::string& name) : ActorObject(name) {}
    virtual ~ActorObjectExtended() = default;
    
    // Component management
    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        auto component = std::make_unique<T>(std::forward<Args>(args)...);
        component->setOwner(this);
        component->initialize();
        
        T* ptr = component.get();
        m_components.push_back(std::move(component));
        m_componentMap[std::type_index(typeid(T))] = ptr;
        
        return ptr;
    }
    
    template<typename T>
    T* getComponent() {
        auto it = m_componentMap.find(std::type_index(typeid(T)));
        if (it != m_componentMap.end()) {
            return static_cast<T*>(it->second);
        }
        return nullptr;
    }
    
    template<typename T>
    void removeComponent() {
        auto it = m_componentMap.find(std::type_index(typeid(T)));
        if (it != m_componentMap.end()) {
            ActorComponent* comp = it->second;
            comp->shutdown();
            
            m_components.erase(
                std::remove_if(m_components.begin(), m_components.end(),
                    [comp](const auto& c) { return c.get() == comp; }),
                m_components.end());
            
            m_componentMap.erase(it);
        }
    }
    
    const std::vector<std::unique_ptr<ActorComponent>>& getComponents() const {
        return m_components;
    }
    
    // Lifecycle (call components)
    void update(float deltaTime) override {
        ActorObject::update(deltaTime);
        for (auto& comp : m_components) {
            if (comp->isEnabled()) {
                comp->update(deltaTime);
            }
        }
    }
    
    void render(SDL_Renderer* renderer) override {
        ActorObject::render(renderer);
        for (auto& comp : m_components) {
            if (comp->isEnabled()) {
                comp->render(renderer);
            }
        }
    }
    
private:
    std::vector<std::unique_ptr<ActorComponent>> m_components;
    std::unordered_map<std::type_index, ActorComponent*> m_componentMap;
};

} // namespace engine
```

---

### Fas 3: CO-EXIST - Parallell Körning

**3.1 Migration Proxy Pattern**

```cpp
// engine/core/MigrationHelpers.h (NY FIL)
#pragma once
#include "CoreRedirects.h"
#include "ActorComponent.h"

namespace engine {

/**
 * @brief Helper to migrate old Sprite to new component system
 * 
 * This allows old code to work while new code uses components.
 */
class SpriteAdapter {
public:
    // Create from old Sprite
    static ActorObjectExtended* fromSprite(Sprite* sprite) {
        auto actor = std::make_unique<ActorObjectExtended>(sprite->getName());
        actor->setPosition(sprite->getPosition());
        
        // Add SpriteComponent with same texture
        auto* spriteComp = actor->addComponent<SpriteComponent>();
        spriteComp->setTexture(sprite->getTexture());
        
        return actor.release();
    }
    
    // Create from new ActorObject + SpriteComponent
    static Sprite* toSprite(ActorObjectExtended* actor) {
        auto* spriteComp = actor->getComponent<SpriteComponent>();
        if (!spriteComp) return nullptr;
        
        auto sprite = std::make_unique<Sprite>(actor->getName());
        sprite->setPosition(actor->getPosition());
        sprite->setTexture(spriteComp->getTexture());
        
        return sprite.release();
    }
};

} // namespace engine
```

**3.2 Gradvis Routing (Strangler Pattern)**

```cpp
// engine/world/Scene.h (MODIFIERA)
class Scene : public Object {
public:
    // OLD API (behåll för backward compat)
    void addChild(std::unique_ptr<Node> node) {
        // Route to new system if possible
        if (auto* extended = dynamic_cast<ActorObjectExtended*>(node.get())) {
            addActor(std::unique_ptr<ActorObjectExtended>(extended));
            node.release(); // Transfer ownership
        } else {
            // Fallback to old system
            m_legacyNodes.push_back(std::move(node));
        }
    }
    
    // NEW API (preferred)
    void addActor(std::unique_ptr<ActorObjectExtended> actor) {
        m_actors.push_back(std::move(actor));
    }
    
    void update(float deltaTime) override {
        // Update new actors
        for (auto& actor : m_actors) {
            actor->update(deltaTime);
        }
        
        // Update legacy nodes
        for (auto& node : m_legacyNodes) {
            node->update(deltaTime);
        }
    }
    
private:
    // New system
    std::vector<std::unique_ptr<ActorObjectExtended>> m_actors;
    
    // Legacy system (phasing out)
    std::vector<std::unique_ptr<Node>> m_legacyNodes;
};
```

**3.3 Feature Flags (Toggle Migration)**

```cpp
// engine/core/MigrationConfig.h (NY FIL)
#pragma once

namespace engine {

struct MigrationConfig {
    static bool useComponentSystem;
    static bool usePawnSystem;
    static bool useControllerSystem;
    
    // Toggle migration on/off
    static void enableMigration(bool enable) {
        useComponentSystem = enable;
        usePawnSystem = enable;
        useControllerSystem = enable;
    }
};

} // namespace engine
```

**Användning:**
```cpp
// main.cpp eller config
MigrationConfig::enableMigration(true);  // Enable new system
MigrationConfig::enableMigration(false); // Rollback to legacy
```

---

### Fas 4: ELIMINATE - Fasning ut av Legacy

**4.1 Deprecation Warnings**

```cpp
// engine/core/Node.h (MODIFIERA)
#pragma once

// Mark as deprecated
#ifdef __GNUC__
#define DEPRECATED(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER)
#define DEPRECATED(msg) __declspec(deprecated(msg))
#else
#define DEPRECATED(msg)
#endif

namespace engine {

// Mark legacy class as deprecated
class DEPRECATED("Node is deprecated, use Object instead") Node {
    // ... existing code
};

class DEPRECATED("Node2D is deprecated, use ActorObject instead") Node2D {
    // ... existing code
};

} // namespace engine
```

**4.2 Compile-time Checks**

```cpp
// engine/core/MigrationAssert.h (NY FIL)
#pragma once

// Phase out old API after certain date
#define MIGRATION_DEADLINE_YEAR 2026
#define MIGRATION_DEADLINE_MONTH 3

#if __cplusplus >= 202002L // C++20
#include <chrono>

namespace engine {

constexpr bool isMigrationDeadlinePassed() {
    // This would be evaluated at compile time in real scenario
    return false; // Set to true after deadline
}

} // namespace engine

#if isMigrationDeadlinePassed()
#error "Migration deadline passed! Remove all Node/Node2D usage and use Object/ActorObject"
#endif

#endif
```

**4.3 Migration Checklist**

```cpp
// tools/migration_checker.cpp (NY FIL)
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// Tool to check for legacy usage
void checkLegacyUsage(const std::string& sourceDir) {
    std::vector<std::string> legacyPatterns = {
        "Node2D",
        "class.*:.*public.*Entity",
        "new Sprite\\(",
    };
    
    // Scan all .cpp and .h files
    for (const auto& entry : std::filesystem::recursive_directory_iterator(sourceDir)) {
        if (entry.path().extension() == ".cpp" || entry.path().extension() == ".h") {
            // Check for legacy patterns
            // Report findings
        }
    }
}
```

---

## 📋 STEG-FÖR-STEG MIGRATIONS-PLAN (Säker)

### Sprint 1: Foundation (Vecka 1)
- [ ] Skapa `CoreRedirects.h` med alias
- [ ] Skapa `ActorComponent.h` och `SceneComponent.h`
- [ ] Utöka Node2D med `ActorObjectExtended`
- [ ] Skapa `MigrationConfig.h` med feature flags
- [ ] Testa kompilering (ska fungera utan ändringar)
- [ ] **KRITISKT:** Commit och tag detta som säker återgångspunkt

### Sprint 2: First Component (Vecka 2)
- [ ] Implementera `SpriteComponent`
- [ ] Skapa `SpriteAdapter` för migration
- [ ] Skapa **ETT** test-rum med nya systemet
- [ ] Jämför resultat: gamla Sprite vs SpriteComponent
- [ ] Mät prestanda: ingen försämring
- [ ] Dokumentera skillnader
- [ ] **KRITISKT:** Om problem → rollback till Sprint 1

### Sprint 3: Scene Routing (Vecka 3)
- [ ] Modifiera `Scene::addChild()` med routing
- [ ] Lägg till `Scene::addActor()` för nya systemet
- [ ] Testa parallell körning (gamla + nya systemet samtidigt)
- [ ] Verifiera att båda fungerar
- [ ] **KRITISKT:** Båda systemen måste fungera samtidigt

### Sprint 4: Pawn + Controller (Vecka 4)
- [ ] Skapa `Pawn.h`
- [ ] Skapa `Controller.h`, `PlayerController.h`, `AIController.h`
- [ ] Migrera `Character` att ärva från `Pawn`
- [ ] Testa Controller-system
- [ ] **KRITISKT:** Gamla Character-kod måste fungera

### Sprint 5: Gradvis Migration (Vecka 5-8)
- [ ] Migrera 1 rum/vecka till nya systemet
- [ ] Dokumentera lärdomar per rum
- [ ] Anpassa strategi baserat på problem
- [ ] Håll feature flags för att kunna toggle

### Sprint 6: Deprecation (Vecka 9+)
- [ ] Lägg till deprecation warnings
- [ ] Räkna usage av legacy API
- [ ] Planera fasning ut
- [ ] Sätt deadline för legacy removal

---

## ⚠️ RISK-HANTERING

### Risk 1: Breaking Changes
**Risk:** Kod slutar fungera efter migration

**Mitigation:**
- ✅ Feature flags (kan stänga av migration)
- ✅ Git tags för varje sprint (säker återgång)
- ✅ Parallell körning (båda systemen samtidigt)
- ✅ Automated tests (fånga regressioner)

### Risk 2: Halvfärdig Migration
**Risk:** Fastnar med två system för evigt

**Mitigation:**
- ✅ Sätt tydlig deadline
- ✅ Compile-time checks efter deadline
- ✅ Mät progress varje sprint
- ✅ Commitment från teamet

### Risk 3: Prestanda-försämring
**Risk:** Nya systemet är långsammare

**Mitigation:**
- ✅ Benchmark före/efter varje sprint
- ✅ Profiling av kritiska delar
- ✅ Optimera komponenter vid behov
- ✅ Rollback om > 10% prestanda-förlust

### Risk 4: Data-förlust
**Risk:** Assets/data går förlorad vid migration

**Mitigation:**
- ✅ Backup innan varje sprint
- ✅ Version control för alla assets
- ✅ Migration adapters (SpriteAdapter etc.)
- ✅ Validering: gamla == nya resultat

---

## ✅ SUCCESS CRITERIA

**Sprint är klar när:**
- [ ] All kod kompilerar utan errors
- [ ] Alla automated tests går igenom
- [ ] Manuell test visar korrekt beteende
- [ ] Prestanda är lika bra eller bättre
- [ ] Dokumentation uppdaterad
- [ ] Git tagged som säker återgångspunkt

**Migration är klar när:**
- [ ] 0% av koden använder legacy API
- [ ] Alla feature flags är ON (nya systemet)
- [ ] Legacy-kod borttagen från codebase
- [ ] Dokumentation reflekterar nya systemet
- [ ] Teamet är bekväma med nya systemet

---

## 🎯 FÖRSTA STEGET (Idag - 2 timmar)

**1. Skapa CoreRedirects.h (30 min)**
```cpp
// Implementera alias-systemet
```

**2. Skapa ActorComponent.h (30 min)**
```cpp
// Implementera component bas-klass
```

**3. Skapa Test (30 min)**
```cpp
// Verifiera att alias fungerar
// Verifiera att ActorComponent kompilerar
```

**4. Git Commit (10 min)**
```bash
git add .
git commit -m "feat(core): Add Core Redirects and Component system foundation"
git tag v0.1.0-migration-foundation
```

**5. Dokumentera (20 min)**
- Uppdatera DEVLOG
- Notera i session report
- Checka av Sprint 1 items

---

## 🤔 BESLUTSPUNKTER

**Innan vi börjar - besluta:**

1. **Feature Flags?**
   - [ ] Ja - Kan toggle migration ON/OFF
   - [ ] Nej - Migration är permanent från start

2. **Deprecation Warnings?**
   - [ ] Ja - Varna vid legacy usage
   - [ ] Nej - Tyst migration

3. **Deadline för Legacy Removal?**
   - [ ] 3 månader
   - [ ] 6 månader
   - [ ] När all kod är migrerad

4. **Rollback-strategi?**
   - [ ] Git revert per sprint
   - [ ] Feature flags
   - [ ] Båda

Vad tycker du? Är detta en säker approach?
