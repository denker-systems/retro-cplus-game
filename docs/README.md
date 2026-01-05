# Retro Adventure - Documentation

Komplett dokumentation för spelet och editorn.

---

##  Utveckling

| Dokument | Beskrivning |
|----------|-------------|
| [CHANGELOG.md](CHANGELOG.md) | Versionerade releases |
| [ROADMAP.md](ROADMAP.md) | Utvecklingsplan och progress |
| [ARCHITECTURE.md](ARCHITECTURE.md) | Teknisk arkitektur |

### Dev Logs
- [Sessions](dev/sessions/) - Dagliga session-rapporter
- [Decisions](dev/decisions/) - Arkitekturbeslut (ADR)
- [Notes](dev/notes/) - Fria anteckningar

---

##  Spel

### System-dokumentation
| System | Dokument |
|--------|----------|
| Room System | [room-system.md](game/systems/room-system.md) |
| Dialog System | [dialog-system.md](game/systems/dialog-system.md) |
| Inventory System | [inventory-system.md](game/systems/inventory-system.md) |
| Quest System | [quest-system.md](game/systems/quest-system.md) |
| AI System | [ai-system.md](game/systems/ai-system.md) |
| Save System | [save-system.md](game/systems/save-system.md) |

### Entities
- [Entity Hierarchy](game/entities/entity-hierarchy.md)

### Data
- [JSON Schemas](game/data/json-schemas.md)

---

##  Editor

| Dokument | Beskrivning |
|----------|-------------|
| [README.md](editor/README.md) | Användarguide |
| [ARCHITECTURE.md](editor/ARCHITECTURE.md) | Teknisk arkitektur |
| [PANELS.md](editor/PANELS.md) | Panel-guide |

---

##  API Reference

- [Core](api/core/) - Game, Input, Renderer
- [Systems](api/systems/) - DialogSystem, QuestSystem, etc.
- [Editor](api/editor/) - IEditorPanel, EditorContext

---

##  Guides

| Guide | Beskrivning |
|-------|-------------|
| [Getting Started](guides/getting-started.md) | Snabbstart |
| [Building](guides/building.md) | Bygginstruktioner |
| [Tiled Workflow](guides/tiled-workflow.md) | Tiled integration |

---

## Quick Links

- **Senaste session:** [dev/sessions/](dev/sessions/)
- **Roadmap progress:** [ROADMAP.md](ROADMAP.md)
- **Editor guide:** [editor/README.md](editor/README.md)
