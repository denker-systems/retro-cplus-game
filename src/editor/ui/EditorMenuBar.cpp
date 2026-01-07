/**
 * @file EditorMenuBar.cpp
 * @brief Implementation of EditorMenuBar
 */
#include "EditorMenuBar.h"
#include "editor/core/EditorContext.h"
#include "editor/panels/core/HierarchyPanel.h"
#include "editor/panels/viewport/ViewportPanel.h"
#include "editor/panels/core/PropertiesPanel.h"
#include "editor/panels/assets/AssetBrowserPanel.h"
#include "editor/panels/core/ConsolePanel.h"
#include "editor/panels/graphs/dialog/DialogGraphPanel.h"
#include "editor/panels/graphs/quest/QuestGraphPanel.h"
#include "editor/panels/graphs/npc/BehaviorGraphPanel.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

EditorMenuBar::EditorMenuBar(EditorContext& context)
    : m_context(context)
{
}

void EditorMenuBar::setPanels(
    HierarchyPanel* hierarchy,
    ViewportPanel* viewport,
    PropertiesPanel* properties,
    AssetBrowserPanel* assetBrowser,
    PlaceActorsPanel* placeActors,
    ConsolePanel* console,
    DialogGraphPanel* dialogGraph,
    QuestGraphPanel* questGraph,
    BehaviorGraphPanel* behaviorGraph)
{
    m_hierarchyPanel = hierarchy;
    m_viewportPanel = viewport;
    m_propertiesPanel = properties;
    m_assetBrowserPanel = assetBrowser;
    m_placeActorsPanel = placeActors;
    m_consolePanel = console;
    m_dialogGraphPanel = dialogGraph;
    m_questGraphPanel = questGraph;
    m_behaviorGraphPanel = behaviorGraph;
}

float EditorMenuBar::render() {
#ifdef HAS_IMGUI
    float menuBarHeight = 0;
    
    if (ImGui::BeginMainMenuBar()) {
        menuBarHeight = ImGui::GetWindowSize().y;
        
        renderFileMenu();
        renderEditMenu();
        renderViewMenu();
        renderToolsMenu();
        
        ImGui::EndMainMenuBar();
    }
    
    return menuBarHeight;
#else
    return 0;
#endif
}

void EditorMenuBar::renderFileMenu() {
#ifdef HAS_IMGUI
    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Save All", "Ctrl+S")) {
            m_context.saveToFiles();
            if (onSaveAll) onSaveAll();
        }
        if (ImGui::MenuItem("Import from Tiled...")) {
            if (onImportTiled) onImportTiled();
        }
        if (ImGui::MenuItem("Export to Tiled...")) {
            if (onExportTiled) onExportTiled();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Exit", "ESC")) {
            if (onExit) onExit();
        }
        ImGui::EndMenu();
    }
#endif
}

void EditorMenuBar::renderEditMenu() {
#ifdef HAS_IMGUI
    if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) {}
        if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}
        ImGui::EndMenu();
    }
#endif
}

void EditorMenuBar::renderViewMenu() {
#ifdef HAS_IMGUI
    if (ImGui::BeginMenu("View")) {
        bool hierarchyVisible = m_hierarchyPanel && m_hierarchyPanel->isVisible();
        bool viewportVisible = m_viewportPanel && m_viewportPanel->isVisible();
        bool propertiesVisible = m_propertiesPanel && m_propertiesPanel->isVisible();
        bool assetBrowserVisible = m_assetBrowserPanel && m_assetBrowserPanel->isVisible();
        bool consoleVisible = m_consolePanel && m_consolePanel->isVisible();
        bool dialogGraphVisible = m_dialogGraphPanel && m_dialogGraphPanel->isVisible();
        bool questGraphVisible = m_questGraphPanel && m_questGraphPanel->isVisible();
        bool behaviorGraphVisible = m_behaviorGraphPanel && m_behaviorGraphPanel->isVisible();
        
        if (ImGui::MenuItem("Hierarchy", nullptr, &hierarchyVisible)) {
            if (m_hierarchyPanel) m_hierarchyPanel->setVisible(hierarchyVisible);
        }
        if (ImGui::MenuItem("Viewport", nullptr, &viewportVisible)) {
            if (m_viewportPanel) m_viewportPanel->setVisible(viewportVisible);
        }
        if (ImGui::MenuItem("Properties", nullptr, &propertiesVisible)) {
            if (m_propertiesPanel) m_propertiesPanel->setVisible(propertiesVisible);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Dialog Graph", nullptr, &dialogGraphVisible)) {
            if (m_dialogGraphPanel) m_dialogGraphPanel->setVisible(dialogGraphVisible);
        }
        if (ImGui::MenuItem("Quest Graph", nullptr, &questGraphVisible)) {
            if (m_questGraphPanel) m_questGraphPanel->setVisible(questGraphVisible);
        }
        if (ImGui::MenuItem("NPC Behavior", nullptr, &behaviorGraphVisible)) {
            if (m_behaviorGraphPanel) m_behaviorGraphPanel->setVisible(behaviorGraphVisible);
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Asset Browser", nullptr, &assetBrowserVisible)) {
            if (m_assetBrowserPanel) m_assetBrowserPanel->setVisible(assetBrowserVisible);
        }
        if (ImGui::MenuItem("Console", nullptr, &consoleVisible)) {
            if (m_consolePanel) m_consolePanel->setVisible(consoleVisible);
        }
        ImGui::Separator();
        ImGui::MenuItem("ImGui Demo", nullptr, &showDemoWindow);
        ImGui::EndMenu();
    }
#endif
}

void EditorMenuBar::renderToolsMenu() {
#ifdef HAS_IMGUI
    if (ImGui::BeginMenu("Tools")) {
        if (ImGui::MenuItem("Validate All Data")) {
            if (m_consolePanel) m_consolePanel->logWarning("Validation not implemented yet");
        }
        if (ImGui::MenuItem("Reload Data")) {
            if (onReloadData) onReloadData();
        }
        if (ImGui::MenuItem("Refresh Assets")) {
            if (m_assetBrowserPanel) {
                m_assetBrowserPanel->refreshAssets();
                if (m_consolePanel) m_consolePanel->log("Assets refreshed");
            }
        }
        ImGui::EndMenu();
    }
#endif
}
