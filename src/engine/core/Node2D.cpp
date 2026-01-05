/**
 * @file Node2D.cpp
 * @brief Implementation of Node2D
 */
#include "Node2D.h"
#include <cmath>

namespace engine {

Node2D::Node2D() : Node("Node2D") {}

Node2D::Node2D(const std::string& name) : Node(name) {}

Node2D::Node2D(const std::string& name, float x, float y) 
    : Node(name), m_position(x, y) {}

// ═══════════════════════════════════════════════════════════════════════════
// GLOBAL TRANSFORM
// ═══════════════════════════════════════════════════════════════════════════

Vec2 Node2D::getGlobalPosition() const {
    Node2D* parent2D = getParent2D();
    if (parent2D) {
        Vec2 parentPos = parent2D->getGlobalPosition();
        float parentRot = parent2D->getGlobalRotation();
        Vec2 parentScale = parent2D->getGlobalScale();
        
        // Apply parent transform
        Vec2 scaledPos = Vec2(m_position.x * parentScale.x, m_position.y * parentScale.y);
        Vec2 rotatedPos = scaledPos.rotated(parentRot);
        return parentPos + rotatedPos;
    }
    return m_position;
}

float Node2D::getGlobalRotation() const {
    Node2D* parent2D = getParent2D();
    if (parent2D) {
        return parent2D->getGlobalRotation() + m_rotation;
    }
    return m_rotation;
}

Vec2 Node2D::getGlobalScale() const {
    Node2D* parent2D = getParent2D();
    if (parent2D) {
        Vec2 parentScale = parent2D->getGlobalScale();
        return Vec2(m_scale.x * parentScale.x, m_scale.y * parentScale.y);
    }
    return m_scale;
}

void Node2D::setGlobalPosition(const Vec2& pos) {
    Node2D* parent2D = getParent2D();
    if (parent2D) {
        Vec2 parentPos = parent2D->getGlobalPosition();
        float parentRot = parent2D->getGlobalRotation();
        Vec2 parentScale = parent2D->getGlobalScale();
        
        // Inverse transform
        Vec2 localPos = pos - parentPos;
        localPos = localPos.rotated(-parentRot);
        m_position = Vec2(localPos.x / parentScale.x, localPos.y / parentScale.y);
    } else {
        m_position = pos;
    }
}

void Node2D::setGlobalPosition(float x, float y) {
    setGlobalPosition(Vec2(x, y));
}

// ═══════════════════════════════════════════════════════════════════════════
// MOVEMENT HELPERS
// ═══════════════════════════════════════════════════════════════════════════

void Node2D::moveToward(const Vec2& target, float delta) {
    Vec2 direction = target - m_position;
    float distance = direction.length();
    
    if (distance <= delta) {
        m_position = target;
    } else {
        m_position += direction.normalized() * delta;
    }
}

void Node2D::lookAt(const Vec2& target) {
    m_rotation = m_position.angleTo(target);
}

float Node2D::distanceTo(const Node2D* other) const {
    if (!other) return 0.0f;
    return getGlobalPosition().distanceTo(other->getGlobalPosition());
}

Vec2 Node2D::directionTo(const Node2D* other) const {
    if (!other) return Vec2::zero();
    return (other->getGlobalPosition() - getGlobalPosition()).normalized();
}

Node2D* Node2D::getParent2D() const {
    return dynamic_cast<Node2D*>(m_parent);
}

} // namespace engine
