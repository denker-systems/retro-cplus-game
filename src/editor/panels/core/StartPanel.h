/**
 * @file StartPanel.h
 * @brief Start panel för standalone editor
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <SDL.h>

/**
 * @brief Start panel for standalone editor
 * 
 * Shows welcome screen and main menu options when the editor first launches.
 * Provides navigation to create new projects, open existing projects, or access documentation.
 */
class StartPanel : public IEditorPanel {
public:
    StartPanel();
    ~StartPanel() override = default;
    
    /**
     * @brief Get unique panel identifier
     * @return Panel ID string
     */
    const std::string& getId() const override { return m_id; }
    
    /**
     * @brief Get display name for UI
     * @return Display name string
     */
    const std::string& getDisplayName() const override { return m_displayName; }
    
    /**
     * @brief Called when panel becomes active
     */
    void onActivate() override {}
    
    /**
     * @brief Called when panel becomes inactive
     */
    void onDeactivate() override {}
    
    /**
     * @brief Update panel logic
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime) override;
    
    /**
     * @brief Render panel content
     * @param renderer SDL renderer for drawing
     */
    void render(SDL_Renderer* renderer) override;
    
    /**
     * @brief Handle input events
     * @param event SDL event to process
     * @return true if event was handled
     */
    bool handleEvent(const SDL_Event& event) override;
    
private:
    std::string m_id = "start";
    std::string m_displayName = "Start";
    int m_selectedOption = 0;
};
