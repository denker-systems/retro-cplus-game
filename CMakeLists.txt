cmake_minimum_required(VERSION 3.16)
project(RetroAdventure VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================================
# DEPENDENCIES
# ============================================================================
# Core dependencies (required)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Editor dependencies (optional)
find_package(imgui CONFIG QUIET)
find_package(imguizmo CONFIG QUIET)
find_package(implot CONFIG QUIET)
find_package(nfd CONFIG QUIET)

# Dev dependencies (optional)
find_package(spdlog CONFIG QUIET)
# efsw removed - using polling-based FileWatcher instead (more robust)

# Report what we found
message(STATUS "ImGui found: ${imgui_FOUND}")
message(STATUS "ImGuizmo found: ${imguizmo_FOUND}")
message(STATUS "ImPlot found: ${implot_FOUND}")
message(STATUS "NativeFileDialog found: ${nfd_FOUND}")

# ============================================================================
# RETRO CORE LIBRARY - Delad kod mellan Game och Editor
# ============================================================================
set(CORE_SOURCES
    src/engine/Renderer.cpp
    src/engine/input/Input.cpp
    src/engine/Room.cpp
    src/engine/VideoSettings.cpp
    src/engine/systems/DialogSystem.cpp
    src/engine/systems/InventorySystem.cpp
    src/engine/systems/RoomManager.cpp
    src/engine/systems/QuestSystem.cpp
    src/engine/systems/AISystem.cpp
    src/engine/systems/SaveSystem.cpp
    src/engine/systems/CutsceneSystem.cpp
    src/engine/systems/HintSystem.cpp
    src/engine/systems/RecapSystem.cpp
    src/engine/systems/GateSystem.cpp
    src/engine/systems/JournalSystem.cpp
    src/engine/systems/WorldState.cpp
    src/engine/systems/ConditionSystem.cpp
    src/engine/systems/EventBus.cpp
    src/engine/systems/WorldQuery.cpp
    src/engine/data/DataLoader.cpp
    src/engine/data/TiledImporter.cpp
    src/engine/graphics/TextureManager.cpp
    src/engine/graphics/SpriteSheet.cpp
    src/engine/graphics/Animation.cpp
    src/engine/graphics/FontManager.cpp
    src/engine/graphics/Transition.cpp
    src/engine/audio/AudioManager.cpp
    src/engine/entities/Entity.cpp
    src/engine/entities/Character.cpp
    src/engine/entities/PlayerCharacter.cpp
    src/engine/entities/NPC.cpp
    src/engine/ui/Widget.cpp
    src/engine/utils/Logger.cpp
    src/engine/utils/FileWatcher.cpp
)

add_library(RetroCore STATIC ${CORE_SOURCES})
target_include_directories(RetroCore PUBLIC src src/engine)
target_link_libraries(RetroCore PUBLIC
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    nlohmann_json::nlohmann_json
)

# Hot Reload alltid tillgängligt (polling-baserat, ingen extern dependency)
message(STATUS "Hot Reload enabled (polling-based FileWatcher)")

# ============================================================================
# RETRO GAME - Huvudspel executable (utan editor)
# ============================================================================
set(GAME_SOURCES
    src/game/main.cpp
    src/game/Game.cpp
    src/game/states/StateManager.cpp
    src/game/states/MenuState.cpp
    src/game/states/PlayState.cpp
    src/game/states/OptionsState.cpp
    src/game/states/PauseState.cpp
    src/game/states/DialogState.cpp
    src/game/states/InventoryState.cpp
    src/game/states/QuestLogState.cpp
    src/game/states/SaveLoadState.cpp
)

add_executable(RetroGame ${GAME_SOURCES})
target_link_libraries(RetroGame PRIVATE
    RetroCore
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
)

# ============================================================================
# RETRO EDITOR - Standalone editor executable
# ============================================================================
# ImGui SDL2 backend (vendored)
set(IMGUI_BACKEND_SOURCES
    src/vendor/imgui/imgui_impl_sdl2.cpp
    src/vendor/imgui/imgui_impl_sdlrenderer2.cpp
)

set(EDITOR_SOURCES
    src/editor/main.cpp
    src/game/Game.cpp
    src/game/states/StateManager.cpp
    src/game/states/MenuState.cpp
    src/game/states/PlayState.cpp
    src/game/states/OptionsState.cpp
    src/game/states/PauseState.cpp
    src/game/states/DialogState.cpp
    src/game/states/InventoryState.cpp
    src/game/states/QuestLogState.cpp
    src/game/states/SaveLoadState.cpp
    src/editor/EditorState.cpp
    src/editor/components/DebugEditor.cpp
    src/editor/EditorContext.cpp
    src/editor/components/VisualRoomEditor.cpp
    src/editor/components/RoomDataManager.cpp
    src/editor/components/EditorTabRenderer.cpp
    src/editor/components/TiledIntegration.cpp
    src/editor/panels/HierarchyPanel.cpp
    src/editor/panels/PropertiesPanel.cpp
    src/editor/panels/ViewportPanel.cpp
    src/editor/panels/AssetBrowserPanel.cpp
    src/editor/panels/ConsolePanel.cpp
    src/editor/ImGuiManager.cpp
    ${IMGUI_BACKEND_SOURCES}
)

add_executable(RetroEditor ${EDITOR_SOURCES})
target_include_directories(RetroEditor PRIVATE src/vendor)
target_link_libraries(RetroEditor PRIVATE
    RetroCore
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<TARGET_NAME_IF_EXISTS:imgui::imgui>
    $<TARGET_NAME_IF_EXISTS:imguizmo::imguizmo>
    $<TARGET_NAME_IF_EXISTS:implot::implot>
    $<TARGET_NAME_IF_EXISTS:nfd::nfd>
    $<TARGET_NAME_IF_EXISTS:spdlog::spdlog>
    $<TARGET_NAME_IF_EXISTS:efsw::efsw>
)

# Definiera HAS_EDITOR för att inkludera editor-funktionalitet
target_compile_definitions(RetroEditor PRIVATE HAS_EDITOR=1)

# Definiera HAS_* om bibliotek finns
if(imgui_FOUND)
    target_compile_definitions(RetroEditor PRIVATE HAS_IMGUI=1)
endif()
if(imguizmo_FOUND)
    target_compile_definitions(RetroEditor PRIVATE HAS_IMGUIZMO=1)
endif()
if(implot_FOUND)
    target_compile_definitions(RetroEditor PRIVATE HAS_IMPLOT=1)
endif()
if(nfd_FOUND)
    target_compile_definitions(RetroEditor PRIVATE HAS_NFD=1)
endif()

# ============================================================================
# POST BUILD - Kopiera assets
# ============================================================================
foreach(target RetroGame RetroEditor)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${target}>/assets
    )
endforeach()
