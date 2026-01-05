cmake_minimum_required(VERSION 3.16)
project(RetroAdventure VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hitta SDL2 paket
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

# Källfiler
set(SOURCES
    src/main.cpp
    src/Game.cpp
    src/Renderer.cpp
    src/Input.cpp
    src/Room.cpp
    src/VideoSettings.cpp
    src/states/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/OptionsState.cpp
    src/states/PauseState.cpp
    src/states/DialogState.cpp
    src/states/InventoryState.cpp
    src/states/QuestLogState.cpp
    src/systems/DialogSystem.cpp
    src/systems/InventorySystem.cpp
    src/systems/RoomManager.cpp
    src/systems/QuestSystem.cpp
    src/systems/AISystem.cpp
    src/systems/SaveSystem.cpp
    src/systems/CutsceneSystem.cpp
    src/data/DataLoader.cpp
    src/graphics/TextureManager.cpp
    src/graphics/SpriteSheet.cpp
    src/graphics/Animation.cpp
    src/graphics/FontManager.cpp
    src/graphics/Transition.cpp
    src/audio/AudioManager.cpp
    src/entities/Entity.cpp
    src/entities/Character.cpp
    src/entities/PlayerCharacter.cpp
    src/entities/NPC.cpp
    src/ui/Widget.cpp
)

# Skapa executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Inkludera headers
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Hitta nlohmann_json
find_package(nlohmann_json CONFIG REQUIRED)

# Länka bibliotek (vcpkg targets)
target_link_libraries(${PROJECT_NAME} PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    nlohmann_json::nlohmann_json
)

# Kopiera assets till build-mappen
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
