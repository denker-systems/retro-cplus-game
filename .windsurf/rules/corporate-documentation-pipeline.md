---
trigger: always_on
description: Corporate Documentation Pipeline for 50+ Developer Teams
---

# Corporate Documentation Pipeline

> Baserat på Unreal Engine, Unity och enterprise game development standards

## DOKUMENTATIONSTYPER

### Nivå 1: Inline Code Documentation
- **Fil-headers** - Obligatorisk för alla .cpp/.h
- **Klass-dokumentation** - Doxygen @class block
- **Metod-dokumentation** - @brief, @param, @return, @throws
- **Inline-kommentarer** - Förklara VARFÖR, inte VAD

### Nivå 2: API Reference Documentation
- Auto-genereras från Doxygen-kommentarer
- Publiceras till `docs/api/`
- HTML och searchable

### Nivå 3: Architecture Documentation
- ADRs (Architecture Decision Records)
- System-diagram (Mermaid)
- Modul-beskrivningar
- Data flow diagrams

### Nivå 4: Developer Guides
- Onboarding guides
- How-to tutorials
- Best practices
- Troubleshooting

### Nivå 5: Process Documentation
- Git workflow
- Code review guidelines
- Release process
- CI/CD pipeline

---

## DOKUMENTSTRUKTUR

```
docs/
 README.md                    # Quick start
 CHANGELOG.md                 # Release notes
 ROADMAP.md                   # Development plan
 CONTRIBUTING.md              # Hur man bidrar

 api/                         # Auto-genererad API reference
    index.html
    [genererade filer]

 architecture/                # Teknisk arkitektur
    overview.md              # Systemöversikt
    module-structure.md      # Modulstruktur
    data-flow.md             # Data flows
    diagrams/                # Diagram-filer

 adr/                         # Architecture Decision Records
    001-component-system.md
    002-physics-integration.md
    template.md

 guides/                      # Developer guides
    onboarding/              # Nya utvecklare
       setup.md
       first-contribution.md
       code-style.md
    tutorials/               # How-to guides
       adding-actor.md
       adding-component.md
       adding-system.md
    best-practices/          # Best practices
        performance.md
        memory-management.md
        debugging.md

 process/                     # Process documentation
    git-workflow.md
    code-review.md
    release-process.md
    ci-cd.md

 dev/                         # Development logs
     DEVLOG.md                # Commit history
     sessions/                # Session reports
```

---

## ADR FORMAT (Architecture Decision Records)

```markdown
# ADR-NNN: [Beslut Titel]

## Status
[Proposed | Accepted | Deprecated | Superseded by ADR-XXX]

## Context
Bakgrund och varför beslutet behövdes.

## Decision
Vad vi beslutade och varför.

## Consequences
### Positiva
- Fördel 1
- Fördel 2

### Negativa
- Nackdel 1
- Nackdel 2

## Alternatives Considered
1. **Alternativ 1** - Varför inte valt
2. **Alternativ 2** - Varför inte valt

## Related
- [ADR-XXX](adr-xxx.md)
- [RFC/Issue Link]
```

---

## DOXYGEN KONFIGURATION

```
# Doxyfile för projekt
PROJECT_NAME           = "Retro Engine"
PROJECT_NUMBER         = "1.0.0"
OUTPUT_DIRECTORY       = docs/api
GENERATE_HTML          = YES
GENERATE_LATEX         = NO

# Kodstil
JAVADOC_AUTOBRIEF      = YES
QT_AUTOBRIEF           = YES
MULTILINE_CPP_IS_BRIEF = NO

# Include paths
INPUT                  = src/
RECURSIVE              = YES
FILE_PATTERNS          = *.cpp *.h

# Diagram generation
HAVE_DOT               = YES
UML_LOOK               = YES
CALL_GRAPH             = YES
CALLER_GRAPH           = YES
```

---

## REVIEW CHECKLISTA

### Code Review
- [ ] Alla publika API:er dokumenterade?
- [ ] Fil-header finns?
- [ ] Inline-kommentarer förklarar VARFÖR?
- [ ] Inga TODO utan issue-referens?
- [ ] @deprecated markering om deprecated?

### Documentation Review
- [ ] ADR skapad för arkitekturbeslut?
- [ ] API docs uppdaterade?
- [ ] README aktuell?
- [ ] CHANGELOG uppdaterad?
