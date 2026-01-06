/**
 * @file HealthComponent.cpp
 * @brief Health Component Implementation
 */
#include "HealthComponent.h"
#include <algorithm>

namespace engine {

HealthComponent::HealthComponent(const std::string& name)
    : ActorComponent(name)
{
}

void HealthComponent::setHealth(int health) {
    m_currentHealth = std::clamp(health, 0, m_maxHealth);
    
    if (m_currentHealth <= 0 && m_onDeath) {
        m_onDeath();
    }
}

void HealthComponent::damage(int amount) {
    setHealth(m_currentHealth - amount);
}

void HealthComponent::heal(int amount) {
    setHealth(m_currentHealth + amount);
}

} // namespace engine
