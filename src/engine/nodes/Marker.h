/**
 * @file Marker.h
 * @brief Marker node for positions (spawn points, checkpoints, etc.)
 * 
 * MIGRATION: Now inherits from InteractiveActor (UE5-style)
 */
#pragma once

#include "engine/actors/InteractiveActor.h"
#include <string>

namespace engine {

/**
 * @brief Position marker (spawn point, checkpoint, waypoint, etc.)
 * 
 * Invisible in game, visible in editor for placement.
 * 
 * Now inherits from InteractiveActor for better categorization
 */
class Marker : public InteractiveActor {
public:
    enum class MarkerType {
        PlayerSpawn,
        NPCSpawn,
        Checkpoint,
        Waypoint,
        Camera,
        Other
    };
    
    Marker();
    explicit Marker(const std::string& name);
    Marker(const std::string& name, MarkerType type);
    ~Marker() override = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // TYPE
    // ═══════════════════════════════════════════════════════════════════
    
    MarkerType getMarkerType() const { return m_markerType; }
    void setMarkerType(MarkerType type) { m_markerType = type; }
    
    /** @brief Get color based on marker type */
    SDL_Color getColor() const;
    
    /** @brief Get icon/label text */
    const char* getLabel() const;
    
    // ═══════════════════════════════════════════════════════════════════
    // RENDERING (for editor)
    // ═══════════════════════════════════════════════════════════════════
    
    void render(SDL_Renderer* renderer) override;

private:
    MarkerType m_markerType = MarkerType::Other;
};

} // namespace engine
