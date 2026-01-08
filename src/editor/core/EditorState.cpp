/**
 * @file EditorState.cpp
 * @brief Minimal EditorState using manager classes for separation of concerns
 */
#include "EditorState.h"
#include "game/Game.h"
#include "editor/managers/EditorPanelManager.h"
#include "editor/managers/EditorWorldManager.h"
#include "editor/managers/EditorEventDispatcher.h"
#include "editor/input/EditorInputHandler.h"
#include "editor/panels/core/HierarchyPanel.h"
#include "editor/panels/core/PropertiesPanel.h"
#include "editor/panels/core/ConsolePanel.h"
#include "editor/panels/core/CommandPanel.h"
#include "editor/panels/viewport/ViewportPanel.h"
#include "editor/viewport/ViewportPanelNew.h"
#include "editor/panels/assets/AssetBrowserPanel.h"
#include "editor/panels/assets/PlaceActorsPanel.h"
#include "editor/panels/world/WorldViewPanel.h"
#include "editor/panels/world/LevelViewPanel.h"
#include "editor/panels/world/SceneGraphPanel.h"
#include "editor/panels/editors/LayerEditorPanel.h"
#include "editor/panels/editors/TileMapEditorPanel.h"
#include "editor/panels/graphs/dialog/DialogGraphPanel.h"
#include "editor/panels/graphs/quest/QuestGraphPanel.h"
#include "editor/panels/graphs/npc/BehaviorGraphPanel.h"
#include "editor/ui/EditorMenuBar.h"
#include "editor/ui/EditorDockspace.h"
#include "editor/legacy/TiledIntegration.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"
#include "ai/AISystemInit.h"
#include "ai/ui/AIChatPanel.h"
#include "editor/panels/core/EditorSettingsPanel.h"
#include "editor/viewport/Viewport3DPanel.h"

#ifdef HAS_IMGUI
#include "editor/core/ImGuiManager.h"
#include <imgui.h>
#endif

EditorState::EditorState() 
    : m_visualRoomEditor(std::make_unique<VisualRoomEditor>()) {
    LOG_INFO("EditorState created");
}

