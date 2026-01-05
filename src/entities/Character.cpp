/**
 * @file Character.cpp
 * @brief Implementation av Character base class
 */
#include "Character.h"
#include <cmath>

Character::Character(float x, float y, int width, int height, const std::string& name)
    : Entity(x, y, width, height)
    , m_name(name)
    , m_targetX(x)
    , m_targetY(y) {}

void Character::setTarget(float x, float y) {
    m_targetX = x - m_width / 2;
    m_targetY = y - m_height;
    m_hasTarget = true;
    m_facingRight = (m_targetX > m_x);
}

void Character::stop() {
    m_hasTarget = false;
    m_moving = false;
}

void Character::updateMovement(float deltaTime) {
    if (!m_hasTarget) {
        m_moving = false;
        return;
    }
    
    float dx = m_targetX - m_x;
    float dy = m_targetY - m_y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    if (distance < 5.0f) {
        m_hasTarget = false;
        m_moving = false;
    } else {
        m_moving = true;
        float moveX = (dx / distance) * m_speed * deltaTime;
        float moveY = (dy / distance) * m_speed * deltaTime;
        
        m_x += moveX;
        m_y += moveY;
        m_facingRight = (dx > 0);
        
        clampToWalkArea();
    }
}

void Character::clampToWalkArea() {
    // Default walk area - subklasser kan override:a
    if (m_x < 0) m_x = 0;
    if (m_x > 640 - m_width) m_x = 640 - m_width;
    if (m_y < 260) m_y = 260;
    if (m_y > 400 - m_height - 25) m_y = 400 - m_height - 25;
}
