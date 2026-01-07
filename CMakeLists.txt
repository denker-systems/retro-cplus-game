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
find_package(box2d CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

# Editor dependencies (optional)
find_package(imgui CONFIG QUIET)
find_package(imguizmo CONFIG QUIET)
find_package(implot CONFIG QUIET)
# imnodes is included locally in src/vendor/imnodes/
find_package(nfd CONFIG QUIET)

# Dev dependencies (optional)
find_package(spdlog CONFIG QUIET)
# efsw removed - using polling-based FileWatcher instead (more robust)

# Report what we found
message(STATUS "ImGui found: ${imgui_FOUND}")
message(STATUS "ImGuizmo found: ${imguizmo_FOUND}")
message(STATUS "ImPlot found: ${implot_FOUND}")
message(STATUS "ImNodes: Using local copy in src/vendor/imnodes/")
message(STATUS "NativeFileDialog found: ${nfd_FOUND}")

# ============================================================================
# RETRO CORE LIBRARY - Delad kod mellan Game och Editor
# ============================================================================
set(CORE_SOURCES
    src/engine/core/Object.cpp
    src/engine/core/ActorObject.cpp
    # LEGACY - Node system removed
    # src/engine/core/Node.cpp
    # src/engine/core/Node2D.cpp
    src/engine/core/ActorComponent.cpp
    src/engine/core/MigrationConfig.cpp
    src/engine/core/ActorObjectExtended.cpp
    src/engine/components/SpriteComponent.cpp
    src/engine/components/AnimationComponent.cpp
    src/engine/components/MovementComponent.cpp
    src/engine/components/HealthComponent.cpp
    src/engine/components/DialogComponent.cpp
    src/engine/components/CollisionComponent.cpp
    src/engine/components/InteractionComponent.cpp
    src/engine/components/QuestComponent.cpp
    src/engine/components/QuestGiverComponent.cpp
    src/engine/components/InventoryComponent.cpp
    src/engine/components/PathfindingComponent.cpp
    src/engine/components/PerceptionComponent.cpp
    src/engine/components/CameraComponent.cpp
    src/engine/components/PhysicsComponent.cpp
    src/engine/components/RigidBody2DComponent.cpp
    src/engine/components/Collider2DComponent.cpp
    src/engine/components/CharacterController2D.cpp
    src/engine/actors/Pawn.cpp
    src/engine/actors/Controller.cpp
    src/engine/actors/PlayerController.cpp
    src/engine/actors/AIController.cpp
    src/engine/actors/ItemActor.cpp
    src/engine/actors/VisualActor.cpp
    src/engine/actors/InteractiveActor.cpp
    src/engine/actors/CharacterActor.cpp
    src/engine/actors/EnvironmentActor.cpp
    src/engine/actors/SpriteActor.cpp
    src/engine/actors/NPC.cpp
    # LEGACY NODES - PARTIALLY DISABLED (Node system replaced by Actor system)
    # src/engine/nodes/Sprite.cpp
    # src/engine/nodes/AnimatedSprite.cpp
    src/engine/nodes/TileMapLayer.cpp  # Needed by editor
    # src/engine/nodes/ParallaxLayer.cpp
    # src/engine/nodes/Label.cpp
    # src/engine/nodes/InteractiveArea.cpp
    # src/engine/nodes/WalkArea.cpp
    # src/engine/nodes/Marker.cpp
    src/engine/world/Camera2D.cpp
    src/engine/world/Scene.cpp
    src/engine/world/World.cpp
    src/engine/world/Level.cpp
    src/engine/world/WorldContainer.h
    src/engine/physics/CollisionShape.cpp
    src/engine/physics/PhysicsBody.cpp
    src/engine/physics/KinematicBody.cpp
    src/engine/physics/SpatialGrid.cpp
    src/engine/physics/box2d/PhysicsWorld2D.cpp
    src/engine/Renderer.cpp
    src/engine/input/Input.cpp
    src/engine/Room.cpp
    src/engine/VideoSettings.cpp
    src/engine/systems/DialogSystem.cpp
    src/engine/systems/InventorySystem.cpp
    src/engine/systems/RoomManager.cpp
    src/engine/systems/SceneManager.cpp
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
    # src/engine/entities/NPC.cpp  # Migrated to src/engine/actors/NPC.cpp
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
    box2d::box2d
    glm::glm
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
    # Core
    src/editor/core/EditorState.cpp
    src/editor/core/EditorContext.cpp
    src/editor/core/EditorCore.cpp
    src/editor/core/EditorApp.cpp
    src/editor/core/CommandManager.cpp
    
    # Commands
    src/editor/commands/HotspotCommands.cpp
    src/editor/commands/ActorCommands.cpp
    
    # Legacy
    src/editor/legacy/FileBrowser.cpp
    src/editor/legacy/DebugEditor.cpp
    src/editor/legacy/VisualRoomEditor.cpp
    src/editor/legacy/RoomDataManager.cpp
    src/editor/legacy/EditorTabRenderer.cpp
    src/editor/legacy/TiledIntegration.cpp
    
    # Panels - Core
    src/editor/panels/core/HierarchyPanel.cpp
    src/editor/panels/core/PropertiesPanel.cpp
    src/editor/panels/core/ConsolePanel.cpp
    src/editor/panels/core/GameSettingsPanel.cpp
    
    # Panels - Viewport
    src/editor/panels/viewport/ViewportPanel_Core.cpp
    src/editor/panels/viewport/ViewportPanel_World.cpp
    src/editor/panels/viewport/ViewportPanel_Level.cpp
    src/editor/panels/viewport/ViewportPanel_Scene.cpp
    src/editor/panels/viewport/ViewportPanel_Actors.cpp
    src/editor/panels/viewport/ViewportPanel_Input.cpp
    
    # Panels - Assets
    src/editor/panels/assets/AssetBrowserPanel.cpp
    src/editor/panels/assets/PlaceActorsPanel.cpp
    
    # Panels - World
    src/editor/panels/world/WorldViewPanel.cpp
    src/editor/panels/world/LevelViewPanel.cpp
    src/editor/panels/world/SceneGraphPanel.cpp
    
    # Panels - Editors
    src/editor/panels/editors/LayerEditorPanel.cpp
    src/editor/panels/editors/TileMapEditorPanel.cpp
    
    # Panels - Graphs
    src/editor/panels/graphs/INodeGraphPanel.cpp
    src/editor/panels/graphs/dialog/DialogNode.cpp
    src/editor/panels/graphs/dialog/DialogGraphPanel.cpp
    src/editor/panels/graphs/quest/QuestNode.cpp
    src/editor/panels/graphs/quest/QuestGraphPanel.cpp
    src/editor/panels/graphs/npc/BehaviorNode.cpp
    src/editor/panels/graphs/npc/BehaviorGraphPanel.cpp
    
    # Properties - Core
    src/editor/properties/core/PropertyEditorUtils.cpp
    
    # Properties - World
    src/editor/properties/world/RoomPropertyEditor.cpp
    src/editor/properties/world/HotspotPropertyEditor.cpp
    
    # Properties - Gameplay
    src/editor/properties/gameplay/DialogPropertyEditor.cpp
    src/editor/properties/gameplay/QuestPropertyEditor.cpp
    src/editor/properties/gameplay/ItemPropertyEditor.cpp
    
    # Properties - Characters
    src/editor/properties/characters/NPCPropertyEditor.cpp
    
    # Tools
    src/editor/tools/SelectTool.cpp
    src/editor/tools/ScaleTool.cpp
    src/editor/tools/ToolManager.cpp
    
    # UI
    src/editor/ui/EditorMenuBar.cpp
    src/editor/ui/EditorDockspace.cpp
    
    # Input
    src/editor/input/EditorInputHandler.cpp
    
    # Managers
    src/editor/managers/EditorPanelManager.cpp
    src/editor/managers/EditorWorldManager.cpp
    src/editor/managers/EditorEventDispatcher.cpp
    
    # Data
    src/editor/data/ISerializer.cpp
    src/editor/data/WorldSerializer.cpp
    src/editor/data/SceneSerializer.cpp
    src/editor/data/EditorDataManager.cpp
    
    # Vendor
    src/vendor/imnodes/imnodes.cpp
    src/editor/core/ImGuiManager.cpp
    ${IMGUI_BACKEND_SOURCES}
)

add_executable(RetroEditor src/editor/core/main.cpp ${EDITOR_SOURCES})
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
# imnodes is always available (local copy)
target_compile_definitions(RetroEditor PRIVATE HAS_IMNODES=1)
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
