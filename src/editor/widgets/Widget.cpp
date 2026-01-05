/**
 * @file Widget.cpp
 * @brief Base editor widget implementation
 */
#include "Widget.h"

namespace Editor {

Widget::Widget(int x, int y, int width, int height) {
    m_bounds = {x, y, width, height};
}

void Widget::update(float deltaTime) {
    (void)deltaTime;
}

void Widget::render(SDL_Renderer* renderer) {
    (void)renderer;
}

bool Widget::handleEvent(const SDL_Event& event) {
    if (!m_state.visible || !m_state.enabled) return false;
    
    switch (event.type) {
        case SDL_MOUSEMOTION: {
            bool wasHovered = m_state.hovered;
            m_state.hovered = containsPoint(event.motion.x, event.motion.y);
            
            if (!wasHovered && m_state.hovered) {
                onMouseEnter();
            } else if (wasHovered && !m_state.hovered) {
                onMouseLeave();
            }
            break;
        }
        
        case SDL_MOUSEBUTTONDOWN: {
            if (containsPoint(event.button.x, event.button.y)) {
                m_state.pressed = true;
                onMouseDown(event.button.x, event.button.y, event.button.button);
                return true;
            }
            break;
        }
        
        case SDL_MOUSEBUTTONUP: {
            if (m_state.pressed) {
                m_state.pressed = false;
                onMouseUp(event.button.x, event.button.y, event.button.button);
                
                if (containsPoint(event.button.x, event.button.y)) {
                    onClick();
                }
                return true;
            }
            break;
        }
    }
    
    return false;
}

void Widget::setPosition(int x, int y) {
    m_bounds.x = x;
    m_bounds.y = y;
}

void Widget::setSize(int width, int height) {
    m_bounds.w = width;
    m_bounds.h = height;
}

void Widget::setBounds(int x, int y, int width, int height) {
    m_bounds = {x, y, width, height};
}

bool Widget::containsPoint(int x, int y) const {
    SDL_Point point = {x, y};
    return SDL_PointInRect(&point, &m_bounds) == SDL_TRUE;
}

// WidgetContainer

void WidgetContainer::addChild(std::unique_ptr<Widget> child) {
    m_children.push_back(std::move(child));
}

void WidgetContainer::removeChild(const std::string& id) {
    m_children.erase(
        std::remove_if(m_children.begin(), m_children.end(),
            [&](const std::unique_ptr<Widget>& w) { return w->getId() == id; }),
        m_children.end()
    );
}

Widget* WidgetContainer::getChild(const std::string& id) {
    for (auto& child : m_children) {
        if (child->getId() == id) return child.get();
    }
    return nullptr;
}

void WidgetContainer::clearChildren() {
    m_children.clear();
}

void WidgetContainer::update(float deltaTime) {
    Widget::update(deltaTime);
    for (auto& child : m_children) {
        if (child->isVisible()) {
            child->update(deltaTime);
        }
    }
}

void WidgetContainer::render(SDL_Renderer* renderer) {
    Widget::render(renderer);
    for (auto& child : m_children) {
        if (child->isVisible()) {
            child->render(renderer);
        }
    }
}

bool WidgetContainer::handleEvent(const SDL_Event& event) {
    // Process children in reverse order (top-most first)
    for (auto it = m_children.rbegin(); it != m_children.rend(); ++it) {
        if ((*it)->isVisible() && (*it)->handleEvent(event)) {
            return true;
        }
    }
    return Widget::handleEvent(event);
}

} // namespace Editor
