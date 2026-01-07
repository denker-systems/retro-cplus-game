---
trigger: always_on
description: Tech stack and tooling overview
---

# Tech Stack

> Komplett teknisk stack för projektet

## Core Technologies

| Kategori | Teknologi | Version |
|----------|-----------|---------|
| Språk | C++ | 17 |
| Build | CMake | 3.16+ |
| Packages | vcpkg | latest |
| Platform | Windows | 11 |

---

## Libraries

### Engine Core
| Bibliotek | Användning |
|-----------|------------|
| SDL2 | Window, rendering, input |
| SDL2_image | PNG texture loading |
| SDL2_mixer | Audio (music, SFX) |
| Box2D | Physics simulation |
| nlohmann/json | JSON serialization |

### Editor
| Bibliotek | Användning |
|-----------|------------|
| Dear ImGui (docking) | Editor UI |
| imnodes | Node graph editor |

---

## Build Targets

| Target | Typ | Beroenden |
|--------|-----|-----------|
| RetroCore | Static lib | SDL2, Box2D, json |
| RetroGame | Executable | RetroCore |
| RetroEditor | Executable | RetroCore, ImGui |

---

## Development Tools

| Verktyg | Användning |
|---------|------------|
| Visual Studio 2022 | IDE, debugger |
| CMake | Build system |
| Git | Version control |
| vcpkg | Package management |

---

## Code Style

- **PascalCase** för klasser
- **camelCase** för metoder
- **m_** prefix för medlemsvariabler
- **#pragma once** för include guards
- **smart pointers** för ownership
