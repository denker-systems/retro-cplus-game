# DEVLOG - Retro Adventure

Kronologisk logg av alla ändringar. Uppdateras vid varje commit.

Format: [DATUM] [HASH] TYPE(SCOPE): Beskrivning

---

## 2026-01-05

### Kväll
- `9f8cb09` docs: add robust documentation system with CHANGELOG, ADR, and restructured folders
  - Add CHANGELOG.md following Keep a Changelog standard
  - Move sessions to docs/dev/sessions/
  - Add ADR (Architecture Decision Records) in docs/dev/decisions/
  - Add docs/dev/DEVLOG.md for commit tracking
  - Add git-policy rules and generate-pr workflow
  - Session: [2026-01-05](sessions/2026-01-05.md)

### Eftermiddag
- `401647c` feat(editor): ImGui panel-based editor architecture
  - Dear ImGui integration med docking support (vcpkg)
  - 5 nya paneler: Hierarchy, Properties, Viewport, AssetBrowser, Console
  - EditorContext för shared panel state
  - Windowed mode för editor (1600x900 max)
  - Komplett editor-dokumentation (docs/editor/)
  - Session: [2026-01-05](docs/sessions/2026-01-05.md)

- `5bdfd02` feat(build): separate editor into standalone executable
  - RetroCore static library (shared code)
  - RetroEditor standalone executable
  - Session: [2026-01-05](docs/sessions/2026-01-05.md)

### Förmiddag
- `cbc0ce3` docs: update session report with evening work
- `22b59b4` refactor(editor): extract EditorState into smaller components
  - VisualRoomEditor, RoomDataManager, EditorTabRenderer, TiledIntegration
  - EditorState.cpp: 1567  600 rader (-61%)
  - Session: [2026-01-05](docs/sessions/2026-01-05.md)

- `b97942e` fix(editor): enable dragging player spawn marker in editor
- `17f2bf0` feat(editor): add player spawn editing and NPC hotspot positioning
  - Player spawn system - playerSpawnX/Y per rum
  - NPC hotspot spawning - NPCs vid hotspot-positioner
  - Session: [2026-01-05](docs/sessions/2026-01-05.md)

### Morgon
- `1f69fa6` fix: use room walk area instead of hardcoded values
- `a692225` docs: add comprehensive project documentation
- `29a2744` docs: update documentation for depth scaling and editor features

---

## 2026-01-04

### Kväll
- `a37ab7c` fix: adjust walkArea, depth scaling and font sizes
- `6f4c5f4` feat(graphics): add perspective depth scaling for characters
  - WalkArea med scaleTop/scaleBottom
  - PlayerCharacter::renderScaled()
  - Session: [2026-01-04](docs/sessions/2026-01-04.md)

- `832a399` fix(editor): save hotspots to rooms.json
- `a782a4b` feat(editor): add mute toggle and visual editor resize/walk area
- `f9dfa41` feat(editor): complete visual hotspot editor with drag-and-drop
- `6530af2` feat(editor): add modular editor architecture
- `3da956b` feat(editor): add visual room editor

### Eftermiddag
- `36b725c` fix(rendering): load and render room layers correctly
- `f5bc119` feat(editor): complete room editor with full functionality
- `1b74459` feat(editor): implement room editor UI
- `8b07728` feat(content): add tavern-inside.png as tavern background

### Förmiddag
- `32c06d4` feat(editor): make Tiled workflow optional
- `c3c397f` feat(rendering): add multi-layer rendering with depth
- `0b29b0b` feat(editor): add Import/Export Tiled buttons
- `7421493` feat(data): add TiledImporter for room editing workflow
- `867e524` fix(editor): load game data when entering EditorState
- `1854c1d` feat(editor): add EditorState accessible from main menu

### Morgon
- `35e8ba3` feat(systems): add IWorldQuery/IWorldMutator interfaces
- `88ecb02` feat(systems): add WorldState, Conditions, EventBus
- `6171de2` feat(systems): implement Fas C - Polish & Feel features
- `5981329` feat(systems): implement Fas B.3 - Recap barks system
- `7b46774` feat(systems): implement Fas A - Anti-Frustration features

---

## Äldre Commits

Se `git log` för komplett historik före 2026-01-04.

---

## Format Guide

Varje entry ska ha:
1. **Commit hash** - Länk till specifik ändring
2. **Conventional commit** - type(scope): beskrivning
3. **Detaljer** (för större ändringar) - Bullet points
4. **Session-referens** - Länk till session report

