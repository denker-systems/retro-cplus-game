/**
 * @file Widget.h
 * @brief Base class for editor UI widgets
 */
#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace Editor {

/**
 * @brief Widget state flags
 */
struct WidgetState {
    bool visible = true;
    bool enabled = true;
    bool focused = false;
    bool hovered = false;
    bool pressed = false;
};

/**
 * @brief Base widget class
 */
class Widget {
public:
    Widget(int x, int y, int width, int height);
    virtual ~Widget() = default;
    
    // Lifecycle
    virtual void update(float deltaTime);
    virtual void render(SDL_Renderer* renderer);
    virtual bool handleEvent(const SDL_Event& event);
    
    // Geometry
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setBounds(int x, int y, int width, int height);
    SDL_Rect getBounds() const { return m_bounds; }
    int getX() const { return m_bounds.x; }
    int getY() const { return m_bounds.y; }
    int getWidth() const { return m_bounds.w; }
    int getHeight() const { return m_bounds.h; }
    
    // State
    WidgetState& getState() { return m_state; }
    const WidgetState& getState() const { return m_state; }
    void setVisible(bool visible) { m_state.visible = visible; }
    void setEnabled(bool enabled) { m_state.enabled = enabled; }
    bool isVisible() const { return m_state.visible; }
    bool isEnabled() const { return m_state.enabled; }
    
    // Hit testing
    bool containsPoint(int x, int y) const;
    
    // Identity
    void setId(const std::string& id) { m_id = id; }
    const std::string& getId() const { return m_id; }

protected:
    virtual void onMouseEnter() {}
    virtual void onMouseLeave() {}
    virtual void onMouseDown(int x, int y, int button) { (void)x; (void)y; (void)button; }
    virtual void onMouseUp(int x, int y, int button) { (void)x; (void)y; (void)button; }
    virtual void onClick() {}
    virtual void onFocusGained() {}
    virtual void onFocusLost() {}
    
    SDL_Rect m_bounds;
    WidgetState m_state;
    std::string m_id;
};

/**
 * @brief Container for child widgets
 */
class WidgetContainer : public Widget {
public:
    using Widget::Widget;
    
    void addChild(std::unique_ptr<Widget> child);
    void removeChild(const std::string& id);
    Widget* getChild(const std::string& id);
    void clearChildren();
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    bool handleEvent(const SDL_Event& event) override;

protected:
    std::vector<std::unique_ptr<Widget>> m_children;
};

} // namespace Editor
