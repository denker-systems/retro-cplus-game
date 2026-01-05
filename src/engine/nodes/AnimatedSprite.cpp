/**
 * @file AnimatedSprite.cpp
 * @brief Implementation of AnimatedSprite
 */
#include "AnimatedSprite.h"

namespace engine {

AnimatedSprite::AnimatedSprite() : Sprite("AnimatedSprite") {}

AnimatedSprite::AnimatedSprite(const std::string& name) : Sprite(name) {}

AnimatedSprite::AnimatedSprite(const std::string& name, SDL_Texture* texture)
    : Sprite(name, texture) {}

void AnimatedSprite::update(float deltaTime) {
    updateAnimation(deltaTime);
    Sprite::update(deltaTime);
}

// ═══════════════════════════════════════════════════════════════════════════
// ANIMATION MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedSprite::addAnimation(const Animation& anim) {
    m_animations[anim.name] = anim;
}

void AnimatedSprite::addAnimation(const std::string& name, const std::vector<SDL_Rect>& frames,
                                 float frameTime, bool loop) {
    Animation anim(name, frames, frameTime, loop);
    m_animations[name] = anim;
}

void AnimatedSprite::removeAnimation(const std::string& name) {
    m_animations.erase(name);
    if (m_currentAnim == name) {
        stop();
    }
}

bool AnimatedSprite::hasAnimation(const std::string& name) const {
    return m_animations.find(name) != m_animations.end();
}

const Animation* AnimatedSprite::getAnimation(const std::string& name) const {
    auto it = m_animations.find(name);
    if (it != m_animations.end()) {
        return &it->second;
    }
    return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// PLAYBACK CONTROL
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedSprite::play(const std::string& name, bool restart) {
    if (!hasAnimation(name)) return;
    
    // If already playing this animation and not restarting, do nothing
    if (m_currentAnim == name && m_isPlaying && !restart) {
        return;
    }
    
    m_currentAnim = name;
    m_isPlaying = true;
    m_paused = false;
    
    if (restart || m_currentAnim != name) {
        m_currentFrame = 0;
        m_frameTimer = 0.0f;
        
        // Update sprite source rect to first frame
        const Animation* anim = getAnimation(name);
        if (anim && !anim->frames.empty()) {
            setSourceRect(anim->frames[0]);
        }
    }
}

void AnimatedSprite::stop() {
    m_isPlaying = false;
    m_paused = false;
    m_currentFrame = 0;
    m_frameTimer = 0.0f;
}

// ═══════════════════════════════════════════════════════════════════════════
// FRAME CONTROL
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedSprite::setCurrentFrame(int frame) {
    const Animation* anim = getAnimation(m_currentAnim);
    if (!anim || anim->frames.empty()) return;
    
    m_currentFrame = frame % static_cast<int>(anim->frames.size());
    setSourceRect(anim->frames[m_currentFrame]);
}

int AnimatedSprite::getFrameCount() const {
    const Animation* anim = getAnimation(m_currentAnim);
    if (anim) {
        return static_cast<int>(anim->frames.size());
    }
    return 0;
}

// ═══════════════════════════════════════════════════════════════════════════
// ANIMATION UPDATE
// ═══════════════════════════════════════════════════════════════════════════

void AnimatedSprite::updateAnimation(float deltaTime) {
    if (!m_isPlaying || m_paused) return;
    
    const Animation* anim = getAnimation(m_currentAnim);
    if (!anim || anim->frames.empty()) return;
    
    m_frameTimer += deltaTime * m_speed;
    
    while (m_frameTimer >= anim->frameTime) {
        m_frameTimer -= anim->frameTime;
        advanceFrame();
    }
}

void AnimatedSprite::advanceFrame() {
    const Animation* anim = getAnimation(m_currentAnim);
    if (!anim || anim->frames.empty()) return;
    
    int oldFrame = m_currentFrame;
    m_currentFrame++;
    
    // Check if reached end
    if (m_currentFrame >= static_cast<int>(anim->frames.size())) {
        if (anim->loop) {
            m_currentFrame = 0;
            if (onAnimationLooped) {
                onAnimationLooped();
            }
        } else {
            // Non-looping animation finished
            m_currentFrame = static_cast<int>(anim->frames.size()) - 1;
            m_isPlaying = false;
            if (onAnimationFinished) {
                onAnimationFinished();
            }
        }
    }
    
    // Update source rect
    setSourceRect(anim->frames[m_currentFrame]);
    
    // Frame changed callback
    if (oldFrame != m_currentFrame && onFrameChanged) {
        onFrameChanged(m_currentFrame);
    }
}

} // namespace engine
