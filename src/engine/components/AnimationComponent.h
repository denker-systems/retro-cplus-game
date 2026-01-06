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
 */
class AnimationComponent : public ActorComponent {
public:
    struct Animation {
        std::string name;
        std::vector<int> frames;
        float frameTime;
        bool loop;
    };
    
    AnimationComponent(const std::string& name = "AnimationComponent");
    virtual ~AnimationComponent() = default;
    
    void addAnimation(const std::string& name, const std::vector<int>& frames, float frameTime, bool loop = true);
    void play(const std::string& name);
    void stop();
    void pause();
    void resume();
    
    bool isPlaying() const { return m_playing && !m_paused; }
    int getCurrentFrame() const { return m_currentFrameIndex >= 0 && m_currentFrameIndex < m_currentAnimation->frames.size() ? m_currentAnimation->frames[m_currentFrameIndex] : 0; }
    
    void update(float deltaTime) override;
    
private:
    std::unordered_map<std::string, Animation> m_animations;
    Animation* m_currentAnimation = nullptr;
    int m_currentFrameIndex = 0;
    float m_frameTimer = 0.0f;
    bool m_playing = false;
    bool m_paused = false;
};

} // namespace engine
