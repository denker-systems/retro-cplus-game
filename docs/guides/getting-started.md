# Getting Started

## Förutsättningar

- Windows 10/11
- Visual Studio 2022 (eller Build Tools)
- CMake 3.16+
- vcpkg (C:\vcpkg)

## Installation

### 1. Klona repo
```powershell
git clone https://github.com/[user]/retro-cplus-game.git
cd retro-cplus-game
```

### 2. Installera dependencies (om ej gjort)
```powershell
vcpkg install sdl2:x64-windows sdl2-image:x64-windows sdl2-mixer:x64-windows
```

### 3. Konfigurera CMake
```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### 4. Bygg
```powershell
cmake --build . --config Release
```

### 5. Kör
```powershell
.\Release\RetroAdventure.exe
```

## Kontroller

| Input | Keyboard | Gamepad |
|-------|----------|---------|
| Rörelse | WASD / Piltangenter | D-pad / Vänster stick |
| Bekräfta | Enter / Space | A |
| Tillbaka | ESC | B |
| Klicka | Vänster musknapp | - |

## Nästa steg

- [Projektarkitektur](../architecture/overview.md)
- [Lägga till ett rum](adding-room.md)
