cmake_minimum_required(VERSION 3.16)
project(MyGame VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use vcpkg for dependencies
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# ============================================================================
# DEPENDENCIES
# ============================================================================
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)

# ============================================================================
# RETRO ENGINE CORE
# ============================================================================
# Link against pre-built RetroCore.lib from engine installation
set(ENGINE_PATH "C:\Users\Calle\Documents\GitHub\retro-cplus-game")
add_library(RetroCore STATIC IMPORTED)
set_target_properties(RetroCore PROPERTIES
    IMPORTED_LOCATION "${ENGINE_PATH}/lib/RetroCore.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${ENGINE_PATH}/include"
)

# ============================================================================
# GAME EXECUTABLE
# ============================================================================
set(GAME_SOURCES
    src/main.cpp
)

add_executable(MyGame ${GAME_SOURCES})

target_include_directories(MyGame PRIVATE
    ${ENGINE_PATH}/include
)

target_link_libraries(MyGame PRIVATE
    RetroCore
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    nlohmann_json::nlohmann_json
    box2d::box2d
    glm::glm
    GLEW::GLEW
    OpenGL::GL
    assimp::assimp
    unofficial::omniverse-physx-sdk::sdk
)

# ============================================================================
# POST BUILD - Copy assets
# ============================================================================
add_custom_command(TARGET MyGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:MyGame>/assets
)
