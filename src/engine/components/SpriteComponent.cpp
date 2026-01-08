/**
 * @file SpriteComponent.cpp
 * @brief Sprite Component Implementation
 */
#include "SpriteComponent.h"
#include <SDL_image.h>
#include "engine/graphics/TextureManager.h"

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
    
    // Get transform from owner actor (not component's own position)
    Vec2 pos = m_position;  // Start with component's local offset
    float rotation = getRotation();
    Vec2 scale = getScale();
    
    // Add owner's position if available
    if (m_owner) {
        Vec2 ownerPos = m_owner->getPosition();
        pos.x += ownerPos.x;
        pos.y += ownerPos.y;
    }
    
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

bool SpriteComponent::loadTexture(const std::string& path, SDL_Renderer* renderer) {
    if (!renderer) return false;
    
    // Free existing texture
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
        m_texture = nullptr;
    }
    
    // Load new texture
    m_texture = IMG_LoadTexture(renderer, path.c_str());
    if (!m_texture) {
        return false;
    }
    
    // Store path and get dimensions
    m_texturePath = path;
    SDL_QueryTexture(m_texture, nullptr, nullptr, &m_width, &m_height);
    
    // Update source rect to full texture
    m_sourceRect = {0, 0, m_width, m_height};
    
    return true;
}

bool SpriteComponent::loadTextureCached(const std::string& path) {
    // Use TextureManager for cached loading
    SDL_Texture* texture = TextureManager::instance().get(path);
    if (!texture) {
        return false;
    }
    
    m_texture = texture;
    m_texturePath = path;
    
    // Get dimensions
    SDL_QueryTexture(m_texture, nullptr, nullptr, &m_width, &m_height);
    
    // Update source rect to full texture
    m_sourceRect = {0, 0, m_width, m_height};
    
    return true;
}

} // namespace engine
