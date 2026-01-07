# ADR-001: SDL2 Tech Stack

**Status:** Accepted  
**Datum:** 2026-01-03  
**Beslutad av:** Calle

## Kontext

Projektet behöver en robust grafik- och ljudmotor för att bygga ett point-and-click adventure-spel. Valet står mellan flera alternativ för C++ spelutveckling.

## Beslut

Använd SDL2 med tillhörande bibliotek:
- **SDL2** - Fönsterhantering, rendering, input
- **SDL2_image** - PNG/JPG textur-laddning
- **SDL2_mixer** - Musik och ljudeffekter
- **SDL2_ttf** - Font-rendering

Pakethantering via **vcpkg** för enkel dependency-hantering.

## Alternativ Övervägda

### Alternativ 1: SFML
- **Fördelar:** Enklare API, bra för 2D
- **Nackdelar:** Mindre flexibelt, färre tutorials

### Alternativ 2: Raylib
- **Fördelar:** Mycket enkelt, snabb prototyping
- **Nackdelar:** Mindre kontroll, färre features

### Alternativ 3: Custom OpenGL
- **Fördelar:** Full kontroll
- **Nackdelar:** Mycket mer kod, längre utvecklingstid

## Konsekvenser

### Positiva
- Vältestat och stabilt bibliotek
- Stor community och dokumentation
- Bra vcpkg-integration
- Fungerar på Windows, Mac, Linux

### Negativa
- C-baserat API kräver wrappers
- Manuell minneshantering för SDL-resurser

## Referenser

- [SDL2 Wiki](https://wiki.libsdl.org/)
- [vcpkg](https://vcpkg.io/)
