/**
 * @file IEditorPanel.h
 * @brief Interface for editor panels (Strategy Pattern)
 * 
 * Each panel represents a distinct editing mode:
 * - RoomPanel: Visual room editing
 * - DialogPanel: Dialog tree editing
 * - QuestPanel: Quest editing
 * - ItemPanel: Item editing
 * etc.
 */
#pragma once

#include <SDL.h>
#include <string>

class EditorContext;

/**
 * @brief Base interface for editor panels
 */
class IEditorPanel {
public:
    virtual ~IEditorPanel() = default;
    
    /**
     * @brief Get panel identifier
     */
    virtual const std::string& getId() const = 0;
    
    /**
     * @brief Get display name for tab
     */
    virtual const std::string& getDisplayName() const = 0;
    
    /**
     * @brief Called when panel becomes active
     */
    virtual void onActivate() = 0;
    
    /**
     * @brief Called when panel becomes inactive
     */
    virtual void onDeactivate() = 0;
    
    /**
     * @brief Update panel logic
     */
    virtual void update(float deltaTime) = 0;
    
    /**
     * @brief Render panel content
     */
    virtual void render(SDL_Renderer* renderer) = 0;
    
    /**
     * @brief Handle input events
     * @return true if event was consumed
     */
    virtual bool handleEvent(const SDL_Event& event) = 0;
    
    /**
     * @brief Get keyboard shortcut hint
     */
    virtual std::string getShortcutHint() const { return ""; }
};

