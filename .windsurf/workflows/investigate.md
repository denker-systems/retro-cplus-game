---
description: Systematisk buggundersökning och root cause analysis
auto_execution_mode: 1
---

# Investigate Workflow

> Komplett root cause analysis för Retro Engine

## 1. Samla Information

### Reproducera Buggen

```powershell
# Bygg och kör Debug
cd build
cmake --build . --config Debug
.\Debug\RetroGame.exe
```

### Dokumentera Buggen

| Information | Beskrivning |
|-------------|-------------|
| **Symptom** | Vad händer? |
| **Förväntad** | Vad borde hända? |
| **Steg att reproducera** | Exakta steg 1, 2, 3... |
| **Frekvens** | Alltid? Ibland? Slumpmässigt? |
| **Environment** | Debug/Release? Windows version? |
| **Senaste fungerande** | Vilken commit fungerade senast? |

### Bug Report Template

```markdown
## Bug: [Kort beskrivning]

### Symptom
[Vad händer]

### Förväntad
[Vad borde hända]

### Steg att reproducera
1. Starta spelet
2. Gå till rum X
3. Klicka på Y
4. Bug uppstår

### Frekvens
[Alltid / Ibland / Slumpmässigt]

### Error messages
```
[Kopiera error output här]
```

### Screenshots/Video
[Om relevant]

### Environment
- Build: Debug/Release
- Commit: [hash]
- OS: Windows 11

### Relaterade filer
- `src/engine/xxx.cpp`
```

---

## 2. Klassificera Buggen

### Bugtyper

| Typ | Symptom | Primärt Verktyg |
|-----|---------|-----------------|
| **Crash** | Program avslutas | Debugger, stack trace |
| **Freeze** | Program hänger | Debugger, break |
| **Logic** | Fel beteende | Logging, debugger |
| **Data** | Data laddas/sparas fel | JSON logging |
| **Memory** | Korruption, leak | ASan, Valgrind |
| **Rendering** | Visuellt fel | Logging, RenderDoc |
| **Audio** | Ljud saknas/fel | Mix_GetError() |
| **Performance** | Långsamt | Profiler |
| **Input** | Input fungerar ej | Event logging |

### Prioritering

| Prioritet | Typ | Åtgärd |
|-----------|-----|--------|
| 🔴 **P1** | Crash, data loss | Fixa omedelbart |
| 🟠 **P2** | Bruten feature | Fixa denna session |
| 🟡 **P3** | Minor bug | Fixa när tid finns |
| 🟢 **P4** | Kosmetiskt | Låg prioritet |

---

## 3. Isolera Problemet

### Strategi 1: Binary Search med Git Bisect

```powershell
# Starta bisect
git bisect start

# Markera nuvarande commit som bad
git bisect bad HEAD

# Markera senast kända fungerande commit som good
git bisect good abc123

# Git checkar ut en commit i mitten
# Testa om buggen finns
# Markera som good eller bad
git bisect good  # eller git bisect bad

# Upprepa tills git hittar första bad commit
# ...

# Avsluta bisect
git bisect reset
```

### Strategi 2: Minimal Reproduction

Skapa minimal test som reproducerar buggen:

```cpp
// test_BugXXX.cpp
#include <gtest/gtest.h>
#include "engine/world/Scene.h"
#include "engine/core/PlayerActor.h"

TEST(BugRepro, WalkAreaNotLoaded) {
    // Minimal setup
    Scene scene("test_room");
    scene.loadFromJSON("assets/data/scenes.json");
    
    // Hämta player
    auto* player = scene.getActor<PlayerActor>("player");
    ASSERT_NE(player, nullptr);
    
    // Verifiera buggen
    // BUG: Walk area är 0,0,0,0 istället för korrekta värden
    EXPECT_GT(player->getWalkMaxX(), 0);
    EXPECT_GT(player->getWalkMaxY(), 0);
}
```

### Strategi 3: Divide and Conquer

1. **Identifiera subsystem** - Vilken del av koden?
2. **Isolera input** - Vad triggar buggen?
3. **Spåra dataflöde** - Var går det fel?