void EditorState::enter() {
    LOG_INFO("EditorState::enter()");
    m_currentTab = EditorTab::Overview;
    m_scrollY = 0;
    
#ifdef HAS_IMGUI
    // Initialize ImGui
    if (m_game) {
        ImGuiManager::instance().init(m_game->getWindow(), m_game->getRenderer());
    }
    
    // Set editor state reference in context
    m_editorContext.setEditorState(this);
    
    // Create managers
    m_panelManager = std::make_unique<EditorPanelManager>(m_editorContext);
    m_panelManager->initialize(m_game);
    
    m_worldManager = std::make_unique<EditorWorldManager>(m_editorContext);
    m_worldManager->initialize(m_game);
    
    m_eventDispatcher = std::make_unique<EditorEventDispatcher>(m_editorContext);
    m_eventDispatcher->setInputHandler(m_panelManager->getInputHandler());
    
    // Create settings panel
    m_settingsPanel = std::make_unique<editor::EditorSettingsPanel>();
    
    // Setup menu bar callbacks
    auto* menuBar = m_panelManager->getMenuBar();
    menuBar->setSettingsPanel(m_settingsPanel.get());
    menuBar->onSaveAll = [this]() {
        m_dataManager.syncFromEngine(m_worldManager->getWorld());
        auto result = m_dataManager.saveAll();
        m_editorContext.saveToFiles();
        m_editorContext.isDirty = false;
        if (m_panelManager->getConsolePanel()) {
            m_panelManager->getConsolePanel()->log(
                result.success ? "Saved all data!" : "Save failed: " + result.errors[0]
            );
        }
    };
    
    menuBar->onImportTiled = [this]() { importFromTiled(); };
    menuBar->onExportTiled = [this]() { exportAllToTiled(); };
    menuBar->onExit = [this]() { if (m_game) m_game->popState(); };
    menuBar->onReloadData = [this]() {
        DataLoader::instance().loadAll();
        if (m_panelManager->getConsolePanel()) {
            m_panelManager->getConsolePanel()->log("Data reloaded");
        }
    };
    
    // Get viewport and selection manager FIRST
    auto* viewport = m_panelManager->getViewportPanel();
    auto* selectionManager = viewport->getSelectionManager();
    
    // Connect selection manager to all panels BEFORE setting navigation
    auto* properties = m_panelManager->getPropertiesPanel();
    properties->setSelectionManager(selectionManager);
    
    auto* hierarchy = m_panelManager->getHierarchyPanel();
    hierarchy->setSelectionManager(selectionManager);
    
    // Register navigation callback BEFORE setting world/level/scene
    selectionManager->registerNavigationChangedCallback([viewport]() {
        viewport->syncFromSelectionManager();
    });
    
    // NOW connect World/Level/Scene hierarchy to panels
    auto* world = m_worldManager->getWorld();
    viewport->setWorld(world);
    viewport->setLevel(world->getActiveLevel());
    viewport->setScene(world->getActiveLevel()->getActiveScene());
    
    // Connect new unified viewport
    if (auto* viewportNew = m_panelManager->getViewportPanelNew()) {
        viewportNew->setWorld(world);
    }
    
    m_panelManager->getSceneGraphPanel()->setScene(world->getActiveLevel()->getActiveScene());
    m_panelManager->getLayerEditorPanel()->setLayerManager(m_worldManager->getLayerManager());
    m_panelManager->getWorldViewPanel()->setWorld(world);
    m_panelManager->getLevelViewPanel()->setLevel(world->getActiveLevel());
    
    // Initialize AI system
    if (ai::initializeAISystem()) {
        if (m_panelManager->getConsolePanel()) {
            m_panelManager->getConsolePanel()->log("AI Assistant initialized");
        }
    }
    
    if (m_panelManager->getConsolePanel()) {
        m_panelManager->getConsolePanel()->log("Editor initialized with manager architecture");
        m_panelManager->getConsolePanel()->log("Converted " + 
            std::to_string(DataLoader::instance().getRooms().size()) + " rooms to scenes");
    }
    
    LOG_INFO("EditorState::enter() - ImGui initialized: " + std::string(ImGuiManager::instance().isInitialized() ? "YES" : "NO"));
    LOG_INFO("EditorState::enter() - PanelManager created: " + std::string(m_panelManager ? "YES" : "NO"));
    LOG_INFO("EditorState::enter() - WorldManager created: " + std::string(m_worldManager ? "YES" : "NO"));
#endif
    
    // Load data to EditorContext
    m_editorContext.loadFromDataLoader();
    LOG_INFO("EditorContext loaded with " + std::to_string(m_editorContext.dialogs.size()) + " dialogs");
    
    // Build room flowchart positions
    m_roomNodes.clear();
    const auto& rooms = DataLoader::instance().getRooms();
    int x = 50;
    int y = 100;
    int col = 0;
    
    for (const auto& room : rooms) {
        RoomNode node;
        node.id = room.id;
        node.x = x + col * 150;
        node.y = y;
        node.width = 120;
        node.height = 40;
        m_roomNodes.push_back(node);
        
        col++;
        if (col >= 4) {
            col = 0;
            y += 80;
        }
    }
}

void EditorState::exit() {
    LOG_INFO("EditorState::exit()");
    if (m_roomPreviewTexture) {
        SDL_DestroyTexture(m_roomPreviewTexture);
        m_roomPreviewTexture = nullptr;
    }
    
#ifdef HAS_IMGUI
    // Shutdown AI system
    ai::shutdownAISystem();
    
    if (m_panelManager) m_panelManager->shutdown();
    if (m_worldManager) m_worldManager->shutdown();
    ImGuiManager::instance().shutdown();
#endif
}

void EditorState::update(float deltaTime) {
    m_statusTimer = std::max(0.0f, m_statusTimer - deltaTime);
    
#ifdef HAS_IMGUI
    // Update panels
    if (m_panelManager && m_panelManager->getViewportPanel()) {
        m_panelManager->getViewportPanel()->update(deltaTime);
    }
    
    // Update AI system (check for async responses)
    ai::AIAgentSystem::instance().update();
#endif
}

void EditorState::render(SDL_Renderer* renderer) {
#ifdef HAS_IMGUI
    static bool firstRender = true;
    if (firstRender) {
        LOG_INFO("EditorState::render() - First render call");
        firstRender = false;
    }
    
    // Use full window for editor (no viewport restriction)
    SDL_RenderSetViewport(renderer, nullptr);
    
    renderImGui();
#endif
}

void EditorState::handleEvent(const SDL_Event& event) {
    if (m_eventDispatcher) {
        m_eventDispatcher->handleEvent(event);
    }
}

void EditorState::importFromTiled() {
    LOG_INFO("EditorState: Import from Tiled requested");
    if (m_panelManager && m_panelManager->getConsolePanel()) {
        m_panelManager->getConsolePanel()->log("Tiled import not yet implemented");
    }
}

