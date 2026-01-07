---
trigger: always_on
description: Dependency management with vcpkg
---

# Dependencies

> vcpkg package management

## vcpkg Setup

```powershell
# Location
C:\vcpkg

# Toolchain
-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

---

## Core Packages

| Package | Purpose |
|---------|---------|
| sdl2 | Window, rendering |
| sdl2-image | PNG loading |
| sdl2-mixer | Audio |
| box2d | Physics |
| nlohmann-json | JSON |

## Editor Packages

| Package | Purpose |
|---------|---------|
| imgui[docking] | Editor UI |
| imnodes | Node graphs |

---

## Install Commands

```powershell
# Core
vcpkg install sdl2:x64-windows
vcpkg install sdl2-image:x64-windows
vcpkg install sdl2-mixer:x64-windows
vcpkg install box2d:x64-windows
vcpkg install nlohmann-json:x64-windows

# Editor
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

## Adding Dependencies

1. `vcpkg install package:x64-windows`
2. Add `find_package()` to CMakeLists.txt
3. Add `target_link_libraries()`
4. Update this document