```cpp
// Lägg till logging på varje steg
void DataLoader::loadRoom(const std::string& path) {
    std::cout << "[1] loadRoom called with: " << path << std::endl;
    
    // ... ladda JSON ...
    std::cout << "[2] JSON loaded, rooms: " << json.size() << std::endl;
    
    // ... parse walk area ...
    std::cout << "[3] Walk area: " 
              << wa.minX << "," << wa.maxX << "," 
              << wa.minY << "," << wa.maxY << std::endl;
    
    // ... returnera data ...
    std::cout << "[4] Returning room data" << std::endl;
}
```

---

## 4. Vanliga Bugmönster i Retro Engine

### 4.1 Null Pointer

```cpp
// BUG: actor kan vara nullptr
actor->update(dt);

// FIX 1: Null check
if (actor) {
    actor->update(dt);
}

// FIX 2: Assertion (debug)
assert(actor != nullptr && "Actor must not be null");
actor->update(dt);

// FIX 3: Optional reference
if (auto* actor = scene.getActor("player")) {
    actor->update(dt);
}
```

### 4.2 JSON Field Saknas

```cpp
// BUG: Kraschar om field saknas
int x = json["walkArea"]["minX"];

// FIX: Kontrollera existens
if (json.contains("walkArea")) {
    auto& wa = json["walkArea"];
    if (wa.contains("minX")) {
        int x = wa["minX"];
    }
}

// FIX 2: Använd value() med default
int x = json.value("minX", 0);
```

### 4.3 Ownership/Move Semantics

```cpp
// BUG: Use after move
auto actor = std::make_unique<PlayerActor>("player");
scene->addActor(std::move(actor));
actor->setPosition(100, 200);  // CRASH! actor är nullptr

// FIX: Spara pekare före move
auto actor = std::make_unique<PlayerActor>("player");
auto* actorPtr = actor.get();  // Spara raw pointer
scene->addActor(std::move(actor));
actorPtr->setPosition(100, 200);  // OK!
```

### 4.4 Iterator Invalidation

```cpp
// BUG: Modifierar vector under iteration
for (auto& actor : m_actors) {
    if (actor->shouldRemove()) {
        m_actors.erase(...);  // CRASH! Iterator invaliderad
    }
}

// FIX: Remove-erase idiom
m_actors.erase(
    std::remove_if(m_actors.begin(), m_actors.end(),
        [](const auto& a) { return a->shouldRemove(); }),
    m_actors.end()
);
```

### 4.5 Float Comparison

```cpp
// BUG: Float comparison med ==
if (position.x == target.x) {
    // Sällan true p.g.a. floating point precision
}

// FIX: Använd epsilon
const float EPSILON = 0.001f;
if (std::abs(position.x - target.x) < EPSILON) {
    // Korrekt jämförelse
}
```

### 4.6 Off-by-One

```cpp
// BUG: Läser utanför array
for (int i = 0; i <= array.size(); i++) {  // <= istället för <
    process(array[i]);  // CRASH när i == size
}

// FIX
for (int i = 0; i < array.size(); i++) {
    process(array[i]);
}

// BÄTTRE: Range-based for
for (const auto& item : array) {
    process(item);
}
```

### 4.7 Data Sync Issue

```cpp
// BUG: Data synkas inte före sparning
void saveGame() {
    // Scene har ändringar som ej synkats till RoomData
    DataLoader::saveRooms();  // Sparar gamla data!
}

// FIX: Synka före sparning
void saveGame() {
    syncScenesToRoomData();  // Synka först
    DataLoader::saveRooms();  // Nu sparas korrekt data
}
```

---

## 5. Debug Strategier

### Logging-strategi

```cpp
// Strukturerad logging
void Actor::update(float dt) {
    std::cout << "[Actor::update] ENTER"
              << " name=" << m_name
              << " dt=" << dt
              << std::endl;
    
    // ... kod ...
    
    std::cout << "[Actor::update] EXIT"
              << " position=(" << m_position.x << "," << m_position.y << ")"
              << std::endl;
}
```

