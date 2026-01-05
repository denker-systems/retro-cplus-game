/**
 * @file Button.cpp
 * @brief Editor button widget implementation
 */
#include "Button.h"
#include "engine/graphics/FontManager.h"
#include "engine/audio/AudioManager.h"

namespace Editor {

Button::Button(int x, int y, int width, int height, const std::string& text)
    : Widget(x, y, width, height), m_text(text) {
}

void Button::render(SDL_Renderer* renderer) {
    if (!m_state.visible) return;
    
    // Determine background color
    SDL_Color bgColor = m_normalColor;
    if (m_state.pressed) {
        bgColor = m_pressedColor;
    } else if (m_state.hovered) {
        bgColor = m_hoverColor;
    }
    
    if (!m_state.enabled) {
        bgColor = {40, 40, 40, 255};
    }
    
    // Draw background
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &m_bounds);
    
    // Draw border
    SDL_SetRenderDrawColor(renderer, m_borderColor.r, m_borderColor.g, m_borderColor.b, m_borderColor.a);
    SDL_RenderDrawRect(renderer, &m_bounds);
    
    // Draw text centered
    SDL_Color textColor = m_state.enabled ? m_textColor : SDL_Color{100, 100, 100, 255};
    
    // Simple centering (approximate)
    int textX = m_bounds.x + (m_bounds.w - static_cast<int>(m_text.length() * 7)) / 2;
    int textY = m_bounds.y + (m_bounds.h - 12) / 2;
    
    FontManager::instance().renderText(renderer, "default", m_text, textX, textY, textColor);
}

void Button::onClick() {
    if (m_onClick && m_state.enabled) {
        AudioManager::instance().playSound("select");
        m_onClick();
    }
}

void Button::setColors(SDL_Color normal, SDL_Color hover, SDL_Color pressed) {
    m_normalColor = normal;
    m_hoverColor = hover;
    m_pressedColor = pressed;
}

} // namespace Editor
