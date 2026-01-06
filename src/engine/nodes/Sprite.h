/**
 * @file Sprite.h
 * @brief Sprite node for texture rendering
 * 
 * MIGRATION: Now inherits from VisualActor (UE5-style)
 */
#pragma once

#include "engine/actors/VisualActor.h"
#include <SDL.h>

namespace engine {

/**
 * @brief Renders a texture (or part of it) at the node's position
 * 
 * Features:
 * - Texture rendering with source rect (for sprite sheets)
 * - Tint color
 * - Flip horizontal/vertical
 * - Centered or custom origin
 * 
 * Now inherits from VisualActor for better categorization
 */
class Sprite : public VisualActor {
public:
    Sprite();
    explicit Sprite(const std::string& name);
    Sprite(const std::string& name, SDL_Texture* texture);
    virtual ~Sprite() = default;
    
    void render(SDL_Renderer* renderer) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // TEXTURE
    // ═══════════════════════════════════════════════════════════════════
    
    SDL_Texture* getTexture() const { return m_texture; }
    void setTexture(SDL_Texture* texture);
    
    /** @brief Set source rect (for sprite sheets) */
    void setSourceRect(const SDL_Rect& rect) { m_srcRect = rect; m_hasCustomSrc = true; }
    void setSourceRect(int x, int y, int w, int h) { m_srcRect = {x, y, w, h}; m_hasCustomSrc = true; }
    void clearSourceRect() { m_hasCustomSrc = false; }
    
    const SDL_Rect& getSourceRect() const { return m_srcRect; }
    
    // ═══════════════════════════════════════════════════════════════════
    // APPEARANCE
    // ═══════════════════════════════════════════════════════════════════
    
    const SDL_Color& getTint() const { return m_tint; }
    void setTint(const SDL_Color& color) { m_tint = color; }
    void setTint(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255) { m_tint = {r, g, b, a}; }
    
    bool getFlipX() const { return m_flipX; }
    void setFlipX(bool flip) { m_flipX = flip; }
    
    bool getFlipY() const { return m_flipY; }
    void setFlipY(bool flip) { m_flipY = flip; }
    
    // ═══════════════════════════════════════════════════════════════════
    // SIZE & ORIGIN
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Get sprite size (from texture or source rect) */
    Vec2 getSize() const;
    int getWidth() const;
    int getHeight() const;
    
    /** @brief Origin point for rotation/scaling (0,0 = top-left, 0.5,0.5 = center) */
    const Vec2& getOrigin() const { return m_origin; }
    void setOrigin(const Vec2& origin) { m_origin = origin; }
    void setOrigin(float x, float y) { m_origin = Vec2(x, y); }
    void centerOrigin() { m_origin = Vec2(0.5f, 0.5f); }
    
protected:
    SDL_Texture* m_texture = nullptr;
    SDL_Rect m_srcRect{0, 0, 0, 0};
    bool m_hasCustomSrc = false;
    
    SDL_Color m_tint{255, 255, 255, 255};
    bool m_flipX = false;
    bool m_flipY = false;
    
    Vec2 m_origin{0.5f, 0.5f};  // Default: centered
};

} // namespace engine
