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
    
    Vec2 pos = getPosition();
    SDL_Rect destRect = {
        static_cast<int>(pos.x),
        static_cast<int>(pos.y),
        m_width,
        m_height
    };
    
    SDL_RenderCopyEx(renderer, m_texture, &m_sourceRect, &destRect, 
                     getRotation(), nullptr, m_flip);
}

} // namespace engine
