/**
 * @file PanelRegistry.cpp
 * @brief Implementation of centralized panel management
 */
#include "PanelRegistry.h"
#include "editor/EditorContext.h"
#include "editor/panels/HierarchyPanel.h"
#include "editor/panels/PropertiesPanel.h"
#include "editor/panels/ViewportPanel.h"
#include "editor/panels/AssetBrowserPanel.h"
#include "editor/panels/ConsolePanel.h"
#include "editor/panels/SceneGraphPanel.h"
#include "editor/panels/LayerEditorPanel.h"
#include "editor/panels/TileMapEditorPanel.h"
#include "editor/panels/WorldViewPanel.h"
#include "editor/panels/LevelViewPanel.h"
#include "editor/graphs/dialog/DialogGraphPanel.h"
#include "editor/graphs/quest/QuestGraphPanel.h"
#include "editor/graphs/npc/BehaviorGraphPanel.h"

PanelRegistry::PanelRegistry(EditorContext& context)
    : m_context(context) {
}

PanelRegistry::~PanelRegistry() = default;

void PanelRegistry::createAllPanels(SDL_Renderer* renderer) {
    // Core panels
    m_hierarchyPanel = std::make_unique<HierarchyPanel>(m_context);
    m_propertiesPanel = std::make_unique<PropertiesPanel>(m_context);
    m_viewportPanel = std::make_unique<ViewportPanel>(m_context);
    m_viewportPanel->setRenderer(renderer);
    
    // Asset and console
    m_assetBrowserPanel = std::make_unique<AssetBrowserPanel>(m_context);
    m_assetBrowserPanel->setRenderer(renderer);
    m_consolePanel = std::make_unique<ConsolePanel>(m_context);
    
    // Graph panels
    m_dialogGraphPanel = std::make_unique<DialogGraphPanel>(m_context);
    m_questGraphPanel = std::make_unique<QuestGraphPanel>(m_context);
    m_behaviorGraphPanel = std::make_unique<BehaviorGraphPanel>(m_context);
    
    // Scene/Layer panels
    m_sceneGraphPanel = std::make_unique<SceneGraphPanel>(m_context);
    m_layerEditorPanel = std::make_unique<LayerEditorPanel>(m_context);
    m_tileMapEditorPanel = std::make_unique<TileMapEditorPanel>(m_context);
    
    // World/Level view panels (hidden by default)
    m_worldViewPanel = std::make_unique<WorldViewPanel>(m_context);
    m_levelViewPanel = std::make_unique<LevelViewPanel>(m_context);
    m_worldViewPanel->setVisible(false);
    m_levelViewPanel->setVisible(false);
}

void PanelRegistry::setupHierarchy(engine::World* world, engine::Level* level, 
                                    engine::Scene* scene, engine::LayerManager* layerManager) {
    if (m_viewportPanel) {
        m_viewportPanel->setWorld(world);
        m_viewportPanel->setLevel(level);
        m_viewportPanel->setScene(scene);
    }
    if (m_sceneGraphPanel) {
        m_sceneGraphPanel->setScene(scene);
    }
    if (m_layerEditorPanel) {
        m_layerEditorPanel->setLayerManager(layerManager);
    }
    if (m_worldViewPanel) {
        m_worldViewPanel->setWorld(world);
    }
    if (m_levelViewPanel) {
        m_levelViewPanel->setLevel(level);
    }
}

void PanelRegistry::renderAll() {
    if (m_hierarchyPanel) m_hierarchyPanel->render();
    if (m_viewportPanel) m_viewportPanel->render();
    if (m_propertiesPanel) m_propertiesPanel->render();
    if (m_dialogGraphPanel) m_dialogGraphPanel->render();
    if (m_questGraphPanel) m_questGraphPanel->render();
    if (m_behaviorGraphPanel) m_behaviorGraphPanel->render();
    if (m_sceneGraphPanel) m_sceneGraphPanel->render();
    if (m_layerEditorPanel) m_layerEditorPanel->render();
    if (m_tileMapEditorPanel) m_tileMapEditorPanel->render();
    if (m_worldViewPanel) m_worldViewPanel->render();
    if (m_levelViewPanel) m_levelViewPanel->render();
    if (m_assetBrowserPanel) m_assetBrowserPanel->render();
    if (m_consolePanel) m_consolePanel->render();
}

void PanelRegistry::updateViewport(float deltaTime) {
    if (m_viewportPanel) {
        m_viewportPanel->update(deltaTime);
    }
}

void PanelRegistry::log(const std::string& message) {
    if (m_consolePanel) m_consolePanel->log(message);
}

void PanelRegistry::logWarning(const std::string& message) {
    if (m_consolePanel) m_consolePanel->logWarning(message);
}

void PanelRegistry::logError(const std::string& message) {
    if (m_consolePanel) m_consolePanel->logError(message);
}
