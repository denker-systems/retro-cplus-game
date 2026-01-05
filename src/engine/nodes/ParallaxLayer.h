/**
 * @file ParallaxLayer.h
 * @brief Layer with parallax scrolling effect
 */
#pragma once

#include "nodes/Sprite.h"

namespace engine {

/**
 * @brief Sprite layer with parallax scrolling
 * 
 * Features:
 * - Parallax factor (0 = static, 1 = normal, >1 = faster)
 * - Repeat horizontally/vertically
 * - Offset for fine-tuning
 */
class ParallaxLayer : public Sprite {
public:
    ParallaxLayer();
    explicit ParallaxLayer(const std::string& name);
    ParallaxLayer(const std::string& name, SDL_Texture* texture);
    virtual ~ParallaxLayer() = default;
    
    void render(SDL_Renderer* renderer) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // PARALLAX
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Parallax factor (0 = static, 1 = normal, >1 = faster) */
    float getParallaxX() const { return m_parallaxX; }
    void setParallaxX(float factor) { m_parallaxX = factor; }
    
    float getParallaxY() const { return m_parallaxY; }
    void setParallaxY(float factor) { m_parallaxY = factor; }
    
    void setParallax(float x, float y) { m_parallaxX = x; m_parallaxY = y; }
    void setParallax(float factor) { m_parallaxX = m_parallaxY = factor; }
    
    // ═══════════════════════════════════════════════════════════════════
    // REPEAT
    // ═══════════════════════════════════════════════════════════════════
    
    bool getRepeatX() const { return m_repeatX; }
    void setRepeatX(bool repeat) { m_repeatX = repeat; }
    
    bool getRepeatY() const { return m_repeatY; }
    void setRepeatY(bool repeat) { m_repeatY = repeat; }
    
    void setRepeat(bool x, bool y) { m_repeatX = x; m_repeatY = y; }
    
    // ═══════════════════════════════════════════════════════════════════
    // OFFSET
    // ═══════════════════════════════════════════════════════════════════
    
    const Vec2& getScrollOffset() const { return m_scrollOffset; }
    void setScrollOffset(const Vec2& offset) { m_scrollOffset = offset; }
    void setScrollOffset(float x, float y) { m_scrollOffset = Vec2(x, y); }
    
protected:
    float m_parallaxX = 0.5f;
    float m_parallaxY = 0.5f;
    
    bool m_repeatX = true;
    bool m_repeatY = false;
    
    Vec2 m_scrollOffset{0, 0};
};

} // namespace engine
