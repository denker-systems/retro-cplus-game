---
description: Komplett build-process för Retro Engine
auto_execution_mode: 1
---

# Build Workflow

> Bygg engine, game och editor för Retro Engine

## 1. Snabb Build (Vanligast)

### Build Release
// turbo
```powershell
cd build; cmake --build . --config Release
```

### Build Debug
```powershell
cd build; cmake --build . --config Debug
```

### Build Med Verbose Output
```powershell
cd build; cmake --build . --config Release --verbose
```

---

## 2. Build Configurations

| Config | Optimering | Debug Symbols | Användning |
|--------|------------|---------------|------------|
| **Debug** | Ingen | Ja | Debugging, breakpoints |
| **Release** | Full (-O2) | Nej | Distribution, testning |
| **RelWithDebInfo** | Full | Ja | Profiling, crash analysis |
| **MinSizeRel** | Storlek | Nej | Size-constrained builds |

### Växla Configuration
```powershell
# Debug för utveckling
cmake --build . --config Debug

# Release för testning/distribution
cmake --build . --config Release

# RelWithDebInfo för profiling
cmake --build . --config RelWithDebInfo
```

---

## 3. Build Targets

### Projektstruktur

| Target | Output | Typ | Beskrivning |
|--------|--------|-----|-------------|
| **RetroCore** | `RetroCore.lib` | Static Library | Engine core - rendering, physics, audio |
| **RetroGame** | `RetroGame.exe` | Executable | Spelkörbar fil |
| **RetroEditor** | `RetroEditor.exe` | Executable | Level editor med ImGui |
| **ALL_BUILD** | Alla | Meta | Bygger alla targets |

### Target Dependencies
```
RetroGame ─────┐
               ├──► RetroCore ──► SDL2, Box2D, nlohmann_json
RetroEditor ───┘              ──► ImGui, imnodes (editor only)
```

### Bygg Specifikt Target
```powershell
# Endast engine library
cmake --build . --config Release --target RetroCore

# Endast spelet
cmake --build . --config Release --target RetroGame

# Endast editorn
cmake --build . --config Release --target RetroEditor

# Allt
cmake --build . --config Release --target ALL_BUILD
```

---

## 4. Kör Efter Build

### Spelet
```powershell
cd build
cmake --build . --config Release --target RetroGame
.\Release\RetroGame.exe
```

### Editorn
```powershell
cd build
cmake --build . --config Release --target RetroEditor
.\Release\RetroEditor.exe
```

### Build och Kör (One-liner)
```powershell
# Build och kör spelet
cd build; cmake --build . --config Release; if ($LASTEXITCODE -eq 0) { .\Release\RetroGame.exe }

# Build och kör editorn
cd build; cmake --build . --config Release; if ($LASTEXITCODE -eq 0) { .\Release\RetroEditor.exe }
```

---

## 5. Parallel Build

### Använd Alla CPU-kärnor
```powershell
cmake --build . --config Release --parallel
```

### Specifikt Antal Kärnor
```powershell
cmake --build . --config Release --parallel 8
```

### Kontrollera Antal Kärnor
```powershell
Write-Host "Antal CPU-kärnor: $env:NUMBER_OF_PROCESSORS"
```

---

## 6. Incremental vs Full Rebuild

### Incremental Build (Standard)
```powershell
# Bygger endast ändrade filer
cmake --build . --config Release
```

### Full Rebuild (Clean Build)
```powershell
cd build

# Ta bort alla build artifacts
Remove-Item -Recurse -Force * -ErrorAction SilentlyContinue

# Kör CMake igen
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Bygg allt
cmake --build . --config Release
```

### Clean Target
```powershell
cmake --build . --target clean
cmake --build . --config Release
```

---

## 7. vcpkg Dependencies

### Installera Alla Dependencies

```powershell
# Navigera till vcpkg
cd C:\vcpkg

# Core dependencies (krävs av alla targets)
.\vcpkg install sdl2:x64-windows
.\vcpkg install sdl2-image:x64-windows
.\vcpkg install sdl2-mixer:x64-windows
.\vcpkg install box2d:x64-windows
.\vcpkg install nlohmann-json:x64-windows

# Editor dependencies (endast RetroEditor)
.\vcpkg install imgui[docking-experimental]:x64-windows
.\vcpkg install imnodes:x64-windows
```

### All-In-One Installation
```powershell
cd C:\vcpkg
.\vcpkg install sdl2:x64-windows sdl2-image:x64-windows sdl2-mixer:x64-windows box2d:x64-windows nlohmann-json:x64-windows imgui[docking-experimental]:x64-windows imnodes:x64-windows
```

### Verifiera Installation
```powershell
.\vcpkg list
```

