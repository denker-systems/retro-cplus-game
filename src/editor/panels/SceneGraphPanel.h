/**
 * @file SceneGraphPanel.h
 * @brief Scene graph hierarchy panel for node visualization
 */
#pragma once

#include "editor/IEditorPanel.h"
#include "world/Scene.h"
#include "core/ActorObject.h"
#include <string>

class EditorContext;

/**
 * @brief Panel showing hierarchical node tree
 */
class SceneGraphPanel : public IEditorPanel {
public:
    explicit SceneGraphPanel(EditorContext& context);
    ~SceneGraphPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    void render() override;
    
    /** @brief Set active scene to display */
    void setScene(engine::Scene* scene) { m_scene = scene; }
    
private:
    void renderActorTree(engine::ActorObject* actor);
    void renderActorContextMenu(engine::ActorObject* actor);
    
    EditorContext& m_context;
    std::string m_id = "SceneGraph";
    std::string m_title = "Scene Graph";
    
    engine::Scene* m_scene = nullptr;
    engine::ActorObject* m_selectedActor = nullptr;
    bool m_showInactive = true;
};
