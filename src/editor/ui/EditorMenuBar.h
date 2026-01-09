/**
 * @file EditorMenuBar.h
 * @brief Main menu bar for the editor
 */
#pragma once

#include <functional>
#include <string>

class EditorContext;
class ConsolePanel;
class AssetBrowserPanel;
class PlaceActorsPanel;
class HierarchyPanel;
class PropertiesPanel;
class DialogGraphPanel;
class QuestGraphPanel;
class BehaviorGraphPanel;

namespace editor {
class EditorSettingsPanel;
namespace viewport {
    class ViewportPanel;
}
}

/**
 * @brief Handles the main menu bar rendering and actions
 */
class EditorMenuBar {
public:
    EditorMenuBar(EditorContext& context);
    
    // Render the menu bar, returns menu bar height
    float render();
    
    // Set panel references for visibility toggles
    void setPanels(
        HierarchyPanel* hierarchy,
        editor::viewport::ViewportPanel* viewport,
        PropertiesPanel* properties,
        AssetBrowserPanel* assetBrowser,
        PlaceActorsPanel* placeActors,
        ConsolePanel* console,
        DialogGraphPanel* dialogGraph,
        QuestGraphPanel* questGraph,
        BehaviorGraphPanel* behaviorGraph
    );
    
    // Callbacks
    std::function<void()> onSaveAll;
    std::function<void()> onImportTiled;
    std::function<void()> onExportTiled;
    std::function<void()> onExit;
    std::function<void()> onReloadData;
    
    bool showDemoWindow = false;
    
    void setSettingsPanel(editor::EditorSettingsPanel* settings) { m_settingsPanel = settings; }
    
private:
    void renderFileMenu();
    void renderEditMenu();
    void renderViewMenu();
    void renderToolsMenu();
    
    EditorContext& m_context;
    
    // Panel pointers for visibility
    HierarchyPanel* m_hierarchyPanel = nullptr;
    editor::viewport::ViewportPanel* m_viewportPanel = nullptr;
    PropertiesPanel* m_propertiesPanel = nullptr;
    AssetBrowserPanel* m_assetBrowserPanel = nullptr;
    PlaceActorsPanel* m_placeActorsPanel = nullptr;
    ConsolePanel* m_consolePanel = nullptr;
    DialogGraphPanel* m_dialogGraphPanel = nullptr;
    QuestGraphPanel* m_questGraphPanel = nullptr;
    BehaviorGraphPanel* m_behaviorGraphPanel = nullptr;
    editor::EditorSettingsPanel* m_settingsPanel = nullptr;
};
