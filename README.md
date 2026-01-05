# Retro Adventure (C++ / SDL2)

Ett point-and-click adventure-spel för Windows, byggt med C++17 och SDL2.

## Krav

- CMake 3.16+
- SDL2
- SDL2_image
- SDL2_mixer
- Visual Studio 2019+ eller MinGW

## Installation av SDL2 (vcpkg)

```powershell
# Installera vcpkg om det inte finns
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat

# Installera SDL2
.\vcpkg install sdl2:x64-windows
.\vcpkg install sdl2-image:x64-windows
.\vcpkg install sdl2-mixer:x64-windows

# Integrera med CMake
.\vcpkg integrate install
```

## Bygga

```powershell
mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg-path]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

## Kontroller

- **Piltangenter / WASD** - Gå
- **ESC** - Avsluta

## Projektstruktur

```
src/
├── main.cpp      # Entry point
├── Game.cpp/h    # Spelloop och init
├── Renderer.cpp/h # Textur-laddning
├── Input.cpp/h   # Tangentbord
├── Player.cpp/h  # Spelarlogik
└── Room.cpp/h    # Rum och hotspots
```

## Parallellt projekt

Detta är C++/SDL2-versionen av DOS-spelet i `retro-game`.
