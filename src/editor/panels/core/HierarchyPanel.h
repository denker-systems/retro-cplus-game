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
namespace engine { class Scene; }

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
    
    // Set selection manager for actor selection
    void setSelectionManager(SelectionManager* selectionManager);
    void setActiveScene(engine::Scene* scene) { m_activeScene = scene; }
    
private:
    const char* getSceneIcon(const std::string& sceneId) const;
    void renderActorsForScene(const std::string& roomId);
    
    EditorContext& m_context;
    SelectionManager* m_selectionManager = nullptr;
    engine::Scene* m_activeScene = nullptr;
    std::string m_id = "hierarchy";
    std::string m_title = "Hierarchy";
    
    SelectionCallback m_onRoomSelected;
    SelectionCallback m_onDialogSelected;
    SelectionCallback m_onQuestSelected;
    SelectionCallback m_onItemSelected;
};
