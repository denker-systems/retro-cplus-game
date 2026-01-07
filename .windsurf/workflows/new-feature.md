---
description: Corporate New Feature Workflow for Large Development Teams
---

# New Feature Workflow (Corporate Edition)

> Komplett feature workflow för 50+ utvecklarteam

## ÖVERSIKT

```
Issue  Branch  Design  Implement  Test  Document  PR  Review  Merge
```

---

## STEG 1: ISSUE & PLANNING

### 1.1 Verifiera issue finns
```powershell
# Se issue-detaljer på GitHub
# Issue ska ha:
# - Clear acceptance criteria
# - Size estimate (T-shirt: S/M/L/XL)
# - Priority (P0-P3)
# - Assigned developer
```

### 1.2 Design review (för L/XL features)
- Skapa design doc om komplext
- Diskutera arkitektur med team
- Skapa ADR om arkitekturbeslut

---

## STEG 2: BRANCH SETUP

### 2.1 Skapa feature branch
```powershell
git checkout main
git pull origin main
git checkout -b feature/XXX-short-description
```

### 2.2 Branch naming convention
```
feature/XXX-description   # Ny funktionalitet
bugfix/XXX-description    # Bug fix
hotfix/XXX-description    # Kritisk fix
refactor/XXX-description  # Refactoring
docs/XXX-description      # Dokumentation
```

---

## STEG 3: IMPLEMENTATION

### 3.1 Skapa filer enligt arkitektur
```
src/
 [modul]/
     NewClass.h       # Header med dokumentation
     NewClass.cpp     # Implementation
```

### 3.2 Header-fil mall
```cpp
/**
 * @file NewClass.h
 * @brief Kort beskrivning av klassen
 * 
 * @details
 * Detaljerad beskrivning av:
 * - Klassens syfte
 * - Hur den används
 * - Beroenden
 */
#pragma once

#include <memory>
#include <string>

namespace engine {

/**
 * @class NewClass
 * @brief Kort beskrivning
 * 
 * @details
 * Detaljerad beskrivning...
 */
class NewClass {
public:
    /**
     * @brief Constructor
     * @param name Klassens namn
     */
    explicit NewClass(const std::string& name);
    
    /**
     * @brief Destructor
     */
    ~NewClass();
    
    // Delete copy (om relevant)
    NewClass(const NewClass&) = delete;
    NewClass& operator=(const NewClass&) = delete;
    
    // Allow move (om relevant)
    NewClass(NewClass&&) = default;
    NewClass& operator=(NewClass&&) = default;
    
    /**
     * @brief Public method
     * @param param Description
     * @return Description
     */
    ReturnType methodName(ParamType param);

private:
    std::string m_name;  ///< Brief description
};

} // namespace engine
```

### 3.3 Implementation-fil mall
```cpp
/**
 * @file NewClass.cpp
 * @brief Implementation of NewClass
 */
#include "NewClass.h"
#include <iostream>

namespace engine {

NewClass::NewClass(const std::string& name)
    : m_name(name)
{
    // Initialization logic
}

NewClass::~NewClass() {
    // Cleanup logic
}

ReturnType NewClass::methodName(ParamType param) {
    // Implementation
}

} // namespace engine
```

### 3.4 Uppdatera CMakeLists.txt
```cmake
set(ENGINE_SOURCES
    # ... existing sources
    src/modul/NewClass.cpp
)
```

---

## STEG 4: TESTING

### 4.1 Unit tests
```cpp
// tests/test_NewClass.cpp
#include <gtest/gtest.h>
#include "engine/modul/NewClass.h"

TEST(NewClassTest, ConstructorSetsName) {
    engine::NewClass obj("TestName");
    EXPECT_EQ(obj.getName(), "TestName");
}

TEST(NewClassTest, MethodReturnsExpectedValue) {
    engine::NewClass obj("Test");
    EXPECT_EQ(obj.methodName(input), expectedOutput);
}
```

### 4.2 Bygg och kör tester
// turbo
```powershell
cd build; cmake --build . --config Release
ctest -C Release --output-on-failure
```

---

## STEG 5: DOCUMENTATION

### 5.1 Inline documentation
- Fil-header med @file och @brief
- Klass-dokumentation med @class
- Metod-dokumentation med @brief, @param, @return

### 5.2 API documentation
Skapa `docs/api/[Modul]/NewClass.md` om extern API

### 5.3 Update CHANGELOG
```markdown
## [Unreleased]
### Added
- NewClass for [functionality] (#XXX)
```

### 5.4 Update ROADMAP
Markera feature som klar: `[x]`

---

## STEG 6: PULL REQUEST

### 6.1 Pre-PR checklista
- [ ] Kod bygger (Debug + Release)
- [ ] Alla tester passerar
- [ ] Self-review genomförd
- [ ] Dokumentation komplett
- [ ] CHANGELOG uppdaterad

### 6.2 Pusha och skapa PR
```powershell
git add .
git commit -m "feat(modul): add NewClass for feature description"
git push origin feature/XXX-short-description
```

### 6.3 PR description
Använd PR template med:
- Summary
- Type of Change
- Changes Made
- Testing
- Documentation
- Related Issues

---

## STEG 7: CODE REVIEW

### 7.1 Svara på feedback
- Åtgärda blockers direkt
- Diskutera majors
- Acceptera/ignorera nits

### 7.2 Uppdatera PR
```powershell
git add .
git commit -m "fix: address review feedback"
git push
```

---

## STEG 8: MERGE

### 8.1 Efter approval
- Squash merge till main
- Ta bort feature branch
- Verifiera CI passerar

### 8.2 Post-merge
- Stäng relaterade issues
- Informera team i Slack/Teams
