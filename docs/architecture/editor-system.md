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

## Framtida Utökningar

- [ ] DialogPanel för dialogträd
- [ ] QuestPanel för quests
- [ ] ItemPanel för items
- [ ] NPCPanel för NPCs
- [ ] Copy/Paste mellan rum
- [ ] Multi-selection
- [ ] Snap to other hotspots
