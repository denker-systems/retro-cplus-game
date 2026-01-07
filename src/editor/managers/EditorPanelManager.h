/**
 * @file EditorPanelManager.h
 * @brief Manages creation and lifecycle of all editor panels
 */
#pragma once

#include <memory>

// Forward declarations
class Game;
class EditorContext;
class HierarchyPanel;
class PropertiesPanel;
class ViewportPanel;
class AssetBrowserPanel;
class PlaceActorsPanel;
class ConsolePanel;
class DialogGraphPanel;
class QuestGraphPanel;
class BehaviorGraphPanel;
class SceneGraphPanel;
class LayerEditorPanel;
class TileMapEditorPanel;
class WorldViewPanel;
class LevelViewPanel;
class EditorInputHandler;
class EditorMenuBar;
class EditorDockspace;

namespace engine {
    class LayerManager;
}

/**
 * @brief Manages all editor panels and UI components
 */
class EditorPanelManager {
public:
    explicit EditorPanelManager(EditorContext& context);
    ~EditorPanelManager() = default;
    
    void initialize(Game* game);
    void shutdown();
    
    // Panel access
    HierarchyPanel* getHierarchyPanel() const { return m_hierarchyPanel.get(); }
    PropertiesPanel* getPropertiesPanel() const { return m_propertiesPanel.get(); }
    ViewportPanel* getViewportPanel() const { return m_viewportPanel.get(); }
    AssetBrowserPanel* getAssetBrowserPanel() const { return m_assetBrowserPanel.get(); }
    PlaceActorsPanel* getPlaceActorsPanel() const { return m_placeActorsPanel.get(); }
    ConsolePanel* getConsolePanel() const { return m_consolePanel.get(); }
    DialogGraphPanel* getDialogGraphPanel() const { return m_dialogGraphPanel.get(); }
    QuestGraphPanel* getQuestGraphPanel() const { return m_questGraphPanel.get(); }
    BehaviorGraphPanel* getBehaviorGraphPanel() const { return m_behaviorGraphPanel.get(); }
    SceneGraphPanel* getSceneGraphPanel() const { return m_sceneGraphPanel.get(); }
    LayerEditorPanel* getLayerEditorPanel() const { return m_layerEditorPanel.get(); }
    TileMapEditorPanel* getTileMapEditorPanel() const { return m_tileMapEditorPanel.get(); }
    WorldViewPanel* getWorldViewPanel() const { return m_worldViewPanel.get(); }
    LevelViewPanel* getLevelViewPanel() const { return m_levelViewPanel.get(); }
    EditorInputHandler* getInputHandler() const { return m_inputHandler.get(); }
    EditorMenuBar* getMenuBar() const { return m_menuBar.get(); }
    EditorDockspace* getDockspace() const { return m_dockspace.get(); }
    
private:
    EditorContext& m_context;
    
    // Panels
    std::unique_ptr<HierarchyPanel> m_hierarchyPanel;
    std::unique_ptr<PropertiesPanel> m_propertiesPanel;
    std::unique_ptr<ViewportPanel> m_viewportPanel;
    std::unique_ptr<AssetBrowserPanel> m_assetBrowserPanel;
    std::unique_ptr<PlaceActorsPanel> m_placeActorsPanel;
    std::unique_ptr<ConsolePanel> m_consolePanel;
    std::unique_ptr<DialogGraphPanel> m_dialogGraphPanel;
    std::unique_ptr<QuestGraphPanel> m_questGraphPanel;
    std::unique_ptr<BehaviorGraphPanel> m_behaviorGraphPanel;
    std::unique_ptr<SceneGraphPanel> m_sceneGraphPanel;
    std::unique_ptr<LayerEditorPanel> m_layerEditorPanel;
    std::unique_ptr<TileMapEditorPanel> m_tileMapEditorPanel;
    std::unique_ptr<WorldViewPanel> m_worldViewPanel;
    std::unique_ptr<LevelViewPanel> m_levelViewPanel;
    
    // Input handler
    std::unique_ptr<EditorInputHandler> m_inputHandler;
    
    // UI components
    std::unique_ptr<EditorMenuBar> m_menuBar;
    std::unique_ptr<EditorDockspace> m_dockspace;
};
