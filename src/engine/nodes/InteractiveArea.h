/**
 * @file InteractiveArea.h
 * @brief Clickable hotspot area (for doors, items, NPCs, etc.)
 * 
 * MIGRATION: Now inherits from InteractiveActor (UE5-style)
 */
#pragma once

#include "engine/actors/InteractiveActor.h"
#include <string>
#include <functional>

namespace engine {

/**
 * @brief Interactive clickable area (hotspot)
 * 
 * Represents a clickable region in the game world.
 * Examples: doors, items, NPCs, examine areas
 * 
 * Now inherits from InteractiveActor for better categorization
 */
class InteractiveArea : public InteractiveActor {
public:
    InteractiveArea();
    explicit InteractiveArea(const std::string& name);
    InteractiveArea(const std::string& name, float x, float y, float w, float h);
    ~InteractiveArea() override = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // SIZE
    // ═══════════════════════════════════════════════════════════════════
    
    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }
    void setSize(float w, float h) { m_width = w; m_height = h; }
    
    // ═══════════════════════════════════════════════════════════════════
    // INTERACTION
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Check if point is inside area */
    bool contains(float x, float y) const;
    
    /** @brief Action to perform (e.g. "goto:tavern_exterior", "pickup:key") */
    const std::string& getAction() const { return m_action; }
    void setAction(const std::string& action) { m_action = action; }
    
    /** @brief Display text when hovering */
    const std::string& getHoverText() const { return m_hoverText; }
    void setHoverText(const std::string& text) { m_hoverText = text; }
    
    /** @brief Cursor type (e.g. "hand", "exit", "talk") */
    const std::string& getCursor() const { return m_cursor; }
    void setCursor(const std::string& cursor) { m_cursor = cursor; }
    
    /** @brief Is this area currently enabled/interactable */
    bool isEnabled() const { return m_enabled; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    
    // ═══════════════════════════════════════════════════════════════════
    // RENDERING (for editor)
    // ═══════════════════════════════════════════════════════════════════
    
    void render(SDL_Renderer* renderer) override;

private:
    float m_width = 32.0f;
    float m_height = 32.0f;
    
    std::string m_action;        // Action when clicked
    std::string m_hoverText;     // Text to show on hover
    std::string m_cursor = "hand";  // Cursor type
    bool m_enabled = true;       // Is interactable
};

} // namespace engine
