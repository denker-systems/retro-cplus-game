# Retro Adventure - Dokumentationsindex

Komplett guide till all projektdokumentation.

---

## 📚 Snabbstart

| Dokument | Beskrivning |
|----------|-------------|
| [README.md](../README.md) | Projektöversikt, installation, kontroller |
| [Getting Started](guides/getting-started.md) | Steg-för-steg guide för att komma igång |
| [Complete Feature Guide](guides/complete-feature-guide.md) | Alla features i detalj |

---

## 🏗️ Arkitektur

| Dokument | Beskrivning |
|----------|-------------|
| [ARCHITECTURE.md](ARCHITECTURE.md) | Komplett systemarkitektur |
| [Overview](architecture/overview.md) | Systemöversikt |
| [States](architecture/states.md) | State Pattern implementation |
| [Entities](architecture/entities.md) | Entity hierarki |
| [Editor System](architecture/editor-system.md) | In-game editor arkitektur |

---

## 📖 API Documentation

### Core

| Klass | Beskrivning |
|-------|-------------|
| [Game](api/core/Game.md) | Huvudspelklass, SDL init, spelloop |

### Systems

| System | Beskrivning |
|--------|-------------|
| [DialogSystem](api/systems/DialogSystem.md) | Dialogträd och konversationer |
| [InventorySystem](api/systems/InventorySystem.md) | Items, kombination, användning |
| [QuestSystem](api/systems/QuestSystem.md) | Quests och objectives |
| [AISystem](api/systems/AISystem.md) | NPC AI och beteenden |
| [SaveSystem](api/systems/SaveSystem.md) | Save/Load hantering |

### States

| State | Beskrivning |
|-------|-------------|
| MenuState | Huvudmeny |
| PlayState | Gameplay |
| PauseState | Pausmeny (overlay) |
| DialogState | Dialog (overlay) |
| InventoryState | Inventory (overlay) |
| QuestLogState | Quest log (overlay) |
| OptionsState | Inställningar |
| EditorState | In-game editor |
| SaveLoadState | Save/Load meny |

### Graphics

| Klass | Beskrivning |
|-------|-------------|
| TextureManager | Texture caching |
| SpriteSheet | Frame-baserad rendering |
| Animation | Tidsbaserad animation |
| FontManager | Text rendering med SDL_ttf |
| Transition | Fade effects |

### Entities

| Klass | Beskrivning |
|-------|-------------|
| Entity | Abstract base class |
| Character | Character base |
| PlayerCharacter | Spelaren |
| NPC | Non-player characters |

---

## 🛠️ Technical

| Dokument | Beskrivning |
|----------|-------------|
| [Build Instructions](technical/build.md) | CMake, vcpkg, kompilering |
| [Data Formats](technical/data-formats.md) | JSON schemas för alla data |
| [Dependencies](technical/dependencies.md) | SDL2, vcpkg, externa bibliotek |

---

## 📝 Guides

| Guide | Beskrivning |
|-------|-------------|
| [Getting Started](guides/getting-started.md) | Kom igång med projektet |
| [Complete Feature Guide](guides/complete-feature-guide.md) | Alla features i detalj |
| [Tiled Workflow](guides/tiled-workflow.md) | Arbeta med Tiled Map Editor |
| [Tiled Multi-Layer](guides/tiled-multilayer.md) | Multi-layer backgrounds |

---

## 🎮 Design

| Dokument | Beskrivning |
|----------|-------------|
| [LucasArts Roadmap](design/LUCASARTS-ROADMAP.md) | LucasArts-inspirerad design |
| [LucasArts Insights](design/lucasarts-insights-applied.md) | Tillämpade insights |

---

## 🗺️ Roadmap

| Dokument | Beskrivning |
|----------|-------------|
| [ROADMAP.md](ROADMAP.md) | Utvecklingsplan och milestones |

---

## 📊 Sessions

Utvecklingssessioner dokumenteras här:

| Session | Datum | Fokus |
|---------|-------|-------|
| [2026-01-04](sessions/2026-01-04.md) | 2026-01-04 | Editor system, hotspot editing |
| [2026-01-05](sessions/2026-01-05.md) | 2026-01-05 | Depth scaling, walk area editing |

---

## 🔍 Sök efter Feature

### Dialog & Conversations
- [DialogSystem API](api/systems/DialogSystem.md)
- [DialogState](api/states/DialogState.md)
- [Dialog JSON Format](technical/data-formats.md#dialogs)

### Inventory & Items
- [InventorySystem API](api/systems/InventorySystem.md)
- [InventoryState](api/states/InventoryState.md)
- [Item JSON Format](technical/data-formats.md#items)

### Quests & Objectives
- [QuestSystem API](api/systems/QuestSystem.md)
- [QuestLogState](api/states/QuestLogState.md)
- [Quest JSON Format](technical/data-formats.md#quests)

### NPC & AI
- [AISystem API](api/systems/AISystem.md)
- [NPC Entity](api/entities/NPC.md)
- [NPC JSON Format](technical/data-formats.md#npcs)

### Rooms & World
- [Room Class](api/core/Room.md)
- [RoomManager](api/systems/RoomManager.md)
- [Room JSON Format](technical/data-formats.md#rooms)

### Save & Load
- [SaveSystem API](api/systems/SaveSystem.md)
- [SaveLoadState](api/states/SaveLoadState.md)
- [Save JSON Format](technical/data-formats.md#save-files)

### Audio & Music
- [AudioManager](api/audio/AudioManager.md)
- [Audio Features](guides/complete-feature-guide.md#audio-system)

### Editor
- [Editor System Architecture](architecture/editor-system.md)
- [EditorState](api/states/EditorState.md)
- [Editor Features](guides/complete-feature-guide.md#editor-system)

---

## 🚀 Quick Links

### För Utvecklare
1. [Build Instructions](technical/build.md) - Kompilera projektet
2. [Architecture](ARCHITECTURE.md) - Förstå systemet
3. [Data Formats](technical/data-formats.md) - JSON schemas

### För Content Creators
1. [Room JSON Format](technical/data-formats.md#rooms) - Skapa rum
2. [Dialog JSON Format](technical/data-formats.md#dialogs) - Skriva dialoger
3. [Quest JSON Format](technical/data-formats.md#quests) - Designa quests

### För Designers
1. [Complete Feature Guide](guides/complete-feature-guide.md) - Alla features
2. [LucasArts Insights](design/lucasarts-insights-applied.md) - Design philosophy
3. [Editor System](architecture/editor-system.md) - Visuell redigering

---

## 📋 Checklista för Nya Features

När du implementerar en ny feature:

- [ ] Uppdatera [ARCHITECTURE.md](ARCHITECTURE.md)
- [ ] Skapa API-dokumentation i `docs/api/`
- [ ] Lägg till i [Complete Feature Guide](guides/complete-feature-guide.md)
- [ ] Uppdatera [Data Formats](technical/data-formats.md) om JSON ändras
- [ ] Markera som klar i [ROADMAP.md](ROADMAP.md)
- [ ] Dokumentera i session-fil `docs/sessions/YYYY-MM-DD.md`

---

## 🤝 Bidra

För att bidra till dokumentationen:

1. Följ befintlig struktur och format
2. Använd Markdown med kod-exempel
3. Inkludera både teori och praktiska exempel
4. Länka till relaterade dokument
5. Uppdatera detta index

---

## 📞 Support

**Problem med dokumentationen?**
- Skapa en issue på GitHub
- Kontrollera [Troubleshooting](guides/complete-feature-guide.md#troubleshooting)

**Saknas något?**
- Kolla [ROADMAP.md](ROADMAP.md) för planerade features
- Föreslå förbättringar via pull request

---

**Senast uppdaterad:** 2026-01-05