### Breakpoint-strategi

1. Sätt breakpoint vid start av misstänkt funktion
2. Step through och watch variabler
3. Identifiera var värden blir fel
4. Narrowa ner till specifik rad

### State Inspection

```cpp
// Debug metod för att inspektera state
void Scene::debugDump() {
    std::cout << "=== Scene Dump ===" << std::endl;
    std::cout << "Name: " << m_name << std::endl;
    std::cout << "Actors: " << m_actors.size() << std::endl;
    
    for (const auto& actor : m_actors) {
        std::cout << "  - " << actor->getName()
                  << " pos=(" << actor->getPosition().x 
                  << "," << actor->getPosition().y << ")"
                  << std::endl;
    }
}
```

---

## 6. Dokumentera Root Cause

### Bug Report Update

```markdown
## Root Cause Analysis

### Problem
Walk area laddades inte från JSON, resulterade i default-värden (0,0,0,0).

### Root Cause
`DataLoader::loadRoom()` parsade inte `walkArea` fältet från JSON.
Fältet existerade i scenes.json men ignorerades vid laddning.

### Location
- **File:** `src/engine/data/DataLoader.cpp`
- **Function:** `loadRoom()`
- **Line:** ~142

### Fix
Lade till parsing av walkArea i loadRoom():
```cpp
if (json.contains("walkArea")) {
    room.walkArea.minX = json["walkArea"]["minX"];
    room.walkArea.maxX = json["walkArea"]["maxX"];
    room.walkArea.minY = json["walkArea"]["minY"];
    room.walkArea.maxY = json["walkArea"]["maxY"];
}
```

### Regression Test
Lade till `test_DataLoader_WalkArea` som verifierar att walk area laddas korrekt.

### Lessons Learned
- Validera att alla JSON fields parsas
- Lägg till tester för data loading
```

---

## 7. Implementera Fix

### Guidelines
1. **Fixa root cause** - Inte bara symptomen
2. **Minimal ändring** - Rör inte onödig kod
3. **Lägg till test** - Förhindra regression
4. **Dokumentera** - Förklara varför

### Verifiera Fix

```powershell
# Build
cmake --build . --config Release

# Kör tester
ctest -C Release --output-on-failure

# Manual test
.\Release\RetroGame.exe
# Verifiera att buggen är fixad

# Verifiera inga regressioner
# Testa relaterade features
```

---

## 8. Commit Fix

```powershell
git add src/engine/data/DataLoader.cpp
git commit -m "fix(data): walk area loads correctly from scenes.json

Root cause: JSON field 'walkArea' was not being parsed in DataLoader::loadRoom().
Walk area coordinates defaulted to 0,0,0,0.

Solution: Added parsing of walkArea field with proper null-checking.

Added: test_DataLoader_WalkArea regression test

Fixes: #XXX"
```

---

## Debug Verktyg

| Verktyg | Användning | När |
|---------|------------|-----|
| **Visual Studio Debugger** | Breakpoints, watch, call stack | Crashes, logic errors |
| **std::cout logging** | Quick debug output | Alla buggar |
| **Git bisect** | Hitta regression | "Det fungerade innan" |
| **SDL_GetError()** | SDL2-relaterade fel | Rendering, window, input |
| **IMG_GetError()** | Texture loading fel | Saknade texturer |
| **Mix_GetError()** | Audio fel | Ljud-problem |
| **Address Sanitizer** | Memory bugs | Korruption, leaks |
| **JSON dump** | Se laddad data | Data loading issues |

---

## Investigation Checklista

### Innan du börjar
- [ ] Kan du reproducera buggen konsekvent?
- [ ] Har du dokumenterat exakta steg?
- [ ] Har du kollat senaste commits?

### Under investigation
- [ ] Klassificerat bugtyp
- [ ] Isolerat problemområde
- [ ] Identifierat root cause
- [ ] Verifierat med minimal test

### Efter fix
- [ ] Fix verifierad
- [ ] Regression test tillagd
- [ ] Dokumentation uppdaterad
- [ ] Inga nya buggar introducerade
