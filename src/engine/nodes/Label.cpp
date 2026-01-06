/**
 * @file Label.cpp
 * @brief Implementation of Label node
 */
#include "Label.h"
#include "../graphics/FontManager.h"

namespace engine {

Label::Label() : VisualActor("Label") {}

Label::Label(const std::string& name) : VisualActor(name) {}

Label::Label(const std::string& name, const std::string& text)
    : VisualActor(name)
    , m_text(text) {}

Vec2 Label::getTextSize() const {
    // This would require FontManager to measure text
    // For now, return approximate size
    return Vec2(static_cast<float>(m_text.length() * 8), 16.0f);
}

int Label::getTextWidth() const {
    return static_cast<int>(getTextSize().x);
}

int Label::getTextHeight() const {
    return static_cast<int>(getTextSize().y);
}

void Label::render(SDL_Renderer* renderer) {
    if (m_text.empty() || !renderer) {
        Node2D::render(renderer);
        return;
    }
    
    Vec2 globalPos = getGlobalPosition();
    
    // Apply alignment offset
    Vec2 textSize = getTextSize();
    Vec2 offset(0, 0);
    
    switch (m_alignment) {
        case Alignment::Center:
            offset.x = -textSize.x * 0.5f;
            break;
        case Alignment::Right:
            offset.x = -textSize.x;
            break;
        case Alignment::Left:
        default:
            break;
    }
    
    Vec2 renderPos = globalPos + offset;
    
    // Render text using FontManager
    // NOTE: FontManager is a singleton, need to include the header
    #include "../graphics/FontManager.h"
    FontManager& fontMgr = FontManager::instance();
    fontMgr.renderText(
        renderer,
        m_fontName,
        m_text,
        static_cast<int>(renderPos.x),
        static_cast<int>(renderPos.y),
        m_color
    );
    
    // Render children
    Node2D::render(renderer);
}

} // namespace engine
