/**
 * @file Label.h
 * @brief Text rendering node
 */
#pragma once

#include "core/Node2D.h"
#include <SDL.h>
#include <string>

namespace engine {

/**
 * @brief Text label node
 * 
 * Features:
 * - Text rendering with FontManager
 * - Color and alignment
 * - Word wrap (optional)
 */
class Label : public Node2D {
public:
    enum class Alignment {
        Left,
        Center,
        Right
    };
    
    Label();
    explicit Label(const std::string& name);
    Label(const std::string& name, const std::string& text);
    virtual ~Label() = default;
    
    void render(SDL_Renderer* renderer) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // TEXT
    // ═══════════════════════════════════════════════════════════════════
    
    const std::string& getText() const { return m_text; }
    void setText(const std::string& text) { m_text = text; m_needsUpdate = true; }
    
    // ═══════════════════════════════════════════════════════════════════
    // FONT
    // ═══════════════════════════════════════════════════════════════════
    
    const std::string& getFontName() const { return m_fontName; }
    void setFontName(const std::string& name) { m_fontName = name; m_needsUpdate = true; }
    
    // ═══════════════════════════════════════════════════════════════════
    // APPEARANCE
    // ═══════════════════════════════════════════════════════════════════
    
    const SDL_Color& getColor() const { return m_color; }
    void setColor(const SDL_Color& color) { m_color = color; }
    void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) { m_color = {r, g, b, a}; }
    
    Alignment getAlignment() const { return m_alignment; }
    void setAlignment(Alignment align) { m_alignment = align; }
    
    // ═══════════════════════════════════════════════════════════════════
    // SIZE
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Get text dimensions (requires FontManager) */
    Vec2 getTextSize() const;
    int getTextWidth() const;
    int getTextHeight() const;
    
protected:
    std::string m_text;
    std::string m_fontName = "default";
    SDL_Color m_color{255, 255, 255, 255};
    Alignment m_alignment = Alignment::Left;
    
    bool m_needsUpdate = true;
};

} // namespace engine
