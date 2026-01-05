---
trigger: always_on
---

# Project Dependencies

<vcpkg>
Location: C:\vcpkg
Toolchain: C:/vcpkg/scripts/buildsystems/vcpkg.cmake

Installed packages:
- sdl2:x64-windows
- sdl2-image:x64-windows
- sdl2-mixer:x64-windows
</vcpkg>

<cmake_targets>
`cmake
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)
`
</cmake_targets>

<includes>
`cpp
// SDL2
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

// Standard library
#include <memory>      // smart pointers
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>  // std::function
#include <iostream>    // debug output
`
</includes>

<future_dependencies>
När vi lägger till:
- SDL2_ttf för fonts: `vcpkg install sdl2-ttf:x64-windows`
- nlohmann/json för data: `vcpkg install nlohmann-json:x64-windows`
</future_dependencies>
