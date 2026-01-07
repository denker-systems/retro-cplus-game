---
description: Update project documentation after changes
auto_execution_mode: 1
---

# Update Docs Workflow

> Håll dokumentationen synkad med koden

## 1. Analysera Ändringar

**KRITISKT:** Kör ALLTID detta först!

// turbo
```powershell
git diff --stat HEAD~1
```

### Identifiera ALLA ändringar:
- [ ] Nya filer/klasser?
- [ ] Flyttade filer (arkitekturändring)?
- [ ] Nya features/system?
- [ ] Build-system ändringar?
- [ ] Bugfixes?

---

## 2. Dokumentationsmatris

| Ändring | Uppdatera |
|---------|-----------|
| Ny Actor/Component | Fil-header + klass-docs |
| Nytt system (engine/) | `docs/architecture/` + API |
| Ny feature | `CHANGELOG.md` |
| Bugfix | `CHANGELOG.md` |
| Arkitekturändring | `docs/architecture/` + ADR |
| Ny JSON-struktur | `assets/data/` schema |
| Editor-ändring | `docs/guides/editor.md` |

---

## 3. Kod-dokumentation

### Fil-header (Obligatorisk)
```cpp
/**
 * @file PlayerActor.cpp
 * @brief Spelarkaraktär med input och rörelse
 * 
 * @author [Team]
 * @date YYYY-MM-DD
 */
```

### Actor/Component Klass
```cpp
/**
 * @class PlayerActor
 * @brief Spelarens kontrollerade karaktär
 * 
 * @details
 * Hanterar:
 * - Keyboard/mouse input
 * - Point-and-click rörelse
 * - Walk area begränsningar
 * 
 * @see CharacterActor
 * @see SpriteComponent
 */
```

### Metod
```cpp
/**
 * @brief Flytta mot position med pathfinding
 * 
 * @param target Målposition i world space
 * @return true om path hittades
 */
bool moveTo(Vec2 target);
```

---

## 4. Projektdokumentation

### docs/CHANGELOG.md
```markdown
## [Unreleased]

### Added
- `PlayerActor` med walk area support
- `SpriteComponent` rendering pipeline

### Changed
- Scene använder nu WorldContainer

### Fixed
- Walk area laddas korrekt från JSON (#123)
```

### docs/ROADMAP.md
```markdown
## Fas 5: Actor System
- [x] Object/ActorObject base classes
- [x] Component system
- [ ] Full SpriteComponent rendering
- [ ] AnimationComponent integration
```

---

## 5. Arkitekturdokumentation

### När krävs uppdatering?
- Ny actor-typ eller component
- Nya system-beroenden
- Ändrad data-flow
- Nya design patterns

### docs/architecture/actors.md
```markdown
## Actor Hierarki

\`\`\`
Object (abstract)
└── ActorObject
    └── ActorObjectExtended
        ├── CharacterActor
        │   ├── PlayerActor
        │   └── NPCActor
        └── SpriteActor
\`\`\`
```

---

## 6. JSON Schema Dokumentation

Vid ändringar i `assets/data/`:

```markdown
## scenes.json

| Fält | Typ | Beskrivning |
|------|-----|-------------|
| id | string | Unikt scene-ID |
| name | string | Visningsnamn |
| gridPosition | {x,y} | Position i level-grid |
| walkArea | {minX,maxX,minY,maxY} | Tillåtet rörelseområde |
```

---

## 7. Verifiera

// turbo
```powershell
cd build; cmake --build . --config Release
```

---

## Checklista

### Kod
- [ ] Fil-headers i nya filer
- [ ] Klass-dokumentation
- [ ] Publika metoder dokumenterade
- [ ] Inline-kommentarer förklarar VARFÖR

### Projekt
- [ ] CHANGELOG uppdaterad
- [ ] ROADMAP uppdaterad (vid milestones)
- [ ] README aktuell

### Arkitektur (vid behov)
- [ ] System-diagram uppdaterat
- [ ] Actor/Component-hierarki dokumenterad
- [ ] JSON-schema dokumenterat
