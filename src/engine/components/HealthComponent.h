/**
 * @file HealthComponent.h
 * @brief Health Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <functional>

namespace engine {

/**
 * @brief Component for health management
 */
class HealthComponent : public ActorComponent {
public:
    using DeathCallback = std::function<void()>;
    
    HealthComponent(const std::string& name = "HealthComponent");
    virtual ~HealthComponent() = default;
    
    void setMaxHealth(int max) { m_maxHealth = max; m_currentHealth = max; }
    int getMaxHealth() const { return m_maxHealth; }
    
    void setHealth(int health);
    int getHealth() const { return m_currentHealth; }
    
    void damage(int amount);
    void heal(int amount);
    
    bool isAlive() const { return m_currentHealth > 0; }
    bool isDead() const { return m_currentHealth <= 0; }
    
    float getHealthPercent() const { return static_cast<float>(m_currentHealth) / m_maxHealth; }
    
    void setOnDeath(DeathCallback callback) { m_onDeath = callback; }
    
private:
    int m_maxHealth = 100;
    int m_currentHealth = 100;
    DeathCallback m_onDeath;
};

} // namespace engine
