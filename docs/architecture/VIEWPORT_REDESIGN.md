# Viewport Arkitektur Redesign

## 1. NUVARANDE TILLSTÅND (KAOS)

### Legacy Split-filer (panels/viewport/) - ~2200 rader
| Fil | Rader | Funktionalitet |
|-----|-------|----------------|
| ViewportPanel.h | 191 | Monolitisk header med ALLT |
| ViewportPanel_Core.cpp | 308 | konstruktor, render(), toolbar, breadcrumbs |
| ViewportPanel_World.cpp | 334 | renderWorldView(), renderWorldSpatialView() |
| ViewportPanel_Level.cpp | 385 | renderLevelView(), renderSpatialView() |
| ViewportPanel_Scene.cpp | 465 | renderSceneView(), physics debug |
| ViewportPanel_Actors.cpp | 325 | renderSceneActors() |
| ViewportPanel_Input.cpp | 273 | handleMouseDown(), handleMouseDrag() |

### Ny OOP-arkitektur (viewport/) - ~2800 rader
| Fil | Rader | Funktionalitet |
|-----|-------|----------------|
| IViewportRenderer.h | 106 | Interface för renderers |
| ViewportRenderer.h/cpp | 755 | Unified 2D/3D renderer |
| ViewportWorldRenderer.h/cpp | 309 | World-specifik rendering |
| ViewportLevelRenderer.h/cpp | 320 | Level-specifik rendering |
| ViewportSceneRenderer.h/cpp | 547 | Scene-specifik rendering |
| ViewportInput.h/cpp | 173 | Input handling |
| EditorCamera3D.h/cpp | 311 | 3D kamera |
| Viewport3DPanel.h/cpp | 1034 | 3D viewport panel |
| ViewportPanel.h/cpp | 302 | Unified panel (ex ViewportPanelNew) |

---

## 2. PROBLEM

1. **Dubbel kompilering** - Båda systemen kompileras och tar plats
2. **Split-filer** - ViewportPanel_*.cpp är anti-pattern (en klass = en fil)
3. **Förvirring** - Två ViewportPanel.h i olika mappar
4. **Inkomplett migration** - Legacy används fortfarande av editor

---

## 3. FINAL ARKITEKTUR

### Hierarki
```
editor/viewport/
├── ViewportPanel.h/cpp          # Main panel (IEditorPanel)
├── IViewportRenderer.h          # Renderer interface
├── ViewportRenderer.h/cpp       # Base renderer implementation
│   ├── render2D() / render3D()
│   └── selection, picking, grid
├── ViewportWorldRenderer.h/cpp  # World-level rendering
├── ViewportLevelRenderer.h/cpp  # Level-level rendering  
├── ViewportSceneRenderer.h/cpp  # Scene-level rendering
├── ViewportInput.h/cpp          # Input handling
├── EditorCamera3D.h/cpp         # 3D camera
└── Viewport3DPanel.h/cpp        # Standalone 3D panel
```

### Klassdiagram
```
IEditorPanel
    │
    └── ViewportPanel
            │
            ├── ViewportRenderer (composition)
            │       │
            │       └── IViewportRenderer* (strategy pattern)
            │               ├── ViewportWorldRenderer
            │               ├── ViewportLevelRenderer
            │               └── ViewportSceneRenderer
            │
            ├── ViewportInput (composition)
            │
            └── EditorCamera3D (for 3D mode)
```

### Ansvar

**ViewportPanel** (IEditorPanel implementation)
- render() - Main render loop
- update() - Frame update
- renderToolbar() - 2D/3D toggle, zoom, grid toggle
- renderBreadcrumbs() - World > Level > Scene navigation
- setWorld/Level/Scene() - Navigation methods

**ViewportRenderer** (Unified renderer)
- 2D rendering (ImGui DrawList)
- 3D rendering (OpenGL + Framebuffer)
- Selection highlighting
- Grid overlay
- Object picking

**IViewportRenderer** (Interface)
- render(drawList, context)
- onMouseClick/Drag/Release()
- onDoubleClick() - navigation
- onScroll() - zoom
- getSelectedItem()

**ViewportWorldRenderer**
- Render levels as cards/boxes
- Level selection
- Level drag for grid position

**ViewportLevelRenderer**
- Render scenes as cards/boxes
- Scene selection
- Scene drag for grid position

**ViewportSceneRenderer**
- Render actors
- Actor selection/drag
- Physics debug visualization
- Walk area rendering

---

## 4. MIGRATION PLAN

### Steg 1: Säkerställ ny arkitektur fungerar
- [ ] ViewportPanel (ny) måste implementera IEditorPanel
- [ ] ViewportRenderer måste hantera alla hierarkinivåer
- [ ] Alla renderers måste implementera IViewportRenderer

### Steg 2: Migrera funktionalitet från legacy
- [ ] Toolbar → ViewportPanel::renderToolbar()
- [ ] Breadcrumbs → ViewportPanel::renderBreadcrumbs()
- [ ] World rendering → ViewportWorldRenderer
- [ ] Level rendering → ViewportLevelRenderer
- [ ] Scene rendering → ViewportSceneRenderer
- [ ] Actor rendering → ViewportSceneRenderer
- [ ] Input handling → ViewportInput
- [ ] Play mode controls → ViewportPanel

### Steg 3: Uppdatera beroenden
- [ ] EditorPanelManager → använd nya ViewportPanel
- [ ] EditorState → använd nya ViewportPanel
- [ ] EditorInputHandler → använd nya ViewportPanel
- [ ] Ta bort ViewportPanelNew referens

### Steg 4: Ta bort legacy
- [ ] Ta bort panels/viewport/ mappen helt
- [ ] Uppdatera CMakeLists.txt
- [ ] Verifiera build

---

## 5. IMPLEMENTATION STATUS

| Komponent | Legacy | Ny OOP | Migrera |
|-----------|--------|--------|---------|
| Panel UI | ✓ | Partial | Behövs |
| Toolbar | ✓ | Partial | Behövs |
| Breadcrumbs | ✓ | ✓ | Klar |
| World 2D | ✓ | ✓ | Klar |
| World 3D | ✓ | ✓ | Klar |
| Level 2D | ✓ | ✓ | Klar |
| Level 3D | ✓ | ✓ | Klar |
| Scene 2D | ✓ | ✓ | Behövs |
| Scene actors | ✓ | Partial | Behövs |
| Physics debug | ✓ | ✗ | Behövs |
| Play mode | ✓ | ✗ | Behövs |
| Input | ✓ | ✓ | Klar |
| Selection | ✓ | ✓ | Klar |
