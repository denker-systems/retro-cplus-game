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
    // FLIP
    // ========================================================================
    
    void setFlip(SDL_RendererFlip flip) { m_flip = flip; }
    SDL_RendererFlip getFlip() const { return m_flip; }
    
private:
    SDL_Texture* m_texture = nullptr;
    SDL_Rect m_sourceRect{0, 0, 32, 32};
    int m_width = 32;
    int m_height = 32;
    SDL_RendererFlip m_flip = SDL_FLIP_NONE;
};

} // namespace engine
