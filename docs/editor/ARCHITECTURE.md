# Editor Architecture

## Översikt

RetroEditor är en ImGui-baserad editor för att skapa och redigera game content. Editorn använder en **panel-baserad arkitektur** där varje UI-komponent är en separat klass.

## Arkitektur

```
┌─────────────────────────────────────────────────────────┐
│                    EditorState                          │
│  - Huvudkoordinator                                     │
│  - Hanterar ImGui docking layout                        │
│  - Äger alla paneler                                    │
└─────────────────────────────────────────────────────────┘
                          │
                          ├─── EditorContext (delat state)
                          │
        ┌─────────────────┼─────────────────┐
        │                 │                 │
   ┌────▼────┐      ┌────▼────┐      ┌────▼────┐
   │ Panel 1 │      │ Panel 2 │      │ Panel 3 │
   └─────────┘      └─────────┘      └─────────┘
```

## Komponenter

### 1. EditorContext
**Fil:** `src/editor/EditorContext.h/cpp`

Delat state mellan alla paneler:
- **Selection state** - Valt rum, dialog, quest, item
- **Status meddelanden** - För Console-panelen
- **Clipboard** - För copy/paste
- **Texture cache** - För preview-bilder
- **Editor preferences** - Grid, snap, etc.

```cpp
class EditorContext {
    SelectionType selectedType;
    std::string selectedRoomId;
    std::string selectedDialogId;
    std::string statusMessage;
    // ...
};
```

### 2. IEditorPanel
**Fil:** `src/editor/IEditorPanel.h`

Base interface för alla paneler:

```cpp
class IEditorPanel {
    virtual const std::string& getId() const = 0;
    virtual const std::string& getTitle() const = 0;
    virtual void render() = 0;
    virtual void update(float deltaTime) {}
    
    bool isVisible() const;
    void setVisible(bool visible);
};
```

### 3. Paneler

#### HierarchyPanel
**Fil:** `src/editor/panels/HierarchyPanel.h/cpp`

Visar träd med alla game-objekt:
- Rooms (collapsible header)
- Dialogs
- Quests
- Items
- NPCs

**Features:**
- Click för att välja objekt
- Synkar selection med `EditorContext`
- Callbacks för selection events

#### PropertiesPanel
**Fil:** `src/editor/panels/PropertiesPanel.h/cpp`

Inspector för valt objekt:
- **Room properties** - ID, namn, walk area, hotspots, spawn
- **Dialog properties** - Nodes, speaker, text
- **Quest properties** - Objectives, rewards
- **Item properties** - Icon, combinable
- **Hotspot properties** - Position, size, target

**Features:**
- Dynamisk rendering baserat på `EditorContext.selectedType`
- "Edit Room" knapp för visuell redigering
- Collapsible sections för hotspots, exits

#### ViewportPanel
**Fil:** `src/editor/panels/ViewportPanel.h/cpp`

Visar rum-preview:
- **Toolbar** - Zoom, grid, hotspot/walk area toggles
- **Preview area** - Skalad rendering av rum
- **Overlays** - Walk area (cyan), hotspots (röd/gul), spawn (magenta)

**Features:**
- Zoom 25%-400%
- Grid overlay
- Hotspot highlighting vid selection
- Auto-laddar rum vid selection change

#### AssetBrowserPanel
**Fil:** `src/editor/panels/AssetBrowserPanel.h/cpp`

Browser för game assets:
- **Filter** - All, Sprites, Backgrounds, Sounds, Music, Fonts
- **Search** - Filtrera på namn
- **View modes** - Grid eller List
- **Metadata** - Filstorlek, path

**Features:**
- Skannar `assets/` mappar automatiskt
- Refresh-knapp
- Grid view med ikoner
- List view med tabell
- Tooltip med full info

#### ConsolePanel
**Fil:** `src/editor/panels/ConsolePanel.h/cpp`

Logg och status:
- **Log entries** - Info, Warning, Error med timestamps
- **Stats** - Antal rum, dialoger, quests, items, npcs
- **Filters** - Visa/dölj per log level
- **Auto-scroll** - Följer nya meddelanden

**API:**
```cpp
console->log("Message");
console->logWarning("Warning");
console->logError("Error");
console->clear();
```

## Docking Layout

Default layout (kan ändras av användaren):

