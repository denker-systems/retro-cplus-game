# Input System Refactoring Checklist

> Centralisera all editor input i en EditorInputController

## Bakgrund

### Problem
- Input-logik är fragmenterad över 7+ filer
- Duplicerad kod för samma beteenden
- Inkonsekvent kontrollschema mellan vyer
- Svårt att underhålla och utöka
- Ingen central konfiguration för keybindings

### Mål
- **En** central plats för all editor input
- Konfigurerbar keybindings (JSON)
- Konsekvent UE-style navigation överallt
- Enkel att utöka med nya input-modes

---

## Nuvarande Input-källor

| Fil | Rader | Ansvar | Status |
|-----|-------|--------|--------|
| `EditorInputHandler.h/cpp` | ~150 | Globala shortcuts | ✅ Behåll |
| `ViewportInput.h/cpp` | ~120 | ViewportPanelNew | ⏳ Migrera |
| `ViewportPanel_Input.cpp` | ~300 | Legacy dragging | ⏳ Migrera |
| `Viewport3DPanel.cpp` | ~100 | UE-style 3D | ⏳ Migrera |
| `ViewportPanel_Scene.cpp` | ~50 | Actor selection | ⏳ Migrera |
| `ViewportPanel_Level.cpp` | ~30 | Scene dragging | ⏳ Migrera |
| `ViewportPanel_World.cpp` | ~30 | Level dragging | ⏳ Migrera |

---

## Phase 1: EditorInputController Grundstruktur

### Skapa Core Files
- [ ] Skapa `src/editor/input/EditorInputController.h`
- [ ] Skapa `src/editor/input/EditorInputController.cpp`
- [ ] Lägg till i CMakeLists.txt
- [ ] Grundläggande singleton/manager pattern

### Input Modes (Strategy Pattern)
- [ ] Skapa `IInputMode` interface
- [ ] Skapa `CameraInputMode` - kameranavigation
- [ ] Skapa `SelectionInputMode` - objektval
- [ ] Skapa `GizmoInputMode` - transform gizmos
- [ ] Skapa `DragDropInputMode` - drag & drop

### Input State
- [ ] Centraliserad mus-state (position, delta, knappar)
- [ ] Centraliserad tangentbords-state
- [ ] Modifier-state (Ctrl, Shift, Alt)

---

## Phase 2: Kamera Input

### UE-Style Navigation (standardisera)
- [ ] **RMB + WASD/QE** → Fly mode
- [ ] **RMB + Mouse** → Look around
- [ ] **Alt + LMB** → Orbit
- [ ] **Alt + RMB** → Dolly
- [ ] **Alt + MMB** → Pan
- [ ] **MMB** → Pan
- [ ] **Scroll** → Zoom
- [ ] **RMB + Scroll** → Justera hastighet
- [ ] **F** → Focus på selection

### Migrera från
- [ ] `Viewport3DPanel::handleInput()` → `CameraInputMode`
- [ ] `ViewportInput::process3DInput()` → `CameraInputMode`
- [ ] `ViewportInput::process2DInput()` → `CameraInputMode`

---

## Phase 3: Selection Input

### Objektval
- [ ] **LMB Click** → Select object
- [ ] **Ctrl + LMB** → Add to selection
- [ ] **Shift + LMB** → Toggle selection
- [ ] **LMB Drag** → Box select
- [ ] **Escape** → Deselect all
- [ ] **Double-click** → Drill down (World→Level→Scene)

### Migrera från
- [ ] `ViewportPanel_Scene.cpp` selection logic
- [ ] `Viewport3DPanel::handlePicking()`
- [ ] `ViewportInput` click detection

---

## Phase 4: Gizmo Input

### Transform Gizmos
- [ ] **W** → Translate mode
- [ ] **E** → Rotate mode
- [ ] **R** → Scale mode
- [ ] **Space** → Toggle Local/World
- [ ] **Ctrl held** → Snap enabled
- [ ] **Gizmo drag** → Transform object

### Migrera från
- [ ] `TransformGizmo3D` UI buttons
- [ ] Integrera med ImGuizmo

---

## Phase 5: Drag & Drop Input

### Hierarchy Dragging
- [ ] Drag actors mellan scenes
- [ ] Drag scenes mellan levels
- [ ] Drag levels i world
- [ ] Grid snapping

