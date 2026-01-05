/**
 * @file Node.cpp
 * @brief Implementation of base Node class
 */
#include "Node.h"
#include <iostream>

namespace engine {

Node::Node() : m_name("Node") {}

Node::Node(const std::string& name) : m_name(name) {}

Node::~Node() {
    if (m_inScene) {
        propagateExit();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// LIFECYCLE
// ═══════════════════════════════════════════════════════════════════════════

void Node::onEnter() {
    // Override in subclass
}

void Node::onExit() {
    // Override in subclass
}

void Node::update(float deltaTime) {
    if (!m_active) return;
    updateChildren(deltaTime);
}

void Node::render(SDL_Renderer* renderer) {
    if (!m_visible) return;
    renderChildren(renderer);
}

void Node::updateChildren(float deltaTime) {
    for (auto& child : m_children) {
        if (child && child->isActive()) {
            child->update(deltaTime);
        }
    }
}

void Node::renderChildren(SDL_Renderer* renderer) {
    if (m_childrenNeedSort) {
        sortChildrenByZIndex();
        m_childrenNeedSort = false;
    }
    
    for (auto& child : m_children) {
        if (child && child->isVisible()) {
            child->render(renderer);
        }
    }
}

void Node::sortChildrenByZIndex() {
    std::stable_sort(m_children.begin(), m_children.end(),
        [](const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) {
            return a->getZIndex() < b->getZIndex();
        });
}

void Node::propagateEnter() {
    m_inScene = true;
    onEnter();
    for (auto& child : m_children) {
        if (child) {
            child->propagateEnter();
        }
    }
}

void Node::propagateExit() {
    for (auto& child : m_children) {
        if (child) {
            child->propagateExit();
        }
    }
    onExit();
    m_inScene = false;
}

// ═══════════════════════════════════════════════════════════════════════════
// HIERARCHY
// ═══════════════════════════════════════════════════════════════════════════

void Node::addChild(std::unique_ptr<Node> child) {
    addChild(std::move(child), 0);
}

void Node::addChild(std::unique_ptr<Node> child, int zIndex) {
    if (!child) return;
    
    // Remove from previous parent if any
    if (child->m_parent) {
        child->removeFromParent();
    }
    
    child->m_parent = this;
    child->m_zIndex = zIndex;
    
    // If we're in scene, propagate enter to new child
    if (m_inScene) {
        child->propagateEnter();
    }
    
    m_children.push_back(std::move(child));
    m_childrenNeedSort = true;
}

std::unique_ptr<Node> Node::removeChild(Node* child) {
    if (!child) return nullptr;
    
    auto it = std::find_if(m_children.begin(), m_children.end(),
        [child](const std::unique_ptr<Node>& ptr) {
            return ptr.get() == child;
        });
    
    if (it != m_children.end()) {
        // Propagate exit if in scene
        if ((*it)->m_inScene) {
            (*it)->propagateExit();
        }
        
        (*it)->m_parent = nullptr;
        auto removed = std::move(*it);
        m_children.erase(it);
        return removed;
    }
    
    return nullptr;
}

void Node::removeFromParent() {
    if (m_parent) {
        m_parent->removeChild(this);
    }
}

Node* Node::getChild(const std::string& name) const {
    for (const auto& child : m_children) {
        if (child && child->getName() == name) {
            return child.get();
        }
    }
    return nullptr;
}

Node* Node::getChildAt(size_t index) const {
    if (index < m_children.size()) {
        return m_children[index].get();
    }
    return nullptr;
}

void Node::setZIndex(int z) {
    if (m_zIndex != z) {
        m_zIndex = z;
        if (m_parent) {
            m_parent->m_childrenNeedSort = true;
        }
    }
}

} // namespace engine
