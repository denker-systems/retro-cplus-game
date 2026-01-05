cmake_minimum_required(VERSION 3.16)
project(RetroAdventure VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hitta dependencies
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui CONFIG QUIET)

# ============================================================================
# RETRO CORE LIBRARY - Delad kod mellan Game och Editor
# ============================================================================
set(CORE_SOURCES
    src/Renderer.cpp
    src/Input.cpp
    src/Room.cpp
    src/VideoSettings.cpp
    src/systems/DialogSystem.cpp
    src/systems/InventorySystem.cpp
    src/systems/RoomManager.cpp
    src/systems/QuestSystem.cpp
    src/systems/AISystem.cpp
    src/systems/SaveSystem.cpp
    src/systems/CutsceneSystem.cpp
    src/systems/HintSystem.cpp
    src/systems/RecapSystem.cpp
    src/systems/GateSystem.cpp
    src/systems/JournalSystem.cpp
    src/systems/WorldState.cpp
    src/systems/ConditionSystem.cpp
    src/systems/EventBus.cpp
    src/systems/WorldQuery.cpp
    src/data/DataLoader.cpp
    src/data/TiledImporter.cpp
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
    src/utils/Logger.cpp
)

add_library(RetroCore STATIC ${CORE_SOURCES})
target_include_directories(RetroCore PUBLIC src)
target_link_libraries(RetroCore PUBLIC
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    nlohmann_json::nlohmann_json
)

# ============================================================================
# RETRO GAME - Huvudspel executable (inkluderar in-game editor)
# ============================================================================
set(GAME_SOURCES
    src/game/main.cpp
    src/Game.cpp
    src/states/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/OptionsState.cpp
    src/states/PauseState.cpp
    src/states/DialogState.cpp
    src/states/InventoryState.cpp
    src/states/QuestLogState.cpp
    src/states/SaveLoadState.cpp
    src/states/EditorState.cpp
    src/editor/DebugEditor.cpp
    src/editor/EditorCore.cpp
    src/editor/EditorContext.cpp
    src/editor/VisualRoomEditor.cpp
    src/editor/RoomDataManager.cpp
    src/editor/EditorTabRenderer.cpp
    src/editor/TiledIntegration.cpp
    src/editor/widgets/Widget.cpp
    src/editor/widgets/Button.cpp
    src/editor/widgets/ListView.cpp
    src/editor/panels/RoomPanel.cpp
    src/editor/commands/HotspotCommands.cpp
)

add_executable(RetroGame ${GAME_SOURCES})
target_link_libraries(RetroGame PRIVATE
    RetroCore
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
)

# ============================================================================
# RETRO EDITOR - Standalone editor executable
# ============================================================================
set(EDITOR_SOURCES
    src/editor/main.cpp
    src/editor/EditorApp.cpp
    src/editor/DebugEditor.cpp
    src/editor/EditorCore.cpp
    src/editor/EditorContext.cpp
    src/editor/VisualRoomEditor.cpp
    src/editor/RoomDataManager.cpp
    src/editor/EditorTabRenderer.cpp
    src/editor/TiledIntegration.cpp
    src/editor/widgets/Widget.cpp
    src/editor/widgets/Button.cpp
    src/editor/widgets/ListView.cpp
    src/editor/panels/RoomPanel.cpp
    src/editor/commands/HotspotCommands.cpp
)

add_executable(RetroEditor ${EDITOR_SOURCES})
target_link_libraries(RetroEditor PRIVATE
    RetroCore
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)

# ============================================================================
# LEGACY - Kombinerad executable (för bakåtkompatibilitet)
# ============================================================================
set(ALL_SOURCES
    src/main.cpp
    src/Game.cpp
    ${CORE_SOURCES}
    src/states/StateManager.cpp
    src/states/MenuState.cpp
    src/states/PlayState.cpp
    src/states/OptionsState.cpp
    src/states/PauseState.cpp
    src/states/DialogState.cpp
    src/states/InventoryState.cpp
    src/states/QuestLogState.cpp
    src/states/SaveLoadState.cpp
    src/states/EditorState.cpp
    src/editor/DebugEditor.cpp
    src/editor/EditorCore.cpp
    src/editor/EditorContext.cpp
    src/editor/VisualRoomEditor.cpp
    src/editor/RoomDataManager.cpp
    src/editor/EditorTabRenderer.cpp
    src/editor/TiledIntegration.cpp
    src/editor/widgets/Widget.cpp
    src/editor/widgets/Button.cpp
    src/editor/widgets/ListView.cpp
    src/editor/panels/RoomPanel.cpp
    src/editor/commands/HotspotCommands.cpp
)

add_executable(RetroAdventure ${ALL_SOURCES})
target_include_directories(RetroAdventure PRIVATE src)
target_link_libraries(RetroAdventure PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    nlohmann_json::nlohmann_json
    $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)

# ============================================================================
# POST BUILD - Kopiera assets
# ============================================================================
foreach(target RetroGame RetroEditor RetroAdventure)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${target}>/assets
    )
endforeach()
