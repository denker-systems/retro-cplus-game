# DEVLOG - Retro Adventure

Kronologisk logg av alla ändringar sedan projektets start.

**Format:** `[HASH]` type(scope): beskrivning

---

## 2026-01-06

### Kväll

- `fd76906` refactor(editor): separate EditorState into manager classes
  - **Arkitektur:** EditorState.cpp reducerad från 998 till 211 rader (-79%)
  - **Separation of Concerns:** Extraherade funktionalitet till manager-klasser
  - **EditorPanelManager:** Skapar och hanterar alla editor panels (~150 rader)
  - **EditorWorldManager:** World/Level/Scene setup från RoomData (~120 rader)
  - **EditorEventDispatcher:** SDL event handling och shortcuts (~80 rader)
  - **ViewportPanel:** Uppdelad i 6 filer (Core, World, Level, Scene, Actors, Input)
  - **PlaceActorsPanel:** Ny panel för drag-and-drop av actor templates (UE5-style)
  - **EditorInputHandler:** Centraliserad input med Windows shortcuts
  - **Shortcuts:** Del (delete actor), Ctrl+D (duplicate), Ctrl+Z/Y (undo/redo), Ctrl+S (save)
  - **WorldContainer:** Lade till removeActor() metod
  - **Build:** CMakeLists.txt uppdaterad med 9 nya filer
  - **Statistik:** 92 filer ändrade, 4478 insertions, 1557 deletions
  - Session: [2026-01-06](sessions/2026-01-06.md)

## 2026-01-05

### Kväll (Sen)

- `e28d059` feat(editor): add FileBrowser and enhance HierarchyPanel for Actor system