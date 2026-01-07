/**
 * @file EditorState.cpp
 * @brief Minimal EditorState using manager classes for separation of concerns
 */
#include "EditorState.h"
#include "game/Game.h"
#include "editor/managers/EditorPanelManager.h"
#include "editor/managers/EditorWorldManager.h"
#include "editor/managers/EditorEventDispatcher.h"
#include "editor/panels/ViewportPanel.h"
#include "editor/panels/ConsolePanel.h"
#include "editor/ui/EditorMenuBar.h"
#include "editor/ui/EditorDockspace.h"
#include "editor/components/TiledIntegration.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"

#ifdef HAS_IMGUI
#include "editor/ImGuiManager.h"
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
    
    // Create managers
    m_panelManager = std::make_unique<EditorPanelManager>(m_editorContext);
    m_panelManager->initialize(m_game);
    
    m_worldManager = std::make_unique<EditorWorldManager>(m_editorContext);
    m_worldManager->initialize(m_game);
    
    m_eventDispatcher = std::make_unique<EditorEventDispatcher>(m_editorContext);
    m_eventDispatcher->setInputHandler(m_panelManager->getInputHandler());
    
    // Setup menu bar callbacks
    auto* menuBar = m_panelManager->getMenuBar();
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
    
    // Connect World/Level/Scene hierarchy to panels
    auto* viewport = m_panelManager->getViewportPanel();
    auto* world = m_worldManager->getWorld();
    viewport->setWorld(world);
    viewport->setLevel(world->getActiveLevel());
    viewport->setScene(world->getActiveLevel()->getActiveScene());
    
    m_panelManager->getSceneGraphPanel()->setScene(world->getActiveLevel()->getActiveScene());
    m_panelManager->getLayerEditorPanel()->setLayerManager(m_worldManager->getLayerManager());
    m_panelManager->getWorldViewPanel()->setWorld(world);
    m_panelManager->getLevelViewPanel()->setLevel(world->getActiveLevel());
    
    if (m_panelManager->getConsolePanel()) {
        m_panelManager->getConsolePanel()->log("Editor initialized with manager architecture");
        m_panelManager->getConsolePanel()->log("Converted " + 
            std::to_string(DataLoader::instance().getRooms().size()) + " rooms to scenes");
    }
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
#endif
}

void EditorState::render(SDL_Renderer* renderer) {
#ifdef HAS_IMGUI
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
    ImGuiManager::instance().newFrame();
    
    // Render dockspace
    if (m_panelManager && m_panelManager->getDockspace()) {
        m_panelManager->getDockspace()->begin();
    }
    
    // Render menu bar
    if (m_panelManager && m_panelManager->getMenuBar()) {
        m_panelManager->getMenuBar()->render();
    }
    
    // Render all panels
    if (m_panelManager) {
        if (m_panelManager->getHierarchyPanel()) m_panelManager->getHierarchyPanel()->render();
        if (m_panelManager->getViewportPanel()) m_panelManager->getViewportPanel()->render();
        if (m_panelManager->getPropertiesPanel()) m_panelManager->getPropertiesPanel()->render();
        if (m_panelManager->getAssetBrowserPanel()) m_panelManager->getAssetBrowserPanel()->render();
        if (m_panelManager->getPlaceActorsPanel()) m_panelManager->getPlaceActorsPanel()->render();
        if (m_panelManager->getConsolePanel()) m_panelManager->getConsolePanel()->render();
        if (m_panelManager->getDialogGraphPanel()) m_panelManager->getDialogGraphPanel()->render();
        if (m_panelManager->getQuestGraphPanel()) m_panelManager->getQuestGraphPanel()->render();
        if (m_panelManager->getBehaviorGraphPanel()) m_panelManager->getBehaviorGraphPanel()->render();
        if (m_panelManager->getSceneGraphPanel()) m_panelManager->getSceneGraphPanel()->render();
        if (m_panelManager->getLayerEditorPanel()) m_panelManager->getLayerEditorPanel()->render();
        if (m_panelManager->getTileMapEditorPanel()) m_panelManager->getTileMapEditorPanel()->render();
        if (m_panelManager->getWorldViewPanel()) m_panelManager->getWorldViewPanel()->render();
        if (m_panelManager->getLevelViewPanel()) m_panelManager->getLevelViewPanel()->render();
    }
    
    // End dockspace
    if (m_panelManager && m_panelManager->getDockspace()) {
        m_panelManager->getDockspace()->end();
    }
    
    ImGuiManager::instance().render();
#endif
}
