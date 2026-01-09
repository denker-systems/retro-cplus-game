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
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(httplib CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)

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
message(STATUS "GLEW found: ${GLEW_FOUND}")
message(STATUS "Assimp found: ${assimp_FOUND}")

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
    src/engine/components/TriggerComponent.cpp
    src/engine/physics/box2d/PhysicsLoader.cpp
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
    src/engine/actors/NPC3DActor.cpp
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
    src/engine/world/RoomToSceneConverter.cpp
    src/engine/world/WorldContainer.h
    src/engine/physics/CollisionShape.cpp
    src/engine/physics/PhysicsBody.cpp
    src/engine/physics/KinematicBody.cpp
    src/engine/physics/SpatialGrid.cpp
    src/engine/physics/box2d/PhysicsWorld2D.cpp
    src/engine/physics/physx/PhysicsWorld3D.cpp
    src/engine/components/RigidBody3DComponent.cpp
    src/engine/components/MeshComponent.cpp
    src/engine/actors/StaticMeshActor.cpp
    src/engine/actors/CameraActor.cpp
    src/engine/actors/LightActor.cpp
    src/engine/actors/PlayerStartActor.cpp
    src/engine/actors/PlayerConfigActor.cpp
    src/engine/actors/Character3DActor.cpp
    src/engine/components/CharacterController3DComponent.cpp
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
    src/engine/graphics/GLContext.cpp
    src/engine/graphics/Framebuffer.cpp
    src/engine/graphics/Shader.cpp
    src/engine/graphics/Mesh.cpp
    src/engine/graphics/GLTextureManager.cpp
    src/engine/audio/AudioManager.cpp
    src/engine/entities/Entity.cpp
    # src/engine/entities/Character.cpp  # DEPRECATED: Use CharacterActor
    # src/engine/entities/PlayerCharacter.cpp  # DEPRECATED: Use PlayerActor
    # src/engine/entities/NPC.cpp  # Migrated to src/engine/actors/NPC.cpp
    src/engine/ui/Widget.cpp
    src/engine/utils/Logger.cpp
    src/engine/utils/FileWatcher.cpp
    src/engine/utils/IconLoader.cpp
    src/engine/build/CMakeGenerator.cpp
    src/engine/build/BuildManager.cpp
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
    GLEW::GLEW
    OpenGL::GL
    unofficial::omniverse-physx-sdk::sdk
)

# Hot Reload alltid tillgängligt (polling-baserat, ingen extern dependency)
message(STATUS "Hot Reload enabled (polling-based FileWatcher)")

# ============================================================================
# RETRO GAME - Runtime Player (NEW - Ren 3D från grunden)
# ============================================================================
# Legacy /game folder är DEPRECATED - använder nya /runtime istället
set(RUNTIME_SOURCES
    src/runtime/main.cpp
    src/runtime/RuntimeApp.cpp
    src/runtime/RuntimeWorld.cpp
    src/runtime/RuntimeRenderer.cpp
    src/editor/viewport/3d/EditorCamera3D.cpp
)

# Windows icon resources
if(WIN32)
    list(APPEND RUNTIME_SOURCES src/game/RetroGame.rc)
endif()

add_executable(RetroGame ${RUNTIME_SOURCES})
target_link_libraries(RetroGame PRIVATE
    RetroCore
    SDL2::SDL2
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    OpenGL::GL
    GLEW::GLEW
    unofficial::omniverse-physx-sdk::sdk
)

# ============================================================================
# RETRO EDITOR - Standalone editor executable
# ============================================================================
# ImGui SDL2 backend (vendored)
set(IMGUI_BACKEND_SOURCES
    src/vendor/imgui/imgui_impl_sdl2.cpp
    src/vendor/imgui/imgui_impl_sdlrenderer2.cpp
    src/vendor/imgui/imgui_impl_opengl3.cpp
)

