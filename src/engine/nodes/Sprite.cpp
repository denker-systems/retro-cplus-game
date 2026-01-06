/**
 * @file Sprite.cpp
 * @brief Implementation of Sprite node
 */
#include "Sprite.h"
#include <cmath>

namespace engine {

Sprite::Sprite() : VisualActor("Sprite") {}

Sprite::Sprite(const std::string& name) : VisualActor(name) {}

Sprite::Sprite(const std::string& name, SDL_Texture* texture) 
    : VisualActor(name), m_texture(texture) {}

void Sprite::setTexture(SDL_Texture* texture) {
    m_texture = texture;
    
    // Reset source rect when texture changes
    if (!m_hasCustomSrc && texture) {
        SDL_QueryTexture(texture, nullptr, nullptr, &m_srcRect.w, &m_srcRect.h);
        m_srcRect.x = 0;
        m_srcRect.y = 0;
    }
}

Vec2 Sprite::getSize() const {
    if (m_hasCustomSrc) {
        return Vec2(static_cast<float>(m_srcRect.w), static_cast<float>(m_srcRect.h));
    }
    
    if (m_texture) {
        int w, h;
        SDL_QueryTexture(m_texture, nullptr, nullptr, &w, &h);
        return Vec2(static_cast<float>(w), static_cast<float>(h));
    }
    
    return Vec2::zero();
}

int Sprite::getWidth() const {
    return static_cast<int>(getSize().x);
}

int Sprite::getHeight() const {
    return static_cast<int>(getSize().y);
}

void Sprite::render(SDL_Renderer* renderer) {
    if (!renderer || !m_texture) return;
    
    // Get global transform
    Vec2 globalPos = getGlobalPosition();
    float globalRot = getGlobalRotation();
    Vec2 globalScale = getGlobalScale();
    
    // Get sprite size
    Vec2 size = getSize();
    
    // Apply scale
    int finalW = static_cast<int>(size.x * globalScale.x);
    int finalH = static_cast<int>(size.y * globalScale.y);
    
    // Calculate origin offset
    int originX = static_cast<int>(finalW * m_origin.x);
    int originY = static_cast<int>(finalH * m_origin.y);
    
    // Destination rect
    SDL_Rect dstRect;
    dstRect.x = static_cast<int>(globalPos.x) - originX;
    dstRect.y = static_cast<int>(globalPos.y) - originY;
    dstRect.w = finalW;
    dstRect.h = finalH;
    
    // Source rect
    SDL_Rect srcRect = m_srcRect;
    if (!m_hasCustomSrc) {
        SDL_QueryTexture(m_texture, nullptr, nullptr, &srcRect.w, &srcRect.h);
        srcRect.x = 0;
        srcRect.y = 0;
    }
    
    // Apply tint
    SDL_SetTextureColorMod(m_texture, m_tint.r, m_tint.g, m_tint.b);
    SDL_SetTextureAlphaMod(m_texture, m_tint.a);
    
    // Flip flags
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    if (m_flipX) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_HORIZONTAL);
    if (m_flipY) flip = static_cast<SDL_RendererFlip>(flip | SDL_FLIP_VERTICAL);
    
    // Rotation in degrees
    double angleDeg = globalRot * 57.2958;
    
    // Origin point for rotation
    SDL_Point center = {originX, originY};
    
    // Render
    SDL_RenderCopyEx(renderer, m_texture, &srcRect, &dstRect, angleDeg, &center, flip);
}

} // namespace engine
