/**
 * @file HierarchyPanel.h
 * @brief Hierarki-panel för rum, dialoger, quests, items
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <string>
#include <functional>
#include <memory>

class EditorContext;
class SelectionManager;
namespace engine { 
    class World;
    class Level;
    class Scene; 
}

/**
 * @brief Visar hierarkisk lista över alla spel-objekt
 */
class HierarchyPanel : public IEditorPanel {
public:
    explicit HierarchyPanel(EditorContext& context);
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
    // Callbacks för selection
    using SelectionCallback = std::function<void(const std::string&)>;
    void setOnRoomSelected(SelectionCallback cb) { m_onRoomSelected = cb; }
    void setOnDialogSelected(SelectionCallback cb) { m_onDialogSelected = cb; }
    void setOnQuestSelected(SelectionCallback cb) { m_onQuestSelected = cb; }
    void setOnItemSelected(SelectionCallback cb) { m_onItemSelected = cb; }
    
    // Set selection manager for actor selection and navigation sync
    void setSelectionManager(SelectionManager* selectionManager);
    
    // Set actor details panel for opening details view
    void setActorDetailsPanel(class ActorDetailsPanel* detailsPanel) { m_actorDetailsPanel = detailsPanel; }
    
private:
    void renderWorldHierarchy();
    void renderLevelHierarchy();
    void renderSceneHierarchy();
    void renderActorsForScene();
    const char* getSceneIcon(const std::string& sceneId) const;
    const char* getActorIcon(const std::string& actorName) const;
    
    EditorContext& m_context;
    SelectionManager* m_selectionManager = nullptr;
    class ActorDetailsPanel* m_actorDetailsPanel = nullptr;
    std::string m_id = "hierarchy";
    std::string m_title = "Hierarchy";
    
    SelectionCallback m_onRoomSelected;
    SelectionCallback m_onDialogSelected;
    SelectionCallback m_onQuestSelected;
    SelectionCallback m_onItemSelected;
};