set(EDITOR_SOURCES
    src/game/Game.cpp
    # Legacy states - Still needed by Game.cpp (TODO: Refactor Game.cpp to not need these)
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
    src/editor/core/EditorPlayMode.cpp
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
    src/editor/panels/core/EditorSettingsPanel.cpp
    src/editor/panels/core/BuildPanel.cpp
    
    # Viewport - Main Panel
    src/editor/viewport/ViewportPanel.cpp
    
    # Viewport - Core Base Classes
    src/editor/viewport/core/BaseViewRenderer.cpp
    src/editor/viewport/core/Base3DRenderer.cpp
    src/editor/viewport/core/Base2DRenderer.cpp
    
    # Viewport - 3D Renderers
    src/editor/viewport/renderers/3d/World3DRenderer.cpp
    src/editor/viewport/renderers/3d/Level3DRenderer.cpp
    src/editor/viewport/renderers/3d/Scene3DRenderer.cpp
    
    # Viewport - 2D Renderers
    src/editor/viewport/renderers/2d/World2DRenderer.cpp
    src/editor/viewport/renderers/2d/Level2DRenderer.cpp
    src/editor/viewport/renderers/2d/Scene2DRenderer.cpp
    
    # Viewport - UI Components
    src/editor/viewport/ui/ViewportToolbar.cpp
    src/editor/viewport/ui/ViewportBreadcrumbs.cpp
    src/editor/viewport/ui/ViewportActorRenderer.cpp
    
    # Viewport - Input
    src/editor/viewport/input/ViewportInputHandler.cpp
    
    # Viewport - 3D Support
    src/editor/viewport/3d/EditorCamera3D.cpp
    src/editor/viewport/3d/Viewport3DPanel.cpp
    src/editor/gizmos/TransformGizmo3D.cpp
    
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
    src/editor/panels/editors/ActorDetailsPanel.cpp
    
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
    
    # Properties - Actors
    src/editor/properties/actors/ActorPropertyEditor.cpp
    src/editor/properties/actors/LockableComponent.cpp
    
    # Core
    src/editor/core/SelectionManager.cpp
    
    # Tools
    src/editor/tools/SelectTool.cpp
    src/editor/tools/ScaleTool.cpp
    src/editor/tools/ToolManager.cpp
    
    # UI
    src/editor/ui/EditorMenuBar.cpp
    src/editor/ui/EditorDockspace.cpp
    src/editor/ui/EditorTheme.cpp
    
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
    
    # AI System
    src/ai/core/EditorToolRegistry.cpp
    src/ai/core/AIAgentSystem.cpp
    src/ai/providers/OpenAIProvider.cpp
    src/ai/providers/AnthropicProvider.cpp
    src/ai/tools/SceneTools.cpp
    src/ai/tools/ActorTools.cpp
    src/ai/tools/HotspotTools.cpp
    src/ai/tools/DialogTools.cpp
    src/ai/tools/ItemTools.cpp
    src/ai/tools/CommandTools.cpp
    src/ai/tools/ContextTools.cpp
    src/ai/tools/LevelTools.cpp
    src/ai/tools/QuestTools.cpp
    src/editor/panels/core/CommandPanel.cpp
    src/ai/ui/AIChatPanel.cpp
    src/ai/AISystemInit.cpp
    
    # Vendor
    src/vendor/imnodes/imnodes.cpp
    src/editor/core/ImGuiManager.cpp
    ${IMGUI_BACKEND_SOURCES}
)

# Windows icon resources
if(WIN32)
    list(APPEND EDITOR_SOURCES src/editor/RetroEditor.rc)
endif()

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
    httplib::httplib
    OpenSSL::SSL
    OpenSSL::Crypto
    assimp::assimp
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
# RETRO LAUNCHER - Project launcher and manager
# ============================================================================
set(LAUNCHER_SOURCES
    src/launcher/LauncherApp.cpp
    # Project
    src/launcher/project/ProjectManager.cpp
    src/launcher/project/ProjectService.cpp
    # UI
    src/launcher/ui/LauncherHeader.cpp
    src/launcher/ui/ProjectGrid.cpp
    src/launcher/ui/NewProjectDialog.cpp
    # Core
    src/launcher/core/LauncherConfig.cpp
    # Editor theme
    src/editor/ui/EditorTheme.cpp
    ${IMGUI_BACKEND_SOURCES}
)

# Windows icon resources
if(WIN32)
    list(APPEND LAUNCHER_SOURCES src/launcher/RetroLauncher.rc)
endif()

add_executable(RetroLauncher src/launcher/main.cpp ${LAUNCHER_SOURCES})
target_include_directories(RetroLauncher PRIVATE src src/vendor)
target_link_libraries(RetroLauncher PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    nlohmann_json::nlohmann_json
    $<TARGET_NAME_IF_EXISTS:imgui::imgui>
)

# Definiera HAS_IMGUI
if(imgui_FOUND)
    target_compile_definitions(RetroLauncher PRIVATE HAS_IMGUI=1)
endif()

# Copy launcher logo to build directory
add_custom_command(TARGET RetroLauncher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/src/launcher/retroengine.png
    $<TARGET_FILE_DIR:RetroLauncher>/retroengine.png
)

# ============================================================================
# POST BUILD - Kopiera assets
# ============================================================================
foreach(target RetroGame RetroEditor)
    add_custom_command(TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${target}>/assets
    )
endforeach()
