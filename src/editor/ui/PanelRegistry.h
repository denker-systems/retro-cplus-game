/**
 * @file PanelRegistry.h
 * @brief Manages creation, lifecycle and rendering of all editor panels
 */
#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>

// Forward declarations
class EditorContext;
class IEditorPanel;
class HierarchyPanel;
class PropertiesPanel;
class ViewportPanel;
class AssetBrowserPanel;
class ConsolePanel;
class DialogGraphPanel;
class QuestGraphPanel;
class BehaviorGraphPanel;
class SceneGraphPanel;
class LayerEditorPanel;
class TileMapEditorPanel;
class WorldViewPanel;
class LevelViewPanel;
struct SDL_Renderer;

namespace editor {
    class GameSettingsPanel;
}

namespace engine {
    class World;
    class Level;
    class Scene;
    class LayerManager;
}

/**
 * @brief Centralized panel management for the editor
 * 
 * Handles creation, initialization, and rendering of all ImGui panels.
 * Provides access to individual panels for callbacks and state queries.
 */
class PanelRegistry {
public:
    explicit PanelRegistry(EditorContext& context);
    ~PanelRegistry();
    
    // Initialization
    void createAllPanels(SDL_Renderer* renderer);
    void setupHierarchy(engine::World* world, engine::Level* level, engine::Scene* scene, engine::LayerManager* layerManager);
    
    // Rendering
    void renderAll();
    void updateViewport(float deltaTime);
    
    // Panel access (for callbacks and visibility)
    HierarchyPanel* getHierarchy() { return m_hierarchyPanel.get(); }
    ViewportPanel* getViewport() { return m_viewportPanel.get(); }
    PropertiesPanel* getProperties() { return m_propertiesPanel.get(); }
    AssetBrowserPanel* getAssetBrowser() { return m_assetBrowserPanel.get(); }
    ConsolePanel* getConsole() { return m_consolePanel.get(); }
    DialogGraphPanel* getDialogGraph() { return m_dialogGraphPanel.get(); }
    QuestGraphPanel* getQuestGraph() { return m_questGraphPanel.get(); }
    BehaviorGraphPanel* getBehaviorGraph() { return m_behaviorGraphPanel.get(); }
    SceneGraphPanel* getSceneGraph() { return m_sceneGraphPanel.get(); }
    LayerEditorPanel* getLayerEditor() { return m_layerEditorPanel.get(); }
    TileMapEditorPanel* getTileMapEditor() { return m_tileMapEditorPanel.get(); }
    WorldViewPanel* getWorldView() { return m_worldViewPanel.get(); }
    LevelViewPanel* getLevelView() { return m_levelViewPanel.get(); }
    
    // Console logging helpers
    void log(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);

private:
    EditorContext& m_context;
    
    // All panels
    std::unique_ptr<HierarchyPanel> m_hierarchyPanel;
    std::unique_ptr<PropertiesPanel> m_propertiesPanel;
    std::unique_ptr<ViewportPanel> m_viewportPanel;
    std::unique_ptr<AssetBrowserPanel> m_assetBrowserPanel;
    std::unique_ptr<ConsolePanel> m_consolePanel;
    std::unique_ptr<DialogGraphPanel> m_dialogGraphPanel;
    std::unique_ptr<QuestGraphPanel> m_questGraphPanel;
    std::unique_ptr<BehaviorGraphPanel> m_behaviorGraphPanel;
    std::unique_ptr<SceneGraphPanel> m_sceneGraphPanel;
    std::unique_ptr<LayerEditorPanel> m_layerEditorPanel;
    std::unique_ptr<TileMapEditorPanel> m_tileMapEditorPanel;
    std::unique_ptr<WorldViewPanel> m_worldViewPanel;
    std::unique_ptr<LevelViewPanel> m_levelViewPanel;
    std::unique_ptr<editor::GameSettingsPanel> m_gameSettingsPanel;
};