### Migrera från
- [ ] `ViewportPanel_World.cpp` level dragging
- [ ] `ViewportPanel_Level.cpp` scene dragging
- [ ] `ViewportPanel_Input.cpp` actor dragging

---

## Phase 6: Keybindings Config

### JSON Configuration
- [ ] Skapa `assets/data/keybindings.json`
- [ ] Ladda vid startup
- [ ] Runtime rebinding support
- [ ] Reset to defaults

### Format
```json
{
  "camera": {
    "fly_forward": "W",
    "fly_back": "S",
    "fly_left": "A",
    "fly_right": "D",
    "fly_up": "E",
    "fly_down": "Q",
    "focus": "F"
  },
  "gizmo": {
    "translate": "W",
    "rotate": "E",
    "scale": "R",
    "toggle_space": "Space"
  },
  "selection": {
    "delete": "Delete",
    "duplicate": "Ctrl+D",
    "deselect": "Escape"
  }
}
```

---

## Phase 7: Cleanup & Integration

### Ta bort duplicerad kod
- [ ] Ta bort `ViewportInput.h/cpp` (ersatt)
- [ ] Ta bort `ViewportPanel_Input.cpp` input-logik
- [ ] Rensa `Viewport3DPanel::handleInput()`
- [ ] Rensa inline input i ViewportPanel_*.cpp

### Integration
- [ ] `EditorState` skapar `EditorInputController`
- [ ] Alla panels använder `EditorInputController`
- [ ] Testa all funktionalitet

---

## Arkitektur

### Klassdiagram
```
EditorInputController (singleton)
├── InputState
│   ├── MouseState (pos, delta, buttons)
│   ├── KeyboardState (keys, modifiers)
│   └── ModifierState (ctrl, shift, alt)
│
├── IInputMode (interface)
│   ├── CameraInputMode
│   │   ├── FlyMode
│   │   ├── OrbitMode
│   │   └── PanMode
│   ├── SelectionInputMode
│   ├── GizmoInputMode
│   └── DragDropInputMode
│
├── KeybindingsConfig
│   └── loads from JSON
│
└── EditorInputHandler (existing - shortcuts)
```

### API
```cpp
class EditorInputController {
public:
    static EditorInputController& instance();
    
    // Per-frame update
    void processInput(const InputContext& ctx);
    
    // Mode management
    void pushMode(std::unique_ptr<IInputMode> mode);
    void popMode();
    IInputMode* getCurrentMode();
    
    // State queries
    const InputState& getState() const;
    bool isModifierDown(Modifier mod) const;
    
    // Keybindings
    void loadKeybindings(const std::string& path);
    void resetToDefaults();
};
```

---

## Testing Checklist

### Kamera
- [ ] RMB + WASD fungerar i alla vyer
- [ ] Alt + LMB orbit fungerar
- [ ] Scroll zoom fungerar
- [ ] F focus fungerar

### Selection
- [ ] Click select fungerar
- [ ] Multi-select med Ctrl
- [ ] Box select fungerar
- [ ] Escape deselect fungerar

### Gizmos
- [ ] W/E/R byter gizmo
- [ ] Gizmo manipulation fungerar
- [ ] Snap med Ctrl fungerar

### Drag & Drop
- [ ] Actor dragging fungerar
- [ ] Scene dragging fungerar
- [ ] Level dragging fungerar

---

## Prioritering

| Fas | Prioritet | Estimat |
|-----|-----------|---------|
| Phase 1 | 🔴 Hög | 1 session |
| Phase 2 | 🔴 Hög | 1 session |
| Phase 3 | 🟠 Medium | 1 session |
| Phase 4 | 🟠 Medium | 0.5 session |
| Phase 5 | 🟡 Låg | 1 session |
| Phase 6 | 🟡 Låg | 0.5 session |
| Phase 7 | 🟠 Medium | 1 session |

**Total: ~6 sessioner**

---

## Success Criteria

✅ All input hanteras via `EditorInputController`  
✅ Konsekvent UE-style navigation i alla vyer  
✅ Keybindings konfigurerbar via JSON  
✅ Ingen duplicerad input-kod  
✅ Enkel att lägga till nya input-modes  

---

*Last Updated: 2026-01-08*  
*Version: 1.0*
