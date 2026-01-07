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
class HierarchyPanel;
class ViewportPanel;
class PropertiesPanel;
class DialogGraphPanel;
class QuestGraphPanel;
class BehaviorGraphPanel;

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
        ViewportPanel* viewport,
        PropertiesPanel* properties,
        AssetBrowserPanel* assetBrowser,
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
    
private:
    void renderFileMenu();
    void renderEditMenu();
    void renderViewMenu();
    void renderToolsMenu();
    
    EditorContext& m_context;
    
    // Panel pointers for visibility
    HierarchyPanel* m_hierarchyPanel = nullptr;
    ViewportPanel* m_viewportPanel = nullptr;
    PropertiesPanel* m_propertiesPanel = nullptr;
    AssetBrowserPanel* m_assetBrowserPanel = nullptr;
    ConsolePanel* m_consolePanel = nullptr;
    DialogGraphPanel* m_dialogGraphPanel = nullptr;
    QuestGraphPanel* m_questGraphPanel = nullptr;
    BehaviorGraphPanel* m_behaviorGraphPanel = nullptr;
};
