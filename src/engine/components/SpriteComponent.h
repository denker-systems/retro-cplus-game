/**
 * @file SpriteComponent.h
 * @brief Sprite Component (UE5-style)
 * 
 * Component-based sprite rendering.
 * Allows sprites to be components attached to ActorObjects.
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <SDL.h>

namespace engine {

/**
 * @brief Component for rendering sprites
 * 
 * Like UStaticMeshComponent in UE5, but for 2D sprites.
 * Renders a texture at the component's position.
 */
class SpriteComponent : public SceneComponent {
public:
    SpriteComponent(const std::string& name = "SpriteComponent");
    virtual ~SpriteComponent() = default;
    
    // ========================================================================
    // TEXTURE
    // ========================================================================
    
    void setTexture(SDL_Texture* texture) { m_texture = texture; }
    SDL_Texture* getTexture() const { return m_texture; }
    
    bool loadTexture(const std::string& path, SDL_Renderer* renderer);
    bool loadTextureCached(const std::string& path);
    const std::string& getTexturePath() const { return m_texturePath; }
    void setTexturePath(const std::string& path) { m_texturePath = path; }
    
    void setSourceRect(SDL_Rect rect) { m_sourceRect = rect; }
    SDL_Rect getSourceRect() const { return m_sourceRect; }
    
    void setSize(int width, int height) {
        m_width = width;
        m_height = height;
    }
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    // ========================================================================
    // RENDERING
    // ========================================================================
    
    void render(SDL_Renderer* renderer) override;
    
    // ========================================================================
    // FLIP & TRANSFORM
    // ========================================================================
    
    void setFlip(SDL_RendererFlip flip) { m_flip = flip; }
    SDL_RendererFlip getFlip() const { return m_flip; }
    
    void setFlipX(bool flip) { m_flipX = flip; }
    void setFlipY(bool flip) { m_flipY = flip; }
    bool isFlipX() const { return m_flipX; }
    bool isFlipY() const { return m_flipY; }
    
    // ========================================================================
    // ORIGIN (Pivot point for rotation/scale)
    // ========================================================================
    
    void setOrigin(float x, float y) { m_originX = x; m_originY = y; }
    void setOriginNormalized(float nx, float ny) { 
        m_originX = nx * m_width;
        m_originY = ny * m_height;
    }
    
    // ========================================================================
    // COLOR & OPACITY
    // ========================================================================
    
    void setTint(SDL_Color color) { m_tint = color; }
    SDL_Color getTint() const { return m_tint; }
    
    void setOpacity(float opacity) { 
        m_tint.a = static_cast<Uint8>(opacity * 255.0f); 
    }
    float getOpacity() const { 
        return m_tint.a / 255.0f; 
    }
    
private:
    SDL_Texture* m_texture = nullptr;
    std::string m_texturePath;
    SDL_Rect m_sourceRect{0, 0, 32, 32};
    int m_width = 32;
    int m_height = 32;
    
    // Flip
    SDL_RendererFlip m_flip = SDL_FLIP_NONE;
    bool m_flipX = false;
    bool m_flipY = false;
    
    // Origin (pivot point)
    float m_originX = 0.0f;
    float m_originY = 0.0f;
    
    // Tint & opacity
    SDL_Color m_tint{255, 255, 255, 255};
};

} // namespace engine
