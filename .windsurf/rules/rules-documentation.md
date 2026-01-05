---
description: Dokumentationsstruktur och regler för systematisk projektdokumentation
trigger: always_on
---

# Documentation Rules

## Principer

1. **En fil per ämne** - Undvik monolitiska dokumentfiler
2. **Hierarkisk struktur** - Mappar för kategorier
3. **Konsekvent format** - Samma struktur i alla filer
4. **Uppdateras vid kodändring** - Dokumentation följer koden
5. **Versionerad** - Allt i git

---

## Filstruktur

```
docs/
├── README.md                 # Projektöversikt, quick start
├── ROADMAP.md               # Utvecklingsplan, milestones
├── CHANGELOG.md             # Versionshistorik
│
├── architecture/            # Teknisk arkitektur
│   ├── overview.md          # Systemöversikt
│   ├── entities.md          # Entity-hierarki
│   ├── states.md            # State pattern
│   ├── components.md        # Component system
│   └── systems.md           # Game systems
│
├── api/                     # Klass/API-dokumentation
│   ├── core/
│   │   ├── Game.md
│   │   └── Input.md
│   ├── entities/
│   │   ├── Entity.md
│   │   ├── Character.md
│   │   └── PlayerCharacter.md
│   ├── graphics/
│   │   ├── TextureManager.md
│   │   └── Animation.md
│   └── audio/
│       └── AudioManager.md
│
├── guides/                  # Tutorials och how-tos
│   ├── getting-started.md   # Setup och första körning
│   ├── adding-room.md       # Hur man lägger till rum
│   ├── adding-npc.md        # Hur man skapar NPC
│   └── adding-dialog.md     # Hur man skapar dialog
│
├── design/                  # Game design dokumentation
│   ├── story.md             # Berättelse och handling
│   ├── characters.md        # Karaktärsbeskrivningar
│   ├── rooms.md             # Rum och platser
│   └── puzzles.md           # Pussel och lösningar
│
└── technical/               # Tekniska specifikationer
    ├── build.md             # Build-instruktioner
    ├── dependencies.md      # Externa beroenden
    ├── data-formats.md      # JSON-scheman etc.
    └── performance.md       # Optimeringsnoteringar
```

---

## Dokumenttyper

### 1. README.md (Rot)
```markdown
# Projektnamn

Kort beskrivning (1-2 meningar).

## Features
- Feature 1
- Feature 2

## Quick Start
\`\`\`bash
# Byggkommando
\`\`\`

## Documentation
- [Architecture](docs/architecture/overview.md)
- [Getting Started](docs/guides/getting-started.md)

## License
```

### 2. Architecture Docs
```markdown
# [System/Modul Namn]

## Översikt
Kort beskrivning av systemet.

## Diagram
\`\`\`
[ASCII eller Mermaid diagram]
\`\`\`

## Komponenter
| Klass | Ansvar |
|-------|--------|
| ... | ... |

## Relationer
Hur denna modul interagerar med andra.

## Exempel
\`\`\`cpp
// Kodexempel
\`\`\`
```

### 3. API Docs (per klass)
```markdown
# ClassName

> Kort beskrivning

## Includes
\`\`\`cpp
#include "path/to/ClassName.h"
\`\`\`

## Konstruktor
\`\`\`cpp
ClassName(param1, param2);
\`\`\`

## Publika Metoder

### methodName
\`\`\`cpp
ReturnType methodName(ParamType param);
\`\`\`
**Beskrivning:** Vad metoden gör.
**Parametrar:** 
- `param` - Beskrivning
**Returnerar:** Beskrivning

## Exempel
\`\`\`cpp
// Användningsexempel
\`\`\`

## Se även
- [RelatedClass](RelatedClass.md)
```

### 4. Guide Docs
```markdown
# How to: [Uppgift]

## Förutsättningar
- Krav 1
- Krav 2

## Steg

### 1. Första steget
Beskrivning...

\`\`\`cpp
// Kod
\`\`\`

### 2. Andra steget
...

## Resultat
Vad användaren ska ha uppnått.

## Felsökning
| Problem | Lösning |
|---------|---------|
| ... | ... |
```

---

## Naming Conventions

| Typ | Format | Exempel |
|-----|--------|---------|
| Mappar | kebab-case | `architecture/`, `api/` |
| Filer | kebab-case.md | `getting-started.md` |
| API-filer | PascalCase.md | `AudioManager.md` |
| Bilder | kebab-case | `state-diagram.png` |

---

## När dokumentera

### Alltid dokumentera:
- Nya klasser/system
- Publika API:er
- Arkitekturbeslut
- Breaking changes
- Setup/installation

### Uppdatera vid:
- Ändrad funktionalitet
- Nya parametrar
- Deprecated features
- Bugfixes som påverkar beteende

---

## Workflow

### Vid ny feature:
1. Skapa API-doc i `docs/api/[modul]/`
2. Uppdatera relevant architecture-doc
3. Lägg till i CHANGELOG.md
4. Uppdatera ROADMAP.md progress

### Vid refactoring:
1. Uppdatera påverkade API-docs
2. Uppdatera architecture om strukturen ändras
3. Notera i CHANGELOG.md

---

## Länkar

Använd relativa länkar mellan docs:
```markdown
Se [Entity-hierarkin](../architecture/entities.md)
```

Länka till kod med GitHub-format:
```markdown
Se implementation i [`src/entities/Entity.h`](../../src/entities/Entity.h)
```

---

## Checklista

- [ ] README.md uppdaterad?
- [ ] ROADMAP.md progress uppdaterad?
- [ ] Ny klass → API-doc skapad?
- [ ] Arkitekturändring → architecture/ uppdaterad?
- [ ] Breaking change → CHANGELOG.md noterad?
