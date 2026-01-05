cmake_minimum_required(VERSION 3.16)
project(RetroAdventure VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hitta SDL2 (vcpkg)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

# Källfiler
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Renderer.cpp
    src/Input.cpp
    src/Room.cpp
    src/states/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/OptionsState.cpp
    src/states/PauseState.cpp
    src/graphics/TextureManager.cpp
    src/graphics/SpriteSheet.cpp
    src/graphics/Animation.cpp
    src/audio/AudioManager.cpp
    src/entities/Entity.cpp
    src/entities/Character.cpp
    src/entities/PlayerCharacter.cpp
)

# Skapa executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Inkludera headers
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Länka bibliotek (vcpkg targets)
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
)

# Kopiera assets till build-mappen
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
