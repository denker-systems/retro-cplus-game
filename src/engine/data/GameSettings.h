/**
 * @file GameSettings.h
 * @brief Global game settings that affect gameplay mode
 */
#pragma once

#include <string>

namespace engine {

/**
 * @brief Game mode determines physics and movement style
 */
enum class GameMode {
    Adventure,  // Point-and-click, depth scaling, no physics
    Platformer  // Side-scrolling, physics, jump/run
};

/**
 * @brief Global game settings - singleton
 */
class GameSettings {
public:
    static GameSettings& instance() {
        static GameSettings inst;
        return inst;
    }
    
    // ========================================================================
    // GAME MODE
    // ========================================================================
    
    GameMode getGameMode() const { return m_gameMode; }
    void setGameMode(GameMode mode) { m_gameMode = mode; }
    
    bool isAdventureMode() const { return m_gameMode == GameMode::Adventure; }
    bool isPlatformerMode() const { return m_gameMode == GameMode::Platformer; }
    
    // ========================================================================
    // PHYSICS SETTINGS (for Platformer mode)
    // ========================================================================
    
    float getGravity() const { return m_gravity; }
    void setGravity(float g) { m_gravity = g; }
    
    float getJumpForce() const { return m_jumpForce; }
    void setJumpForce(float f) { m_jumpForce = f; }
    
    float getWalkSpeed() const { return m_walkSpeed; }
    void setWalkSpeed(float s) { m_walkSpeed = s; }
    
    float getRunSpeed() const { return m_runSpeed; }
    void setRunSpeed(float s) { m_runSpeed = s; }
    
    bool isPhysicsDebugEnabled() const { return m_physicsDebug; }
    void setPhysicsDebug(bool enabled) { m_physicsDebug = enabled; }
    
    // ========================================================================
    // HELPER
    // ========================================================================
    
    const char* getGameModeName() const {
        switch (m_gameMode) {
            case GameMode::Adventure: return "Adventure";
            case GameMode::Platformer: return "Platformer";
            default: return "Unknown";
        }
    }
    
private:
    GameSettings() = default;
    
    GameMode m_gameMode = GameMode::Platformer; // Default to platformer for testing
    
    // Physics defaults
    float m_gravity = 980.0f;
    float m_jumpForce = 450.0f;
    float m_walkSpeed = 200.0f;
    float m_runSpeed = 350.0f;
    bool m_physicsDebug = true;
};

} // namespace engine
