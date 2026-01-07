# ADR-002: ImGui Editor Architecture

**Status:** Accepted  
**Datum:** 2026-01-05  
**Beslutad av:** Calle

## Kontext

Projektet behöver en visuell editor för att redigera rum, hotspots, dialoger och annat spelinnehåll. Den ursprungliga custom UI-implementationen blev för komplex och svår att underhålla (1567 rader i EditorState.cpp).

## Beslut

Använd **Dear ImGui** med docking-support för editor-UI:
- **imgui[docking-experimental]** via vcpkg
- Panel-baserad arkitektur med IEditorPanel interface
- 5 huvudpaneler: Hierarchy, Properties, Viewport, AssetBrowser, Console
- Standalone editor executable (RetroEditor.exe)
- Windowed mode för editor, fullscreen för spel

## Alternativ Övervägda

### Alternativ 1: Fortsätt med Custom UI
- **Fördelar:** Ingen extern dependency
- **Nackdelar:** Mycket kod, svårt att underhålla, begränsad funktionalitet

### Alternativ 2: Qt
- **Fördelar:** Fullständig widget-toolkit, professionell
- **Nackdelar:** Stor dependency, komplex integration med SDL2, licensfrågor

### Alternativ 3: wxWidgets
- **Fördelar:** Native look, vältestat
- **Nackdelar:** Komplex integration, äldre API

## Konsekvenser

### Positiva
- Professionell editor med minimal kod
- Docking layout ger flexibilitet
- Enkelt att lägga till nya paneler
- Bra dokumentation och community
- EditorState.cpp: 1567  600 rader (-61%)

### Negativa
- Extern dependency
- Kräver vendored SDL2 backends
- Docking är experimentell feature

## Referenser

- [Dear ImGui](https://github.com/ocornut/imgui)
- [ImGui Docking](https://github.com/ocornut/imgui/wiki/Docking)
- [Session 2026-01-05](../sessions/2026-01-05.md)