void EditorState::exportAllToTiled() {
    LOG_INFO("EditorState: Export to Tiled requested");
    if (m_panelManager && m_panelManager->getConsolePanel()) {
        m_panelManager->getConsolePanel()->log("Tiled export not yet implemented");
    }
}

void EditorState::renderImGui() {
#ifdef HAS_IMGUI
    static bool firstImGuiRender = true;
    if (firstImGuiRender) {
        LOG_INFO("EditorState::renderImGui() - First ImGui render");
        firstImGuiRender = false;
    }
    
    if (!m_panelManager) {
        LOG_WARNING("EditorState::renderImGui() - m_panelManager is null!");
        return;
    }
    
    if (!ImGuiManager::instance().isInitialized()) {
        LOG_WARNING("EditorState::renderImGui() - ImGui not initialized!");
        return;
    }
    
    ImGuiManager::instance().beginFrame();
    
    // Render menu bar FIRST (before dockspace)
    if (m_panelManager->getMenuBar()) {
        m_panelManager->getMenuBar()->render();
    }
    
    // Render dockspace - use actual window size
    if (m_panelManager->getDockspace() && m_game) {
        float screenW = static_cast<float>(m_game->getScreenWidth());
        float screenH = static_cast<float>(m_game->getScreenHeight());
        m_panelManager->getDockspace()->render(20.0f, screenW, screenH);
    }
    
    // Render all panels (inside dockspace)
    if (m_panelManager->getHierarchyPanel() && m_panelManager->getHierarchyPanel()->isVisible()) {
        m_panelManager->getHierarchyPanel()->render();
    }
    if (m_panelManager->getViewportPanel() && m_panelManager->getViewportPanel()->isVisible()) {
        m_panelManager->getViewportPanel()->render();
    }
    if (m_panelManager->getPropertiesPanel() && m_panelManager->getPropertiesPanel()->isVisible()) {
        m_panelManager->getPropertiesPanel()->render();
    }
    if (m_panelManager->getAssetBrowserPanel() && m_panelManager->getAssetBrowserPanel()->isVisible()) {
        m_panelManager->getAssetBrowserPanel()->render();
    }
    if (m_panelManager->getPlaceActorsPanel() && m_panelManager->getPlaceActorsPanel()->isVisible()) {
        m_panelManager->getPlaceActorsPanel()->render();
    }
    if (m_panelManager->getConsolePanel() && m_panelManager->getConsolePanel()->isVisible()) {
        m_panelManager->getConsolePanel()->render();
    }
    if (m_panelManager->getCommandPanel() && m_panelManager->getCommandPanel()->isVisible()) {
        m_panelManager->getCommandPanel()->render();
    }
    if (m_panelManager->getDialogGraphPanel() && m_panelManager->getDialogGraphPanel()->isVisible()) {
        m_panelManager->getDialogGraphPanel()->render();
    }
    if (m_panelManager->getQuestGraphPanel() && m_panelManager->getQuestGraphPanel()->isVisible()) {
        m_panelManager->getQuestGraphPanel()->render();
    }
    if (m_panelManager->getBehaviorGraphPanel() && m_panelManager->getBehaviorGraphPanel()->isVisible()) {
        m_panelManager->getBehaviorGraphPanel()->render();
    }
    if (m_panelManager->getSceneGraphPanel() && m_panelManager->getSceneGraphPanel()->isVisible()) {
        m_panelManager->getSceneGraphPanel()->render();
    }
    if (m_panelManager->getLayerEditorPanel() && m_panelManager->getLayerEditorPanel()->isVisible()) {
        m_panelManager->getLayerEditorPanel()->render();
    }
    if (m_panelManager->getTileMapEditorPanel() && m_panelManager->getTileMapEditorPanel()->isVisible()) {
        m_panelManager->getTileMapEditorPanel()->render();
    }
    if (m_panelManager->getWorldViewPanel() && m_panelManager->getWorldViewPanel()->isVisible()) {
        m_panelManager->getWorldViewPanel()->render();
    }
    if (m_panelManager->getLevelViewPanel() && m_panelManager->getLevelViewPanel()->isVisible()) {
        m_panelManager->getLevelViewPanel()->render();
    }
    
    // Render AI Chat Panel
    ai::AIChatPanel::instance().render();
    
    // Render Settings Panel (modal)
    if (m_settingsPanel) {
        m_settingsPanel->render();
    }
    
    ImGuiManager::instance().endFrame();
#endif
}
