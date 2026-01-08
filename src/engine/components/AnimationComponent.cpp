/**
 * @file AnimationComponent.cpp
 * @brief Animation Component Implementation
 */
#include "AnimationComponent.h"

namespace engine {

std::string AnimationComponent::m_emptyString = "";

AnimationComponent::AnimationComponent(const std::string& name)
    : ActorComponent(name)
{
}

void AnimationComponent::addAnimation(const std::string& name, 
                                     const std::vector<SDL_Rect>& frames, 
                                     float frameTime, bool loop) {
    Animation anim;
    anim.name = name;
    anim.frames = frames;
    anim.frameTime = frameTime;
    anim.loop = loop;
    m_animations[name] = anim;
}

bool AnimationComponent::hasAnimation(const std::string& name) const {
    return m_animations.find(name) != m_animations.end();
}

void AnimationComponent::play(const std::string& name, bool restart) {
    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        // Don't restart if already playing this animation
        if (!restart && m_currentAnimation == &it->second && m_playing) {
            return;
        }
        
        m_currentAnimation = &it->second;
        m_currentFrameIndex = 0;
        m_frameTimer = 0.0f;
        m_playing = true;
        m_paused = false;
    }
}

void AnimationComponent::stop() {
    m_playing = false;
    m_currentFrameIndex = 0;
    m_frameTimer = 0.0f;
}

void AnimationComponent::pause() {
    m_paused = true;
}

void AnimationComponent::resume() {
    m_paused = false;
}

void AnimationComponent::play() {
    // Resume current animation if exists
    if (m_currentAnimation) {
        m_playing = true;
        m_paused = false;
    }
}

SDL_Rect AnimationComponent::getCurrentFrameRect() const {
    if (m_currentAnimation && 
        m_currentFrameIndex >= 0 && 
        m_currentFrameIndex < m_currentAnimation->frames.size()) {
        return m_currentAnimation->frames[m_currentFrameIndex];
    }
    return {0, 0, 0, 0};
}

void AnimationComponent::update(float deltaTime) {
    if (!m_playing || m_paused || !m_currentAnimation) return;
    
    m_frameTimer += deltaTime * m_speed;
    
    if (m_frameTimer >= m_currentAnimation->frameTime) {
        m_frameTimer = 0.0f;
        m_currentFrameIndex++;
        
        if (m_currentFrameIndex >= m_currentAnimation->frames.size()) {
            if (m_currentAnimation->loop) {
                m_currentFrameIndex = 0;
            } else {
                m_currentFrameIndex = m_currentAnimation->frames.size() - 1;
                m_playing = false;
            }
        }
    }
}

} // namespace engine
