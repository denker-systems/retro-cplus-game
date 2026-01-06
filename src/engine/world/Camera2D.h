/**
 * @file Camera2D.h
 * @brief 2D camera with viewport, follow, and zoom
 */
#pragma once

#include "core/ActorObject.h"
#include <SDL.h>

namespace engine {

/**
 * @brief 2D camera with viewport and follow behavior
 * 
 * Features:
 * - Smooth camera follow
 * - Zoom support
 * - Screen shake
 * - World/screen coordinate conversion
 */
class Camera2D : public ActorObject {
public:
    Camera2D();
    explicit Camera2D(const std::string& name);
    virtual ~Camera2D() = default;
    
    void update(float deltaTime) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // VIEWPORT
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Set viewport size (screen size) */
    void setViewportSize(int width, int height);
    void setViewportSize(const Vec2& size);
    
    int getViewportWidth() const { return m_viewportWidth; }
    int getViewportHeight() const { return m_viewportHeight; }
    
    // ═══════════════════════════════════════════════════════════════════
    // ZOOM
    // ═══════════════════════════════════════════════════════════════════
    
    float getZoom() const { return m_zoom; }
    void setZoom(float zoom);
    
    void zoomIn(float amount = 0.1f) { setZoom(m_zoom + amount); }
    void zoomOut(float amount = 0.1f) { setZoom(m_zoom - amount); }
    
    // ═══════════════════════════════════════════════════════════════════
    // FOLLOW TARGET
    // ═══════════════════════════════════════════════════════════════════
    
    ActorObject* getTarget() const { return m_target; }
    void setTarget(ActorObject* target) { m_target = target; }
    
    /** @brief Smoothing factor (0 = instant, 1 = very slow) */
    float getSmoothing() const { return m_smoothing; }
    void setSmoothing(float smoothing) { m_smoothing = smoothing; }
    
    /** @brief Offset from target position */
    const Vec2& getOffset() const { return m_offset; }
    void setOffset(const Vec2& offset) { m_offset = offset; }
    void setOffset(float x, float y) { m_offset = Vec2(x, y); }
    
    // ═══════════════════════════════════════════════════════════════════
    // LIMITS
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Set world bounds (camera won't go outside) */
    void setLimits(int minX, int minY, int maxX, int maxY);
    void setLimits(const SDL_Rect& limits) { m_limits = limits; m_hasLimits = true; }
    void clearLimits() { m_hasLimits = false; }
    
    bool hasLimits() const { return m_hasLimits; }
    const SDL_Rect& getLimits() const { return m_limits; }
    
    // ═══════════════════════════════════════════════════════════════════
    // SCREEN SHAKE
    // ═══════════════════════════════════════════════════════════════════
    
    void shake(float intensity, float duration);
    void stopShake();
    
    // ═══════════════════════════════════════════════════════════════════
    // COORDINATE CONVERSION
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Convert world position to screen position */
    Vec2 worldToScreen(const Vec2& worldPos) const;
    
    /** @brief Convert screen position to world position */
    Vec2 screenToWorld(const Vec2& screenPos) const;
    
    /** @brief Get camera's view rectangle in world space */
    SDL_Rect getViewRect() const;
    
    /** @brief Check if a point is visible */
    bool isVisible(const Vec2& worldPos) const;
    
    /** @brief Check if a rect is visible */
    bool isVisible(const SDL_Rect& worldRect) const;
    
protected:
    void updateFollow(float deltaTime);
    void updateShake(float deltaTime);
    void applyLimits();
    
    int m_viewportWidth = 640;
    int m_viewportHeight = 400;
    float m_zoom = 1.0f;
    
    // Follow
    ActorObject* m_target = nullptr;
    float m_smoothing = 0.1f;
    Vec2 m_offset{0, 0};
    
    // Limits
    bool m_hasLimits = false;
    SDL_Rect m_limits{0, 0, 0, 0};
    
    // Screen shake
    float m_shakeIntensity = 0.0f;
    float m_shakeDuration = 0.0f;
    float m_shakeTimer = 0.0f;
    Vec2 m_shakeOffset{0, 0};
};

} // namespace engine
