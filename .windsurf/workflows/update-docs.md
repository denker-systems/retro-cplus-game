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
| Nytt system (engine/) | `docs/dev/DEVLOG.md` + session rapport |
| Ny feature | `docs/dev/DEVLOG.md` + `CHANGELOG.md` |
| Bugfix | `docs/dev/DEVLOG.md` + `CHANGELOG.md` |
| Arkitekturändring | `docs/dev/sessions/YYYY-MM-DD.md` med detaljer |
| Ny JSON-struktur | `assets/data/` schema + DEVLOG |
| Editor-ändring | `docs/dev/DEVLOG.md` + session rapport |

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

### docs/dev/DEVLOG.md (PRIMÄR)
```markdown
## YYYY-MM-DD

### Kväll (HH:MM-HH:MM)

- `abc123` type(scope): beskrivning
  - **Nya filer:** Lista alla nya filer
  - **Borttagna:** Lista borttagna filer
  - **Statistik:** X filer, +A/-B rader
  - **Features:** Beskriv nya features
  - **Verifierat:** Lista vad som testats
- Session: [YYYY-MM-DD](sessions/YYYY-MM-DD.md)
```

### docs/dev/sessions/YYYY-MM-DD.md (SEKUNDÄR)
Detaljerad session rapport med:
- Commits denna session
- Utfört arbete
- Nya/borttagna/modifierade filer
- Tekniska beslut
- Nästa session
- Handoff notes

### docs/CHANGELOG.md (TERTIÄR)
```markdown
## [Unreleased]

### Added
- Feature beskrivning

### Changed
- Ändring beskrivning

### Fixed
- Buggfix beskrivning
```

---

## 5. JSON Schema Dokumentation

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

## 6. Verifiera

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

### Projekt (PRIMÄRT)
- [ ] **docs/dev/DEVLOG.md** uppdaterad med commit detaljer
- [ ] **docs/dev/sessions/YYYY-MM-DD.md** session rapport komplett
- [ ] **docs/CHANGELOG.md** uppdaterad (vid releases)

### Arkitektur (vid stora ändringar)
- [ ] Skapa docs/architecture/[feature].md om nödvändigt
- [ ] Dokumentera tekniska beslut i session rapport
