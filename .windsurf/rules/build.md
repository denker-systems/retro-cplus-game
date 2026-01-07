---
trigger: always_on
description: Build commands and configurations
---

# Build System

> CMake + vcpkg build system

## Quick Commands

### Build Release
// turbo
```powershell
cd build; cmake --build . --config Release
```

### Build Debug
```powershell
cd build; cmake --build . --config Debug
```

### Run Game
```powershell
.\build\Release\RetroGame.exe
```

### Run Editor
```powershell
.\build\Release\RetroEditor.exe
```

---

## Full Rebuild

```powershell
cd build
Remove-Item -Recurse -Force *
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

---

## Build Targets

| Target | Output | Description |
|--------|--------|-------------|
| RetroCore | .lib | Engine static library |
| RetroGame | .exe | Game executable |
| RetroEditor | .exe | Editor executable |

---

## Configurations

| Config | Optimering | Symbols | Användning |
|--------|------------|---------|------------|
| Debug | Ingen | Ja | Utveckling, debugging |
| Release | Full | Nej | Distribution |
| RelWithDebInfo | Full | Ja | Profiling |
