/**
 * @file WalkArea.h
 * @brief Defines walkable area for player movement
 * 
 * MIGRATION: Now inherits from InteractiveActor (UE5-style)
 */
#pragma once

#include "engine/actors/InteractiveActor.h"
#include "engine/data/GameData.h"
#include <vector>

namespace engine {

/**
 * @brief Walkable area polygon
 * 
 * Defines where the player can move.
 * Simple rectangle for now, can be extended to polygon later.
 * 
 * Now inherits from InteractiveActor for better categorization
 */
class WalkArea : public InteractiveActor {
public:
    WalkArea();
    explicit WalkArea(const std::string& name);
    ~WalkArea() override = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // BOUNDS
    // ═══════════════════════════════════════════════════════════════════
    
    void setBounds(const WalkAreaData& data) { m_bounds = data; }
    const WalkAreaData& getBounds() const { return m_bounds; }
    
    void setBounds(int minX, int minY, int maxX, int maxY) {
        m_bounds.minX = minX;
        m_bounds.minY = minY;
        m_bounds.maxX = maxX;
        m_bounds.maxY = maxY;
    }
    
    /** @brief Check if point is inside walkable area */
    bool contains(float x, float y) const;
    
    /** @brief Clamp point to walkable area */
    void clampPoint(float& x, float& y) const;
    
    // ═══════════════════════════════════════════════════════════════════
    // RENDERING (for editor)
    // ═══════════════════════════════════════════════════════════════════
    
    void render(SDL_Renderer* renderer) override;

private:
    WalkAreaData m_bounds;
};

} // namespace engine
