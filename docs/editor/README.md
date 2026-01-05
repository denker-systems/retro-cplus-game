# RetroEditor Documentation

## Översikt

RetroEditor är en ImGui-baserad editor för att skapa och redigera game content för Retro Adventure. Editorn använder en modern panel-baserad arkitektur med docking support.

## Snabbstart

### Bygga Editorn

```powershell
cd c:\Users\Calle\Documents\GitHub\retro-cplus-game
cmake --build build --config Release --target RetroEditor
```

### Köra Editorn

```powershell
.\build\Release\RetroEditor.exe
```

## Funktioner

### Paneler

- **Hierarchy** - Träd med rum, dialoger, quests, items
- **Viewport** - Visuell preview av rum med hotspots och walk areas
- **Properties** - Inspector för valt objekt
- **Asset Browser** - Browser för sprites, sounds, backgrounds
- **Console** - Logg och status information

### Menyer

#### File
- **Save All** (Ctrl+S) - Spara alla ändringar till JSON
- **Import from Tiled** - Importera rum från Tiled Map Editor
- **Export to Tiled** - Exportera rum till Tiled
- **Exit** (ESC) - Stäng editor

#### Edit
- **Undo** (Ctrl+Z) - Ångra (planerad)
- **Redo** (Ctrl+Y) - Gör om (planerad)

#### View
- Toggle synlighet för alla paneler
- ImGui Demo window för testing

#### Tools
- **Validate All Data** - Validera JSON (planerad)
- **Reload Data** - Ladda om från disk
- **Refresh Assets** - Uppdatera asset-listan

## Workflow

### Redigera Rum

1. **Välj rum** i Hierarchy-panelen
2. **Inspektera** properties i Properties-panelen
3. **Klicka "Edit Room"** för visuell redigering
4. **Dra hotspots** och walk area i Viewport
5. **Spara** med File → Save All

### Bläddra Assets

1. Öppna **Asset Browser**-panelen
2. **Filtrera** per typ (Sprites, Backgrounds, etc.)
3. **Sök** efter specifika filer
4. **Toggle** Grid/List view
5. **Click** för att välja asset

### Loggar och Status

- **Console-panelen** visar alla meddelanden
- **Filter** per log level (Info, Warning, Error)
- **Auto-scroll** följer nya meddelanden
- **Clear** för att rensa loggen

## Docking Layout

Editorn använder ImGui docking för flexibel layout:

- **Dra tabs** för att flytta paneler
- **Dra till kanter** för att docka
- **Dra till center** för att tabba
- **Layout sparas** automatiskt

### Default Layout

```
┌─────────────────────────────────────────────────────┐
│              Menu Bar                               │
├──────────┬────────────────────────┬─────────────────┤
│          │                        │   Properties    │
│ Hierarchy│      Viewport          ├─────────────────┤
│          │                        │ Asset Browser   │
├──────────┴────────────────────────┴─────────────────┤
│                   Console                           │
└─────────────────────────────────────────────────────┘
```

## Teknisk Information

### Arkitektur

Se [ARCHITECTURE.md](ARCHITECTURE.md) för detaljerad arkitektur-dokumentation.

**Huvudkomponenter:**
- `EditorState` - Huvudkoordinator
- `EditorContext` - Delat state mellan paneler
- `IEditorPanel` - Base interface för paneler
- Panel-klasser - HierarchyPanel, PropertiesPanel, etc.

### Paneler

Se [PANELS.md](PANELS.md) för guide om hur man skapar och använder paneler.

**Befintliga paneler:**
- HierarchyPanel
- PropertiesPanel
- ViewportPanel
- AssetBrowserPanel
- ConsolePanel

### Data Format

Editorn arbetar med JSON-filer i `assets/data/`:
- `rooms.json` - Rum, hotspots, walk areas
- `dialogs.json` - Dialogträd
- `quests.json` - Quests och objectives
- `items.json` - Items och kombinationer
- `npcs.json` - NPCs och beteenden

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+S | Save All |
| ESC | Exit Editor |
| Ctrl+Z | Undo (planerad) |
| Ctrl+Y | Redo (planerad) |

## Felsökning

### Editor startar inte
- Kontrollera att `assets/data/` finns
- Verifiera att JSON-filerna är valid
- Kolla Console-panelen för felmeddelanden

### Paneler syns inte
- Öppna View-menyn och aktivera panelen
- Kontrollera att panelen inte är dockad utanför skärmen
- Återställ layout genom att stänga och öppna editorn

### Data sparas inte
- Kontrollera att du har skrivbehörighet till `assets/data/`
- Verifiera att File → Save All körs
- Kolla Console-panelen för fel

### Assets visas inte
- Klicka Tools → Refresh Assets
- Kontrollera att filerna finns i `assets/` mapparna
- Verifiera filformat (PNG för bilder, WAV för ljud, etc.)

## Framtida Features

### Planerade Förbättringar
- **Undo/Redo System** - Command pattern för ändringar
- **Dialog Node Editor** - Visual editor för dialogträd
- **Quest Flow Editor** - Flowchart för quest objectives
- **Hot Reload** - Automatisk reload vid filändringar
- **Asset Preview** - Visa sprites och spela ljud
- **JSON Validation** - Schema-baserad validering
- **Search & Replace** - Global sökning i all data

### Nya Paneler
- **ScenePanel** - Drag-and-drop scene editor
- **AnimationPanel** - Sprite animation timeline
- **AudioPanel** - Waveform viewer och playback
- **ScriptPanel** - Lua script editor (om scripting läggs till)

## Relaterad Dokumentation

- [Architecture](ARCHITECTURE.md) - Teknisk arkitektur
- [Panels](PANELS.md) - Panel-guide
- [Data Formats](../technical/data-formats.md) - JSON-scheman
- [Getting Started](../guides/getting-started.md) - Kom igång guide

## Support

För frågor och buggrapporter, se projektets GitHub repository.
