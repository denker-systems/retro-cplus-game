/**
 * @file StartPanel.h
 * @brief Start panel för standalone editor
 */
#pragma once

#include "../IEditorPanel.h"
#include <SDL.h>

class StartPanel : public IEditorPanel {
public:
    StartPanel();
    ~StartPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getDisplayName() const override { return m_displayName; }
    
    void onActivate() override {}
    void onDeactivate() override {}
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    bool handleEvent(const SDL_Event& event) override;
    
private:
    std::string m_id = "start";
    std::string m_displayName = "Start";
    int m_selectedOption = 0;
};
