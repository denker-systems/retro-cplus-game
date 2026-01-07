
---
description: Comprehensive documentation update workflow for large teams
---

# Update Documentation Workflow (Corporate Edition)

> Komplett dokumentationsworkflow för 50+ utvecklarteam

## ÖVERSIKT

```
Code Change  API Docs  Architecture  Guides  Release Notes
                                                             
                                                             
  Doxygen       docs/api     docs/arch      docs/guides    CHANGELOG
```

---

## STEG 1: ANALYSERA ÄNDRINGAR

### 1.1 Se alla ändringar
// turbo
```powershell
git diff --stat HEAD~1
```

### 1.2 Identifiera dokumentationsbehov
| Ändring | Dokumentation som behövs |
|---------|--------------------------|
| Ny klass | API doc, Architecture (om system) |
| Ny metod | API doc (Doxygen) |
| Ändrad signatur | API doc, eventuellt Migration guide |
| Ny feature | Guide, CHANGELOG, ROADMAP |
| Bugfix | CHANGELOG |
| Arkitekturändring | ADR, Architecture docs |
| Breaking change | Migration guide, CHANGELOG |

---

## STEG 2: INLINE DOKUMENTATION

### 2.1 Verifiera fil-headers
```cpp
/**
 * @file NewFile.cpp
 * @brief Kort beskrivning
 * 
 * @details Längre beskrivning...
 */
```

### 2.2 Verifiera klass-dokumentation
```cpp
/**
 * @class NewClass
 * @brief Kort beskrivning
 * 
 * @details Detaljerad beskrivning...
 */
```

### 2.3 Verifiera metod-dokumentation
```cpp
/**
 * @brief Kort beskrivning
 * @param[in] param Beskrivning
 * @return Beskrivning
 */
```

---

## STEG 3: API DOCUMENTATION

### 3.1 Generera Doxygen (om konfigurerat)
```powershell
doxygen Doxyfile
```

### 3.2 Manuell API doc (om ingen Doxygen)
Skapa `docs/api/[Modul]/ClassName.md`:

```markdown
# ClassName

> Kort beskrivning

## Include
\\\cpp
#include "path/to/ClassName.h"
\\\

## Constructor
\\\cpp
ClassName(param1, param2);
\\\

## Public Methods

### methodName
\\\cpp
ReturnType methodName(ParamType param);
\\\
**Description:** Vad metoden gör.
**Parameters:**
- `param` - Beskrivning
**Returns:** Beskrivning

## Example
\\\cpp
// Användningsexempel
ClassName obj;
obj.methodName(value);
\\\

## See Also
- [RelatedClass](RelatedClass.md)
```

---

## STEG 4: ARCHITECTURE DOCUMENTATION

### 4.1 När uppdatera architecture docs?
- Ny modul/system
- Nya beroenden mellan system
- Ändrad dataflöde
- Nya design patterns

### 4.2 Architecture doc format
```markdown
# [System Name]

## Overview
Kort beskrivning av systemet.

## Diagram
\\\mermaid
classDiagram
    ClassA --> ClassB
    ClassB --> ClassC
\\\

## Components
| Component | Responsibility |
|-----------|----------------|
| ComponentA | ... |
| ComponentB | ... |

## Data Flow
1. Step 1
2. Step 2
3. Step 3

## Dependencies
- DependencyA
- DependencyB
```

### 4.3 ADR för arkitekturbeslut
Om ett viktigt beslut togs, skapa `docs/adr/NNN-title.md`

---

## STEG 5: DEVELOPER GUIDES

### 5.1 När skriva guide?
- Ny feature som utvecklare behöver använda
- Komplex setup eller konfiguration
- Vanliga use cases

### 5.2 Guide format
```markdown
# How to: [Task]

## Prerequisites
- Requirement 1
- Requirement 2

## Steps

### 1. First step
Description...

\\\cpp
// Code example
\\\

### 2. Second step
Description...

## Result
What the user should have achieved.

## Troubleshooting
| Problem | Solution |
|---------|----------|
| ... | ... |
```

---

## STEG 6: RELEASE NOTES

### 6.1 CHANGELOG.md format
```markdown
## [Unreleased]

### Added
- New feature description (#issue)

### Changed
- Changed behavior description (#issue)

### Fixed
- Bug fix description (#issue)

### Deprecated
- Deprecated feature description

### Removed
- Removed feature description

### Security
- Security fix description
```

### 6.2 ROADMAP.md update
```markdown
## Phase X: [Name]
- [x] Completed feature
- [ ] In progress feature (XX%)
- [ ] Planned feature
```

---

## STEG 7: VERIFIERING

### 7.1 Checklista
- [ ] Fil-headers i alla nya filer?
- [ ] Klasser dokumenterade?
- [ ] Publika metoder dokumenterade?
- [ ] API docs uppdaterade?
- [ ] Architecture docs uppdaterade (om relevant)?
- [ ] CHANGELOG uppdaterad?
- [ ] ROADMAP uppdaterad?
- [ ] Guides skapade (om relevant)?
- [ ] ADR skapad (om arkitekturbeslut)?

### 7.2 Bygg för att verifiera
// turbo
```powershell
cd build; cmake --build . --config Release
```

---

## AUTOMATISERING

### Pre-commit hook för dokumentation
```bash
#!/bin/bash
# Verifiera att alla .h-filer har fil-header
for file in ; do
    if ! grep -q "@file" ""; then
        echo "ERROR:  saknar fil-header"
        exit 1
    fi
done
```