### Update Dependencies
```powershell
cd C:\vcpkg
git pull
.\vcpkg upgrade --no-dry-run
```

---

## 8. CMake Configuration

### Initial Setup
```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Reconfigure
```powershell
cd build
cmake ..
```

### CMake Options
```powershell
# Visa alla options
cmake -LAH ..

# Med specifika options
cmake .. -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTS=ON
```

---

## 9. Build Verification

### Kontrollera För Errors
```powershell
cmake --build . --config Release 2>&1 | Select-String "error"
```

### Kontrollera För Warnings
```powershell
cmake --build . --config Release 2>&1 | Select-String "warning"
```

### Full Build Log
```powershell
cmake --build . --config Release 2>&1 | Tee-Object -FilePath build.log
```

---

## 10. Felsökning

### Vanliga Build Errors

| Error | Orsak | Lösning |
|-------|-------|---------|
| `SDL2 not found` | vcpkg package saknas | `vcpkg install sdl2:x64-windows` |
| `SDL2_image not found` | vcpkg package saknas | `vcpkg install sdl2-image:x64-windows` |
| `SDL2_mixer not found` | vcpkg package saknas | `vcpkg install sdl2-mixer:x64-windows` |
| `box2d not found` | vcpkg package saknas | `vcpkg install box2d:x64-windows` |
| `nlohmann_json not found` | vcpkg package saknas | `vcpkg install nlohmann-json:x64-windows` |
| `imgui not found` | vcpkg package saknas | `vcpkg install imgui[docking-experimental]:x64-windows` |
| `imnodes not found` | vcpkg package saknas | `vcpkg install imnodes:x64-windows` |
| `LNK2019: unresolved external` | Missing library link | Kolla `target_link_libraries` i CMakeLists.txt |
| `Cannot open include file` | Include path saknas | Kolla `target_include_directories` |
| `multiple definitions` | Header saknar `#pragma once` | Lägg till `#pragma once` |
| Stale build artifacts | CMake cache korrupt | Full rebuild |

### Linker Errors

```powershell
# Hitta vilken symbol som saknas
cmake --build . --config Release 2>&1 | Select-String "unresolved external"

# Vanliga orsaker:
# 1. Glömt lägga till .cpp fil i CMakeLists.txt
# 2. Glömt länka library i target_link_libraries
# 3. Deklaration utan implementation
```

### CMake Cache Problems
```powershell
# Rensa CMake cache
cd build
Remove-Item CMakeCache.txt
Remove-Item -Recurse CMakeFiles
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

---

## 11. CI Build Script

```powershell
# Full CI build script
$ErrorActionPreference = "Stop"

Write-Host "=== RETRO ENGINE CI BUILD ===" -ForegroundColor Cyan
Write-Host "Start: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"

# Configure
Write-Host "`n=== CMake Configure ===" -ForegroundColor Yellow
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# Build Release
Write-Host "`n=== Build Release ===" -ForegroundColor Yellow
cmake --build . --config Release --parallel
if ($LASTEXITCODE -ne 0) { throw "Release build failed" }

# Build Debug
Write-Host "`n=== Build Debug ===" -ForegroundColor Yellow
cmake --build . --config Debug --parallel
if ($LASTEXITCODE -ne 0) { throw "Debug build failed" }

# Run tests (if available)
Write-Host "`n=== Run Tests ===" -ForegroundColor Yellow
ctest -C Release --output-on-failure

Write-Host "`n=== BUILD COMPLETE ===" -ForegroundColor Green
Write-Host "End: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')"
```

---

## 12. Build Checklista

### Innan Commit
- [ ] Release build fungerar
- [ ] Debug build fungerar
- [ ] Inga nya warnings
- [ ] Game startar
- [ ] Editor startar

### Vid Problem
- [ ] Kolla error message noga
- [ ] Sök efter specifik error
- [ ] Kontrollera CMakeLists.txt
- [ ] Verifiera vcpkg packages
- [ ] Prova full rebuild

---

## Quick Reference

### Vanligaste Kommandon
```powershell
# Standard build
cd build; cmake --build . --config Release

# Build och kör game
cd build; cmake --build . --config Release; .\Release\RetroGame.exe

# Build och kör editor
cd build; cmake --build . --config Release; .\Release\RetroEditor.exe

# Full rebuild
cd build; Remove-Item -Recurse -Force *; cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake; cmake --build . --config Release

# Debug build
cd build; cmake --build . --config Debug
```

### Fil Locations
```
build/
├── Release/
│   ├── RetroGame.exe
│   ├── RetroEditor.exe
│   └── RetroCore.lib
├── Debug/
│   ├── RetroGame.exe
│   ├── RetroEditor.exe
│   └── RetroCore.lib
└── CMakeCache.txt
```
