/**
 * @file SpriteActor.cpp
 * @brief Implementation of SpriteActor
 */
#include "SpriteActor.h"
#include "engine/components/SpriteComponent.h"
#include "engine/components/AnimationComponent.h"
#include "engine/graphics/TextureManager.h"

namespace engine {

SpriteActor::SpriteActor(const std::string& name)
    : ActorObjectExtended(name)
{
}

void SpriteActor::initializeSprite(const std::string& texturePath) {
    // Add SpriteComponent if not already added
    if (!m_spriteComponent) {
        m_spriteComponent = addComponent<SpriteComponent>();
    }
    
    // Load texture via TextureManager
    SDL_Texture* texture = TextureManager::instance().load(texturePath);
    if (texture) {
        m_spriteComponent->setTexture(texture);
        
        // Query texture size
        int w, h;
        SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
        m_spriteComponent->setSize(w, h);
        m_spriteComponent->setSourceRect({0, 0, w, h});
    }
}

void SpriteActor::initializeAnimation() {
    // Add AnimationComponent if not already added
    if (!m_animationComponent) {
        m_animationComponent = addComponent<AnimationComponent>();
    }
}

} // namespace engine
