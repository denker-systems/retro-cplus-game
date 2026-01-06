/**
 * @file SpriteComponent.cpp
 * @brief Sprite Component Implementation
 */
#include "SpriteComponent.h"

namespace engine {

SpriteComponent::SpriteComponent(const std::string& name)
    : SceneComponent(name)
    , m_texture(nullptr)
    , m_width(32)
    , m_height(32)
    , m_flip(SDL_FLIP_NONE)
{
    m_sourceRect = {0, 0, 32, 32};
}

void SpriteComponent::render(SDL_Renderer* renderer) {
    if (!m_texture || !renderer) return;
    
    // Get transform from owner actor
    Vec2 pos = getPosition();
    float rotation = getRotation();
    Vec2 scale = getScale();
    
    // Apply scale to size
    int finalW = static_cast<int>(m_width * scale.x);
    int finalH = static_cast<int>(m_height * scale.y);
    
    // Calculate destination with origin offset
    SDL_Rect destRect = {
        static_cast<int>(pos.x - m_originX * scale.x),
        static_cast<int>(pos.y - m_originY * scale.y),
        finalW,
        finalH
    };
    
    // Apply tint/opacity
    SDL_SetTextureColorMod(m_texture, m_tint.r, m_tint.g, m_tint.b);
    SDL_SetTextureAlphaMod(m_texture, m_tint.a);
    
    // Calculate flip flags
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (m_flipX) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
    if (m_flipY) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
    
    // Origin point for rotation
    SDL_Point center = {
        static_cast<int>(m_originX * scale.x),
        static_cast<int>(m_originY * scale.y)
    };
    
    // Rotation in degrees
    double angleDeg = rotation * 57.2958; // rad to deg
    
    // Render
    SDL_RenderCopyEx(renderer, m_texture, &m_sourceRect, &destRect, 
                     angleDeg, &center, flip);
}

} // namespace engine
