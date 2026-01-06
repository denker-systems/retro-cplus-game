/**
 * @file LevelViewPanel.h
 * @brief Panel för att visa och hantera Scenes i en Level
 */
#pragma once

#include "editor/IEditorPanel.h"
#include <string>

class EditorContext;

namespace engine {
    class Level;
    class Scene;
}

/**
 * @brief Panel som visar alla Scenes i en Level
 * 
 * Visar:
 * - Alla Scenes i vald Level
 * - Aktiv Scene
 * - Scene-thumbnails/previews
 * - Möjlighet att skapa/ta bort Scenes
 * - Möjlighet att byta aktiv Scene
 */
class LevelViewPanel : public IEditorPanel {
public:
    explicit LevelViewPanel(EditorContext& context);
    ~LevelViewPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    void update(float deltaTime) override {}
    
    /** @brief Set the Level to visualize */
    void setLevel(engine::Level* level) { m_level = level; }

private:
    void renderSceneGrid();
    void renderSceneCard(engine::Scene* scene);
    void renderCreateSceneButton();
    void renderSceneProperties(engine::Scene* scene);
    
    EditorContext& m_context;
    std::string m_id = "level_view";
    std::string m_title = "Level View";
    
    engine::Level* m_level = nullptr;
    std::string m_selectedSceneId;
    
    // UI state
    bool m_showCreateDialog = false;
    bool m_showGridView = true;  // Toggle between grid and list view
    char m_newSceneId[64] = "";
    char m_newSceneName[128] = "";
};