```
┌─────────────────────────────────────────────────────┐
│              Menu Bar (File, Edit, View, Tools)     │
├──────────┬────────────────────────┬─────────────────┤
│          │                        │   Properties    │
│ Hierarchy│      Viewport          ├─────────────────┤
│          │                        │ Asset Browser   │
├──────────┴────────────────────────┴─────────────────┤
│                   Console                           │
└─────────────────────────────────────────────────────┘
```

**Proportioner:**
- Hierarchy: 20% bredd
- Viewport: Center (dynamisk)
- Properties + Asset Browser: 30% bredd (delad 50/50)
- Console: 25% höjd

## Dataflöde

### Selection Flow
```
User clicks i Hierarchy
    ↓
HierarchyPanel uppdaterar EditorContext.selectedRoomId
    ↓
PropertiesPanel läser EditorContext.selectedRoomId
    ↓
PropertiesPanel renderar room properties
    ↓
ViewportPanel detekterar ändring
    ↓
ViewportPanel laddar rum-preview
```

### Save Flow
```
User klickar "Save All" i File-menyn
    ↓
EditorContext.saveToFiles()
    ↓
Skriver JSON till assets/data/
    ↓
DataLoader.loadAll() (reload)
    ↓
ConsolePanel loggar "Saved all data"
```

## Meny-struktur

### File
- **Save All** (Ctrl+S) - Spara alla ändringar
- **Import from Tiled...** - Importera från Tiled
- **Export to Tiled...** - Exportera till Tiled
- **Exit** (ESC) - Stäng editor

### Edit
- **Undo** (Ctrl+Z) - Ångra (ej implementerad)
- **Redo** (Ctrl+Y) - Gör om (ej implementerad)

### View
- **Hierarchy** - Toggle Hierarchy panel
- **Viewport** - Toggle Viewport panel
- **Properties** - Toggle Properties panel
- **Asset Browser** - Toggle Asset Browser panel
- **Console** - Toggle Console panel
- **ImGui Demo** - Visa ImGui demo window

### Tools
- **Validate All Data** - Validera JSON (ej implementerad)
- **Reload Data** - Ladda om från disk
- **Refresh Assets** - Uppdatera asset-listan

## Framtida Utökningar

### Planerade Features
1. **Undo/Redo System** - Command pattern för ändringar
2. **Dialog Editor** - Visual node editor för dialogträd
3. **Quest Editor** - Flowchart för quest objectives
4. **Hot Reload** - Automatisk reload vid filändringar (efsw)
5. **Asset Preview** - Visa sprites/sounds i Asset Browser
6. **Validation** - JSON schema validation
7. **Search & Replace** - Global sökning i all data

### Nya Paneler
- **ScenePanel** - Visuell scene editor med drag-and-drop
- **AnimationPanel** - Sprite animation editor
- **AudioPanel** - Waveform viewer och playback
- **ScriptPanel** - Lua script editor (om scripting läggs till)

## Best Practices

### Skapa Ny Panel
1. Skapa `MyPanel.h/cpp` i `src/editor/panels/`
2. Ärv från `IEditorPanel`
3. Ta `EditorContext&` i konstruktor
4. Implementera `render()` metod
5. Lägg till i `CMakeLists.txt`
6. Skapa instans i `EditorState::enter()`
7. Rendera i `EditorState::renderImGui()`

### Panel Guidelines
- **Minimal state** - Använd `EditorContext` för delat state
- **Readonly data** - Läs från `DataLoader`, skriv via `EditorContext`
- **Visibility** - Respektera `m_visible` flag
- **Performance** - Cacha tunga beräkningar
- **Logging** - Använd `ConsolePanel` för feedback

## Tekniska Detaljer

### Dependencies
- **ImGui 1.91.9** (docking branch)
- **SDL2** - För rendering och input
- **nlohmann/json** - För data serialization

### Build
```powershell
cmake --build build --config Release --target RetroEditor
.\build\Release\RetroEditor.exe
```

### Debugging
- Aktivera ImGui Demo för att testa widgets
- Console-panelen visar alla log-meddelanden
- Använd `LOG_INFO/WARNING/ERROR` i kod

## Relaterad Dokumentation
- [Getting Started](../guides/getting-started.md)
- [Data Formats](../technical/data-formats.md)
- [ImGui Integration](IMGUI.md)
