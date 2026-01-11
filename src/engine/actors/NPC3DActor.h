/**
 * @file NPC3DActor.h
 * @brief 3D NPC character actor
 */
#pragma once

#include "Character3DActor.h"
#include <string>

namespace engine {

/**
 * @brief 3D NPC character (replaces legacy 2D NPCActor)
 * 
 * Features:
 * - 3D position and movement
 * - Dialog support
 * - AI behavior
 * - Interaction
 */
class NPC3DActor : public Character3DActor {
public:
    NPC3DActor(const std::string& name = "NPC");
    virtual ~NPC3DActor() = default;
    
    // ========================================================================
    // DIALOG
    // ========================================================================
    
    void setDialogId(const std::string& dialogId) { m_dialogId = dialogId; }
    const std::string& getDialogId() const { return m_dialogId; }
    bool canTalk() const { return !m_dialogId.empty(); }
    
    // ========================================================================
    // INTERACTION
    // ========================================================================
    
    void setInteractionText(const std::string& text) { m_interactionText = text; }
    const std::string& getInteractionText() const { return m_interactionText; }
    
    void setInteractionRange(float range) { m_interactionRange = range; }
    float getInteractionRange() const { return m_interactionRange; }
    
    void setInteractionVolume(const glm::vec3& volume) { m_interactionVolume = volume; }
    glm::vec3 getInteractionVolume() const { return m_interactionVolume; }
    
    void interact();
    
    // ========================================================================
    // AI BEHAVIOR
    // ========================================================================
    
    void setCanMove(bool canMove) { m_canMove = canMove; }
    bool canMove() const { return m_canMove; }
    
    void setPatrolPath(const std::vector<glm::vec3>& path) { m_patrolPath = path; }
    const std::vector<glm::vec3>& getPatrolPath() const { return m_patrolPath; }
    
    // ========================================================================
    // SPRITE
    // ========================================================================
    
    void setSpriteName(const std::string& sprite) { m_spriteName = sprite; }
    const std::string& getSpriteName() const { return m_spriteName; }
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    void update(float deltaTime) override;
    
private:
    std::string m_dialogId;
    std::string m_interactionText;
    float m_interactionRange = 2.0f;
    glm::vec3 m_interactionVolume{6.0f, 2.0f, 6.0f};  // Default interaction volume (X, Y, Z)
    bool m_canMove = false;
    std::string m_spriteName;
    std::vector<glm::vec3> m_patrolPath;
};

} // namespace engine
