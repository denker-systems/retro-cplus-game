# Editor System Architecture

## Översikt

Editor-systemet är ett modulärt, skalbart system för att redigera spelinnehåll visuellt.

## Folder-struktur

```
src/editor/
├── EditorCore.h/cpp        # Central koordinator
├── EditorContext.h/cpp     # Delad editor-state
├── IEditorCommand.h        # Command interface (undo/redo)
├── IEditorPanel.h          # Panel interface
│
├── commands/               # Undoable commands
│   └── HotspotCommands.h/cpp
│
├── panels/                 # Editor panels
│   └── RoomPanel.h/cpp
│
└── widgets/                # UI widgets
    ├── Widget.h/cpp        # Base widget
    ├── Button.h/cpp
    ├── TextField.h/cpp
    └── ListView.h/cpp
```

## Design Patterns

### 1. Command Pattern (Undo/Redo)
Varje användarhandling kapslas i ett `IEditorCommand`:

```cpp
class MoveHotspotCommand : public IEditorCommand {
    bool execute() override;  // Utför
    bool undo() override;     // Ångra
};
```

### 2. Strategy Pattern (Panels)
Olika redigeringslägen implementeras som `IEditorPanel`:

```cpp
class RoomPanel : public IEditorPanel {
    void render(SDL_Renderer*) override;
    bool handleEvent(const SDL_Event&) override;
};
```

### 3. Observer Pattern (Events)
Panels kommunicerar via event-system:

```cpp
EditorCore::instance().subscribe(EditorEvent::SelectionChanged, "RoomPanel", callback);
EditorCore::instance().dispatch({EditorEvent::DataModified, "source", "payload"});
```

## Komponenter

### EditorCore
- Singleton som koordinerar allt
- Hanterar panel-registrering och switching
- Command history (undo/redo stack)
- Event dispatch

### EditorContext
- Delad state för alla panels
- Mutable kopior av speldata
- Texture cache
- Selection state
- Clipboard

### Widget System
- Återanvändbara UI-komponenter
- Event-driven (click, hover, focus)
- Composable (containers)

## Användning

```cpp
// Registrera panel
EditorCore::instance().registerPanel("rooms", std::make_unique<RoomPanel>());

// Utför command (med undo)
auto cmd = std::make_unique<MoveHotspotCommand>(...);
EditorCore::instance().executeCommand(std::move(cmd));

// Undo/Redo
EditorCore::instance().undo();
EditorCore::instance().redo();

// Spara
EditorCore::instance().getContext().saveToFiles();
```

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+Z | Undo |
| Ctrl+Y / Ctrl+Shift+Z | Redo |
| Ctrl+S | Save |
| V | Select tool |
| R | Resize tool |
| C | Create tool |
| DEL | Delete selected |
| + | Öka bottom scale (depth) |
| - | Minska bottom scale |
| Shift++ | Öka top scale |
| Shift+- | Minska top scale |

## Walk Area Editing

Walk area kan redigeras visuellt i editorn:

```
┌─────────────────────────────────────┐
│         [Top Handle]                │  ← Dra för att ändra minY
├─[L]─────────────────────────────[R]─┤  ← Sidohandtag
│                                     │
│         Walk Area                   │
│                                     │
├─────────────────────────────────────┤
│         [Bottom Handle]             │  ← Dra för att ändra maxY
└─────────────────────────────────────┘
```

### Depth Scaling

Perspektiv-skalning gör att karaktärer ser mindre ut längre bort:

- **scaleTop**: Storlek vid walk area topp (t.ex. 0.5 = 50%)
- **scaleBottom**: Storlek vid walk area botten (t.ex. 1.0 = 100%)

Visuell indikator visar två figurer med olika storlekar.

## EditorState vs Editor Panels

- **EditorState** (`src/states/EditorState.cpp`): Legacy fullskärmseditor
- **Editor Panels** (`src/editor/panels/`): Modulära, utbyggbara panels

Båda system fungerar parallellt. EditorState hanterar visuell redigering, 
medan Editor Panels används för mer komplexa operationer med undo/redo.

## Framtida Utökningar

- [ ] DialogPanel för dialogträd
- [ ] QuestPanel för quests
- [ ] ItemPanel för items
- [ ] NPCPanel för NPCs
- [ ] Copy/Paste mellan rum
- [ ] Multi-selection
- [ ] Snap to other hotspots
- [x] Walk area visual editing
- [x] Depth scaling editing
