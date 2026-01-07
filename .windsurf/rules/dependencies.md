---
trigger: always_on
description: Project dependencies managed via vcpkg
---

# Dependencies

> vcpkg package manager for C++ libraries

## vcpkg Location

```
C:\vcpkg
Toolchain: C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

---

## Installed Packages

### Core (Game + Engine)
| Package | Version | Användning |
|---------|---------|------------|
| sdl2 | x64-windows | Window, rendering, input |
| sdl2-image | x64-windows | PNG texture loading |
| sdl2-mixer | x64-windows | Audio playback |
| box2d | x64-windows | Physics simulation |
| nlohmann-json | x64-windows | JSON serialization |

### Editor Only
| Package | Version | Användning |
|---------|---------|------------|
| imgui[docking] | x64-windows | Editor UI |
| imnodes | x64-windows | Node graph editor |

---

## Install Commands

```powershell
# Core dependencies
vcpkg install sdl2:x64-windows
vcpkg install sdl2-image:x64-windows
vcpkg install sdl2-mixer:x64-windows
vcpkg install box2d:x64-windows
vcpkg install nlohmann-json:x64-windows

# Editor dependencies
vcpkg install imgui[docking-experimental]:x64-windows
vcpkg install imnodes:x64-windows
```

---

## CMake Integration

```cmake
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
```

---

## Adding New Dependencies

1. Install via vcpkg: `vcpkg install package:x64-windows`
2. Add find_package() to CMakeLists.txt
3. Add target_link_libraries()
4. Update this document
