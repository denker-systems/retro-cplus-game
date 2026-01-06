/**
 * @file AnimationComponent.h
 * @brief Animation Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace engine {

/**
 * @brief Component for sprite animation
 * 
 * Handles frame-based sprite animations.
 * Works together with SpriteComponent to update source rects.
 */
class AnimationComponent : public ActorComponent {
public:
    struct Animation {
        std::string name;
        std::vector<SDL_Rect> frames;  // Frame source rects
        float frameTime;
        bool loop;
    };
    
    AnimationComponent(const std::string& name = "AnimationComponent");
    virtual ~AnimationComponent() = default;
    
    // ========================================================================
    // ANIMATION MANAGEMENT
    // ========================================================================
    
    void addAnimation(const std::string& name, const std::vector<SDL_Rect>& frames, 
                     float frameTime, bool loop = true);
    bool hasAnimation(const std::string& name) const;
    
    // ========================================================================
    // PLAYBACK
    // ========================================================================
    
    void play(const std::string& name, bool restart = false);
    void stop();
    void pause();
    void resume();
    
    bool isPlaying() const { return m_playing && !m_paused; }
    bool isPaused() const { return m_paused; }
    
    const std::string& getCurrentAnimationName() const { 
        return m_currentAnimation ? m_currentAnimation->name : m_emptyString; 
    }
    
    // ========================================================================
    // FRAME ACCESS
    // ========================================================================
    
    int getCurrentFrameIndex() const { return m_currentFrameIndex; }
    SDL_Rect getCurrentFrameRect() const;
    
    int getFrameCount() const { 
        return m_currentAnimation ? m_currentAnimation->frames.size() : 0; 
    }
    
    // ========================================================================
    // SPEED CONTROL
    // ========================================================================
    
    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    void update(float deltaTime) override;
    
private:
    std::unordered_map<std::string, Animation> m_animations;
    Animation* m_currentAnimation = nullptr;
    int m_currentFrameIndex = 0;
    float m_frameTimer = 0.0f;
    float m_speed = 1.0f;
    bool m_playing = false;
    bool m_paused = false;
    
    static std::string m_emptyString;
};

} // namespace engine
