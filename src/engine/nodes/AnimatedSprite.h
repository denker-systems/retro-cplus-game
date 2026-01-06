/**
 * @file AnimatedSprite.h
 * @brief Sprite with animation state machine
 */
#pragma once

#include "Sprite.h"
#include <SDL.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace engine {

/**
 * @brief Animation definition
 */
struct Animation {
    std::string name;
    std::vector<SDL_Rect> frames;  // Source rects for each frame
    float frameTime = 0.1f;         // Seconds per frame
    bool loop = true;
    
    Animation() = default;
    Animation(const std::string& n, const std::vector<SDL_Rect>& f, float ft = 0.1f, bool l = true)
        : name(n), frames(f), frameTime(ft), loop(l) {}
};

/**
 * @brief Sprite with animation playback
 * 
 * Features:
 * - Multiple named animations
 * - Play, pause, stop controls
 * - Frame callbacks
 * - Speed control
 */
class AnimatedSprite : public Sprite {
public:
    AnimatedSprite();
    explicit AnimatedSprite(const std::string& name);
    AnimatedSprite(const std::string& name, SDL_Texture* texture);
    virtual ~AnimatedSprite() = default;
    
    void update(float deltaTime) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // ANIMATION MANAGEMENT
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Add an animation */
    void addAnimation(const Animation& anim);
    void addAnimation(const std::string& name, const std::vector<SDL_Rect>& frames, 
                     float frameTime = 0.1f, bool loop = true);
    
    /** @brief Remove an animation */
    void removeAnimation(const std::string& name);
    
    /** @brief Check if animation exists */
    bool hasAnimation(const std::string& name) const;
    
    /** @brief Get animation */
    const Animation* getAnimation(const std::string& name) const;
    
    // ═══════════════════════════════════════════════════════════════════
    // PLAYBACK CONTROL
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Play an animation */
    void play(const std::string& name, bool restart = false);
    
    /** @brief Stop current animation */
    void stop();
    
    /** @brief Pause/resume */
    void pause() { m_paused = true; }
    void resume() { m_paused = false; }
    bool isPaused() const { return m_paused; }
    
    /** @brief Check if playing */
    bool isPlaying() const { return m_isPlaying; }
    
    /** @brief Get current animation name */
    const std::string& getCurrentAnimation() const { return m_currentAnim; }
    
    // ═══════════════════════════════════════════════════════════════════
    // FRAME CONTROL
    // ═══════════════════════════════════════════════════════════════════
    
    int getCurrentFrame() const { return m_currentFrame; }
    void setCurrentFrame(int frame);
    
    int getFrameCount() const;
    
    /** @brief Speed multiplier (1.0 = normal, 2.0 = double speed) */
    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }
    
    // ═══════════════════════════════════════════════════════════════════
    // CALLBACKS
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Called when animation finishes (non-looping) */
    std::function<void()> onAnimationFinished;
    
    /** @brief Called when animation loops */
    std::function<void()> onAnimationLooped;
    
    /** @brief Called on each frame change */
    std::function<void(int frame)> onFrameChanged;
    
protected:
    void updateAnimation(float deltaTime);
    void advanceFrame();
    
    std::unordered_map<std::string, Animation> m_animations;
    std::string m_currentAnim;
    
    int m_currentFrame = 0;
    float m_frameTimer = 0.0f;
    float m_speed = 1.0f;
    
    bool m_isPlaying = false;
    bool m_paused = false;
};

} // namespace engine
