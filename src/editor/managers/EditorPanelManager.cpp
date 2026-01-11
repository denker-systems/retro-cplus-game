/**
 * @file EditorPanelManager.cpp
 * @brief Panel manager implementation
 */
#include "EditorPanelManager.h"
#include "game/Game.h"
#include "editor/core/EditorContext.h"
#include "editor/panels/core/HierarchyPanel.h"
#include "editor/panels/core/PropertiesPanel.h"
#include "editor/viewport/ViewportPanel.h"
#include "editor/panels/assets/AssetBrowserPanel.h"
#include "editor/panels/assets/PlaceActorsPanel.h"
#include "editor/panels/core/ConsolePanel.h"
#include "editor/panels/core/CommandPanel.h"
#include "editor/panels/core/BuildPanel.h"
#include "editor/panels/world/SceneGraphPanel.h"
#include "editor/panels/editors/LayerEditorPanel.h"
#include "editor/panels/editors/TileMapEditorPanel.h"
#include "editor/panels/world/WorldViewPanel.h"
#include "editor/panels/world/LevelViewPanel.h"
#include "editor/panels/editors/ActorDetailsPanel.h"
#include "editor/panels/graphs/dialog/DialogGraphPanel.h"
#include "editor/panels/graphs/quest/QuestGraphPanel.h"
#include "editor/panels/graphs/npc/BehaviorGraphPanel.h"
#include "editor/panels/gameplay/QuestLogPanel.h"
#include "editor/input/EditorInputHandler.h"
#include "editor/ui/EditorMenuBar.h"
#include "editor/ui/EditorDockspace.h"

EditorPanelManager::EditorPanelManager(EditorContext& context)
    : m_context(context) {
}

void EditorPanelManager::initialize(Game* game) {
    // Create panels
    m_hierarchyPanel = std::make_unique<HierarchyPanel>(m_context);
    m_propertiesPanel = std::make_unique<PropertiesPanel>(m_context);
    m_viewportPanel = std::make_unique<editor::viewport::ViewportPanel>(m_context);
    m_assetBrowserPanel = std::make_unique<AssetBrowserPanel>(m_context);
    m_assetBrowserPanel->setRenderer(game->getRenderer());
    m_placeActorsPanel = std::make_unique<PlaceActorsPanel>(m_context);
    m_placeActorsPanel->setRenderer(game->getRenderer());
    m_consolePanel = std::make_unique<ConsolePanel>(m_context);
    m_commandPanel = std::make_unique<CommandPanel>(m_context);
    m_buildPanel = std::make_unique<BuildPanel>(m_context);
    m_dialogGraphPanel = std::make_unique<DialogGraphPanel>(m_context);
    m_questGraphPanel = std::make_unique<QuestGraphPanel>(m_context);
    m_behaviorGraphPanel = std::make_unique<BehaviorGraphPanel>(m_context);
    m_sceneGraphPanel = std::make_unique<SceneGraphPanel>(m_context);
    m_layerEditorPanel = std::make_unique<LayerEditorPanel>(m_context);
    m_tileMapEditorPanel = std::make_unique<TileMapEditorPanel>(m_context);
    m_worldViewPanel = std::make_unique<WorldViewPanel>(m_context);
    m_levelViewPanel = std::make_unique<LevelViewPanel>(m_context);
    m_actorDetailsPanel = std::make_unique<ActorDetailsPanel>(m_context);
    m_questLogPanel = std::make_unique<QuestLogPanel>(m_context);
    
    // Create input handler
    m_inputHandler = std::make_unique<EditorInputHandler>(m_context);
    m_inputHandler->setViewportPanel(m_viewportPanel.get());
    
    // Create UI components
    m_menuBar = std::make_unique<EditorMenuBar>(m_context);
    m_dockspace = std::make_unique<EditorDockspace>();
    
    // Setup menu bar panel references
    m_menuBar->setPanels(
        m_hierarchyPanel.get(),
        m_viewportPanel.get(),
        m_propertiesPanel.get(),
        m_assetBrowserPanel.get(),
        m_placeActorsPanel.get(),
        m_consolePanel.get(),
        m_buildPanel.get(),
        m_dialogGraphPanel.get(),
        m_questGraphPanel.get(),
        m_behaviorGraphPanel.get()
    );
    
    // Hide World/Level View panels by default (integrated in Viewport now)
    m_worldViewPanel->setVisible(false);
    m_levelViewPanel->setVisible(false);
}

void EditorPanelManager::shutdown() {
    // Panels will be automatically destroyed via unique_ptr
}
