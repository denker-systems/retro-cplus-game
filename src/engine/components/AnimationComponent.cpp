/**
 * @file AnimationComponent.cpp
 * @brief Animation Component Implementation
 */
#include "AnimationComponent.h"

namespace engine {

AnimationComponent::AnimationComponent(const std::string& name)
    : ActorComponent(name)
{
}

void AnimationComponent::addAnimation(const std::string& name, const std::vector<int>& frames, float frameTime, bool loop) {
    Animation anim;
    anim.name = name;
    anim.frames = frames;
    anim.frameTime = frameTime;
    anim.loop = loop;
    m_animations[name] = anim;
}

void AnimationComponent::play(const std::string& name) {
    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
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

void AnimationComponent::update(float deltaTime) {
    if (!m_playing || m_paused || !m_currentAnimation) return;
    
    m_frameTimer += deltaTime;
    
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
