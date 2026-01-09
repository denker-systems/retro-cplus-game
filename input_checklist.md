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

## Phase 1: EditorInputController Grundstruktur ✅ KLAR

### Skapa Core Files
- [x] Skapa `src/editor/input/EditorInputController.h`
- [x] Skapa `src/editor/input/EditorInputController.cpp`
- [x] Lägg till i CMakeLists.txt
- [x] Grundläggande singleton/manager pattern

### Input Modes (Strategy Pattern)
- [x] Skapa `IInputMode` interface
- [x] Skapa `CameraInputMode` - kameranavigation
- [x] Skapa `SelectionInputMode` - objektval
- [x] Skapa `GizmoInputMode` - transform gizmos
- [ ] Skapa `DragDropInputMode` - drag & drop (framtida)

### Input State
- [x] Centraliserad mus-state (position, delta, knappar)
- [x] Centraliserad tangentbords-state
- [x] Modifier-state (Ctrl, Shift, Alt)

---

## Phase 2: Kamera Input ✅ KLAR

### UE-Style Navigation (standardisera)
- [x] **RMB + WASD/QE** → Fly mode
- [x] **RMB + Mouse** → Look around
- [x] **Alt + LMB** → Orbit
- [x] **Alt + RMB** → Dolly
- [x] **Alt + MMB** → Pan
- [x] **MMB** → Pan
- [x] **Scroll** → Zoom
- [x] **RMB + Scroll** → Justera hastighet
- [x] **F** → Focus på selection

### Migrera från
- [x] `Viewport3DPanel::handleInput()` → `CameraInputMode`
- [x] `ViewportInput::process3DInput()` → `CameraInputMode`
- [x] `ViewportInput::process2DInput()` → `CameraInputMode`

---

## Phase 3: Selection Input ✅ KLAR

### Objektval
- [x] **LMB Click** → Select object
- [x] **Ctrl + LMB** → Add to selection
- [x] **Shift + LMB** → Toggle selection
- [x] **LMB Drag** → Box select (grundstruktur)
- [x] **Escape** → Deselect all
- [x] **Double-click** → Drill down (World→Level→Scene)

### Migrera från
- [x] `ViewportPanel_Scene.cpp` selection logic
- [x] `Viewport3DPanel::handlePicking()`
- [x] `ViewportInput` click detection

---

## Phase 4: Gizmo Input ✅ KLAR

### Transform Gizmos
- [x] **W** → Translate mode
- [x] **E** → Rotate mode
- [x] **R** → Scale mode
- [x] **Space** → Toggle Local/World
- [x] **Ctrl held** → Snap enabled
- [ ] **Gizmo drag** → Transform object (kräver ImGuizmo integration)

### Migrera från
- [ ] `TransformGizmo3D` UI buttons (framtida)
- [ ] Integrera med ImGuizmo (framtida)

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

## Phase 6: Keybindings Config ✅ KLAR

### JSON Configuration
- [x] Skapa `assets/data/keybindings.json`
- [x] Ladda vid startup
- [x] Runtime rebinding support
- [x] Reset to defaults

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

## Phase 7: Cleanup & Integration ✅ KLAR

### Ta bort duplicerad kod
- [x] Ta bort `ViewportInput.h/cpp` (ersatt)
- [ ] Ta bort `ViewportPanel_Input.cpp` input-logik (legacy behålls för nu)
- [ ] Rensa `Viewport3DPanel::handleInput()` (framtida)
- [ ] Rensa inline input i ViewportPanel_*.cpp (framtida)

### Integration
- [x] `ViewportPanelNew` använder `EditorInputController`
- [x] Keybindings laddas vid startup
- [x] Alla modes integrerade och aktiva

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
