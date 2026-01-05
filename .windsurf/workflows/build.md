---
description: Bygger och kör spelet snabbt
---

# Build & Run Workflow

## 1. Bygg Release
// turbo
`powershell
cd build; cmake --build . --config Release
`

## 2. Kör spelet
`powershell
.\Release\RetroAdventure.exe
`

## Quick Build (om CMake redan konfigurerat)
// turbo
`powershell
cd build; cmake --build . --config Release; if ($LASTEXITCODE -eq 0) { .\Release\RetroAdventure.exe }
`

## Full Rebuild (vid CMake-ändringar)
`powershell
cd build
Remove-Item -Recurse -Force *
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
`

## Debug Build
`powershell
cd build
cmake --build . --config Debug
.\Debug\RetroAdventure.exe
`
