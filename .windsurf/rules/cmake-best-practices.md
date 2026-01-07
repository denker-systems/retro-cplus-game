---
trigger: always_on
description: Modern CMake best practices
---

# CMake Best Practices

> Modern CMake 3.15+ guidelines

## Project Structure

```cmake
cmake_minimum_required(VERSION 3.16)
project(RetroEngine 
    VERSION 1.0.0
    DESCRIPTION "Retro Game Engine"
    LANGUAGES CXX
)

# C++17 as target requirement
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

---

## Target-Based CMake

### Create Library
```cmake
add_library(RetroCore STATIC
    src/engine/core/Object.cpp
    src/engine/core/Actor.cpp
)

target_include_directories(RetroCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/engine
)
```

### Create Executable
```cmake
add_executable(RetroGame
    src/game/main.cpp
    src/game/Game.cpp
)

target_link_libraries(RetroGame
    PRIVATE
        RetroCore
        SDL2::SDL2
        SDL2_image::SDL2_image
)
```

---

## Dependency Management

### vcpkg Integration
```cmake
# In CMakePresets.json or command line
# -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
```

### Link Dependencies
```cmake
target_link_libraries(RetroCore
    PUBLIC
        nlohmann_json::nlohmann_json
    PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        box2d::box2d
)
```

---

## Build Options

```cmake
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_EDITOR "Build editor application" ON)
option(ENABLE_ASAN "Enable Address Sanitizer" OFF)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_EDITOR)
    add_subdirectory(src/editor)
endif()
```

---

## Compiler Warnings

```cmake
if(MSVC)
    target_compile_options(RetroCore PRIVATE /W4 /WX)
else()
    target_compile_options(RetroCore PRIVATE 
        -Wall -Wextra -Wpedantic -Werror)
endif()
```

---

## Asset Copying

```cmake
# Copy assets to build directory
add_custom_command(TARGET RetroGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:RetroGame>/assets
)
```

---

## Best Practices

### DO
- ✓ Use `target_*` commands
- ✓ Specify PUBLIC/PRIVATE/INTERFACE
- ✓ Use generator expressions
- ✓ Keep CMakeLists.txt modular
- ✓ Use `find_package` for dependencies

### DON'T
- ❌ Set global variables (CMAKE_CXX_FLAGS)
- ❌ Use `include_directories()`
- ❌ Use `link_directories()`
- ❌ Hardcode paths
- ❌ Use `file(GLOB)` for sources
