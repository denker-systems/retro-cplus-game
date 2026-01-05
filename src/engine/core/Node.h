/**
 * @file Node.h
 * @brief Base node class for scene graph hierarchy
 * 
 * Inspired by Godot, Cocos2d-x, and SFML scene graph patterns.
 * All game objects inherit from Node.
 */
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <algorithm>
#include <SDL.h>

namespace engine {

/**
 * @brief Base class for all scene graph nodes
 * 
 * Provides:
 * - Parent/child hierarchy
 * - Lifecycle callbacks (onEnter, onExit, update, render)
 * - Z-index for render ordering
 * - Active/visible flags
 */
class Node {
public:
    Node();
    explicit Node(const std::string& name);
    virtual ~Node();
    
    // Non-copyable, movable
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) = default;
    Node& operator=(Node&&) = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // LIFECYCLE CALLBACKS (override in subclasses)
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Called when node is added to scene tree */
    virtual void onEnter();
    
    /** @brief Called when node is removed from scene tree */
    virtual void onExit();
    
    /** @brief Called every frame */
    virtual void update(float deltaTime);
    
    /** @brief Called every frame for rendering */
    virtual void render(SDL_Renderer* renderer);
    
    // ═══════════════════════════════════════════════════════════════════
    // HIERARCHY
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Add a child node */
    void addChild(std::unique_ptr<Node> child);
    
    /** @brief Add a child node with z-index */
    void addChild(std::unique_ptr<Node> child, int zIndex);
    
    /** @brief Remove a child node (returns ownership) */
    std::unique_ptr<Node> removeChild(Node* child);
    
    /** @brief Remove this node from parent */
    void removeFromParent();
    
    /** @brief Get child by name */
    Node* getChild(const std::string& name) const;
    
    /** @brief Get child by index */
    Node* getChildAt(size_t index) const;
    
    /** @brief Get number of children */
    size_t getChildCount() const { return m_children.size(); }
    
    /** @brief Get all children */
    const std::vector<std::unique_ptr<Node>>& getChildren() const { return m_children; }
    
    /** @brief Get parent node */
    Node* getParent() const { return m_parent; }
    
    // ═══════════════════════════════════════════════════════════════════
    // PROPERTIES
    // ═══════════════════════════════════════════════════════════════════
    
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    int getZIndex() const { return m_zIndex; }
    void setZIndex(int z);
    
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }
    
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
    
    bool isInScene() const { return m_inScene; }
    
    /** @brief Called when entering scene tree */
    void propagateEnter();
    
    /** @brief Called when exiting scene tree */
    void propagateExit();

protected:
    /** @brief Update all children */
    void updateChildren(float deltaTime);
    
    /** @brief Render all children (sorted by z-index) */
    void renderChildren(SDL_Renderer* renderer);
    
    /** @brief Sort children by z-index */
    void sortChildrenByZIndex();
    
    std::string m_name;
    Node* m_parent = nullptr;
    std::vector<std::unique_ptr<Node>> m_children;
    
    int m_zIndex = 0;
    bool m_visible = true;
    bool m_active = true;
    bool m_inScene = false;
    bool m_childrenNeedSort = false;
};

} // namespace engine
