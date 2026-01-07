---
trigger: glob
globs: ["CMakeLists.txt"]
description: CMake build system guidelines
---

# CMake Guidelines

> vcpkg + CMake för cross-platform builds

## Project Structure

```cmake
cmake_minimum_required(VERSION 3.16)
project(RetroEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

---

## vcpkg Integration

```cmake
# Configure with vcpkg toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
```

---

## Find Packages

```cmake
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
```

---

## Target Linking

```cmake
target_link_libraries(RetroCore PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    box2d::box2d
    nlohmann_json::nlohmann_json
)
```

---

## Asset Copying

```cmake
add_custom_command(TARGET RetroGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets 
    $<TARGET_FILE_DIR:RetroGame>/assets
)
```

---

## Best Practices

- **PRIVATE linking** för implementation dependencies
- **PUBLIC linking** endast för interface dependencies
- **Separata targets** för engine, game, editor
