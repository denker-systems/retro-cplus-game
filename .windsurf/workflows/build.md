---
description: Build and run the project quickly
---

# Build Workflow

> Snabb build och körning

## Build Release
// turbo
```powershell
cd build; cmake --build . --config Release
```

## Run Game
```powershell
.\build\Release\RetroGame.exe
```

## Run Editor
```powershell
.\build\Release\RetroEditor.exe
```

---

## Quick Build + Run
// turbo
```powershell
cd build; cmake --build . --config Release; if ($LASTEXITCODE -eq 0) { .\Release\RetroGame.exe }
```

---

## Full Rebuild
```powershell
cd build
Remove-Item -Recurse -Force * -ErrorAction SilentlyContinue
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

---

## Debug Build
```powershell
cd build; cmake --build . --config Debug
.\Debug\RetroGame.exe
```
