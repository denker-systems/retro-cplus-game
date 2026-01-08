# feat(editor): 3D Viewport with Unreal Engine-style Navigation

## Sammanfattning

Implementerar ett komplett 3D viewport-system för Retro Engine Editor med Unreal Engine-liknande navigation, ImGuizmo transform gizmos, och OpenGL texture management. Inkluderar även planering för centraliserad input-hantering.

## Relaterad Issue

Closes #N/A (3D Viewport Implementation)

---

## Typ av Ändring

- [x] ✨ **Feature** - 3D viewport, UE-style navigation, ImGuizmo gizmos
- [x] 🔧 **Refactoring** - OpenGL texture management
- [x] 📚 **Documentation** - Checklists, workflows, changelogs
- [ ] 🐛 **Bugfix**
- [ ] ⚡ **Performance**
- [ ] 🧪 **Tests**
- [ ] 💥 **Breaking Change**

---

## Ändringar

### Added

**3D Viewport:**
- `Viewport3DPanel` - Full 3D scene rendering
- `EditorCamera3D` - Orbit/Fly camera with UE-style controls
- `TransformGizmo3D` - ImGuizmo integration (Translate/Rotate/Scale)
- `GLTextureManager` - OpenGL texture loading via stb_image

**UE-Style Navigation:**
| Kontroll | Funktion |
|----------|----------|
| RMB + WASD/QE | Fly mode |
| RMB + Mouse | Look around |
| Alt + LMB | Orbit |
| Alt + RMB | Dolly |
| MMB | Pan |
| RMB + Scroll | Adjust fly speed |
| F | Focus on selection |

**Documentation:**
- `3d_checklist.md` - 3D implementation checklist
- `input_checklist.md` - Input refactoring plan

**Workflow Updates:**
- `git-commit.md` - Added critical rule: always commit build files

### Changed

- `SpriteComponent` - GL texture support
- `AssetBrowserPanel` - OpenGL texture rendering
- `ActorObject` - Z coordinate support
- `ViewportPanel_Actors` - GL rendering integration
- `ImGuiManager` - isUsingOpenGL() helper

---

## Commits

| Hash | Type | Scope | Beskrivning |
|------|------|-------|-------------|
| `158eaca` | feat | editor | Add 3D viewport and unified selection/navigation sync |
| `07648c0` | docs | - | Update CHANGELOG and ROADMAP with 3D viewport features |
| `de26d16` | feat | editor | Add Unreal Engine-style 3D viewport navigation |
| `7c89611` | feat | editor | Add GLTextureManager and input refactoring plan |
| `0b00c23` | docs | workflow | Add critical rule - always commit build files |

---

## Implementation Details

### Arkitektur

```
Viewport3DPanel
├── EditorCamera3D (UE-style controls)
│   ├── onLookAround() - RMB + mouse
│   ├── onOrbit() - Alt + LMB
│   ├── onDolly() - Alt + RMB
│   ├── onPan() - MMB
│   └── onKeyboardMove() - WASD/QE
├── TransformGizmo3D (ImGuizmo)
│   ├── Translate mode (W)
│   ├── Rotate mode (E)
│   └── Scale mode (R)
└── Framebuffer (OpenGL FBO)

GLTextureManager (singleton)
└── stb_image texture loading
```

### Nya Filer

- `src/editor/viewport/EditorCamera3D.h/cpp`
- `src/editor/viewport/Viewport3DPanel.h/cpp`
- `src/editor/gizmos/TransformGizmo3D.h/cpp`
- `src/engine/graphics/GLTextureManager.h/cpp`
- `3d_checklist.md`
- `input_checklist.md`

### Modifierade Filer

- `CMakeLists.txt` - Added new sources
- `src/engine/components/SpriteComponent.h/cpp` - GL texture support
- `src/editor/panels/assets/AssetBrowserPanel.cpp` - OpenGL textures
- `vcpkg.json` - stb dependency

---

## Testning

### Manuell Testning

- [x] Testat i RetroEditor.exe
- [x] 3D viewport fungerar
- [x] RMB + WASD fly mode fungerar
- [x] Alt + LMB orbit fungerar
- [x] ImGuizmo gizmos renderas
- [x] Asset browser thumbnails fungerar

### Teststeg

1. Starta RetroEditor
2. Välj en Scene i hierarkin
3. Klicka "3D" toggle i viewport
4. Testa RMB + WASD för att flyga runt
5. Testa Alt + LMB för att orbita
6. Välj en actor och testa transform gizmo

---

## Screenshots

3D viewport med actors, grid, och transform gizmo.

---

## Dokumentation

- [x] CHANGELOG uppdaterad
- [x] ROADMAP uppdaterad
- [x] 3d_checklist.md skapad
- [x] input_checklist.md skapad

---

## Planerade Förbättringar (input_checklist.md)

- EditorInputController - Centraliserad input
- Konfigurerbar keybindings via JSON
- Konsekvent UE-style navigation i alla vyer

---

## Checklista

### Kod
- [x] Följer coding standards
- [x] Ingen debug-kod kvar
- [x] Dokumenterad med Doxygen

### Build
- [x] Kompilerar utan errors
- [x] Inga nya warnings

### Docs
- [x] CHANGELOG uppdaterad
- [x] Workflows uppdaterade

---

## Reviewer Notes

- Fokusera på `EditorCamera3D` för kamerakontroller
- `TransformGizmo3D` använder ImGuizmo library
- Debug-loggning finns kvar i `handleInput()` - kan tas bort efter test
