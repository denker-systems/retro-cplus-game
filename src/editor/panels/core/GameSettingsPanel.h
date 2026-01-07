/**
 * @file GameSettingsPanel.h
 * @brief Editor panel for game mode and physics settings
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <string>

namespace editor {

/**
 * @brief Panel for configuring game mode (Adventure/Platformer)
 */
class GameSettingsPanel : public IEditorPanel {
public:
    GameSettingsPanel();
    ~GameSettingsPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    void render() override;
    
private:
    void renderGameModeSection();
    void renderPhysicsSection();
    
    std::string m_id = "game_settings";
    std::string m_title = "Game Settings";
};

} // namespace editor
