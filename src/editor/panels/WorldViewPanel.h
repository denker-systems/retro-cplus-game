/**
 * @file WorldViewPanel.h
 * @brief Panel för att visa och hantera World och Levels
 */
#pragma once

#include "editor/IEditorPanel.h"
#include <string>

class EditorContext;

namespace engine {
    class World;
    class Level;
}

/**
 * @brief Panel som visar hela World-hierarkin
 * 
 * Visar:
 * - Alla Levels i World
 * - Aktiv Level
 * - Möjlighet att skapa/ta bort Levels
 * - Möjlighet att byta aktiv Level
 */
class WorldViewPanel : public IEditorPanel {
public:
    explicit WorldViewPanel(EditorContext& context);
    ~WorldViewPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    void update(float deltaTime) override {}
    
    /** @brief Set the World to visualize */
    void setWorld(engine::World* world) { m_world = world; }

private:
    void renderLevelList();
    void renderLevelProperties(engine::Level* level);
    void renderCreateLevelButton();
    
    EditorContext& m_context;
    std::string m_id = "world_view";
    std::string m_title = "World View";
    
    engine::World* m_world = nullptr;
    std::string m_selectedLevelId;
    
    // UI state
    bool m_showCreateDialog = false;
    char m_newLevelId[64] = "";
    char m_newLevelName[128] = "";
};
