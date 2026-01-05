# Build System

## Verktyg

| Verktyg | Version | Syfte |
|---------|---------|-------|
| CMake | 3.16+ | Build system |
| vcpkg | Latest | Pakethantering |
| MSVC | 2022 | Kompilator |

## Dependencies

```
sdl2:x64-windows
sdl2-image:x64-windows
sdl2-mixer:x64-windows
```

## CMake Konfiguration

```cmake
cmake_minimum_required(VERSION 3.16)
project(RetroAdventure)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")
```

## Kommandon

### Full rebuild
```powershell
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

### Incremental build
```powershell
cd build
cmake --build . --config Release
```

### Clean
```powershell
cd build
cmake --build . --target clean
```

## Filstruktur

```
build/
├── Release/
│   └── RetroAdventure.exe
├── CMakeCache.txt
└── RetroAdventure.vcxproj
```
