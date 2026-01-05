/**
 * @file Widget.cpp
 * @brief Implementation av UI widgets
 */
#include "Widget.h"
#include "../graphics/FontManager.h"
#include <algorithm>

// ============================================================================
// WIDGET BASE
// ============================================================================

Widget::Widget(int x, int y, int width, int height)
    : m_x(x), m_y(y), m_width(width), m_height(height) {
}

void Widget::update(float deltaTime) {
    (void)deltaTime;
}

void Widget::handleEvent(const SDL_Event& event) {
    if (!m_enabled || !m_visible) return;
    
    if (event.type == SDL_MOUSEMOTION) {
        m_hovered = containsPoint(event.motion.x, event.motion.y);
    }
}

bool Widget::containsPoint(int x, int y) const {
    return x >= m_x && x < m_x + m_width &&
           y >= m_y && y < m_y + m_height;
}

// ============================================================================
// LABEL
// ============================================================================

Label::Label(int x, int y, const std::string& text)
    : Widget(x, y, 0, 0), m_text(text) {
}

void Label::render(SDL_Renderer* renderer) {
    if (!m_visible || m_text.empty()) return;
    
    if (m_centered) {
        FontManager::instance().renderTextCentered(renderer, m_fontId,
            m_text, m_x, m_y, m_color);
    } else {
        FontManager::instance().renderText(renderer, m_fontId,
            m_text, m_x, m_y, m_color);
    }
}

// ============================================================================
// BUTTON
// ============================================================================

Button::Button(int x, int y, int width, int height, const std::string& text)
    : Widget(x, y, width, height), m_text(text) {
}

void Button::render(SDL_Renderer* renderer) {
    if (!m_visible) return;
    
    SDL_Rect rect = getBounds();
    
    // Bakgrund
    SDL_Color color = m_normalColor;
    if (!m_enabled) {
        color = {40, 40, 40, 255};
    } else if (m_pressed) {
        color = m_pressedColor;
    } else if (m_hovered) {
        color = m_hoverColor;
    }
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    
    // Ram
    SDL_Color borderColor = m_hovered ? SDL_Color{200, 180, 100, 255} : SDL_Color{100, 90, 120, 255};
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawRect(renderer, &rect);
    
    // Text (centrerad)
    SDL_Color textColor = m_enabled ? m_textColor : SDL_Color{100, 100, 100, 255};
    int textX = m_x + m_width / 2;
    int textY = m_y + m_height / 2 - 8;  // Approximera textcentrering
    FontManager::instance().renderTextCentered(renderer, "default", m_text, textX, textY, textColor);
}

void Button::handleEvent(const SDL_Event& event) {
    Widget::handleEvent(event);
    if (!m_enabled || !m_visible) return;
    
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        if (m_hovered) {
            m_pressed = true;
        }
    } else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        if (m_pressed && m_hovered && m_onClick) {
            m_onClick();
        }
        m_pressed = false;
    }
}

// ============================================================================
// PANEL
// ============================================================================

Panel::Panel(int x, int y, int width, int height)
    : Widget(x, y, width, height) {
}

void Panel::update(float deltaTime) {
    for (auto& widget : m_widgets) {
        if (widget->isVisible()) {
            widget->update(deltaTime);
        }
    }
}

void Panel::render(SDL_Renderer* renderer) {
    if (!m_visible) return;
    
    SDL_Rect rect = getBounds();
    
    // Bakgrund
    if (m_drawBackground) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);
        SDL_RenderFillRect(renderer, &rect);
    }
    
    // Ram
    if (m_drawBorder) {
        SDL_SetRenderDrawColor(renderer, m_borderColor.r, m_borderColor.g, m_borderColor.b, m_borderColor.a);
        SDL_RenderDrawRect(renderer, &rect);
    }
    
    // Rita barn-widgets
    for (auto& widget : m_widgets) {
        if (widget->isVisible()) {
            widget->render(renderer);
        }
    }
}

void Panel::handleEvent(const SDL_Event& event) {
    if (!m_enabled || !m_visible) return;
    
    for (auto& widget : m_widgets) {
        if (widget->isEnabled()) {
            widget->handleEvent(event);
        }
    }
}

void Panel::addWidget(std::unique_ptr<Widget> widget) {
    // Justera widget-position relativt till panel
    widget->setPosition(m_x + widget->getX(), m_y + widget->getY());
    m_widgets.push_back(std::move(widget));
}

Widget* Panel::getWidget(const std::string& id) {
    for (auto& widget : m_widgets) {
        if (widget->getId() == id) {
            return widget.get();
        }
    }
    return nullptr;
}

void Panel::clearWidgets() {
    m_widgets.clear();
}

// ============================================================================
// PROGRESSBAR
// ============================================================================

ProgressBar::ProgressBar(int x, int y, int width, int height)
    : Widget(x, y, width, height) {
}

void ProgressBar::render(SDL_Renderer* renderer) {
    if (!m_visible) return;
    
    SDL_Rect rect = getBounds();
    
    // Bakgrund
    SDL_SetRenderDrawColor(renderer, m_bgColor.r, m_bgColor.g, m_bgColor.b, m_bgColor.a);
    SDL_RenderFillRect(renderer, &rect);
    
    // Fyllning
    if (m_value > 0.0f) {
        SDL_Rect fillRect = rect;
        fillRect.w = static_cast<int>(rect.w * m_value);
        SDL_SetRenderDrawColor(renderer, m_fillColor.r, m_fillColor.g, m_fillColor.b, m_fillColor.a);
        SDL_RenderFillRect(renderer, &fillRect);
    }
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 80, 70, 90, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void ProgressBar::setValue(float value) {
    m_value = std::max(0.0f, std::min(1.0f, value));
}
