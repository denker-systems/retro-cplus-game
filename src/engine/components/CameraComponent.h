/**
 * @file CameraComponent.h
 * @brief Camera Component (UE5-style)
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/core/Vec2.h"
#include <SDL.h>

namespace engine {

// ActorObject is defined in CoreRedirects.h, no forward declaration needed

/**
 * @brief Component for camera functionality
 * 
 * Like UCameraComponent in UE5.
 * Can be attached to any Actor to make it a camera.
 */
class CameraComponent : public SceneComponent {
public:
    CameraComponent(const std::string& name = "CameraComponent");
    virtual ~CameraComponent() = default;
    
    void update(float deltaTime) override;
    
    // ========================================================================
    // VIEWPORT
    // ========================================================================
    
    void setViewportSize(int width, int height) {
        m_viewportWidth = width;
        m_viewportHeight = height;
    }
    
    int getViewportWidth() const { return m_viewportWidth; }
    int getViewportHeight() const { return m_viewportHeight; }
    
    // ========================================================================
    // ZOOM
    // ========================================================================
    
    void setZoom(float zoom) { m_zoom = zoom; }
    float getZoom() const { return m_zoom; }
    
    // ========================================================================
    // FOLLOW
    // ========================================================================
    
    void setFollowTarget(ActorObject* target) { m_followTarget = target; }
    ActorObject* getFollowTarget() const { return m_followTarget; }
    
    void setFollowSmoothing(float smoothing) { m_followSmoothing = smoothing; }
    float getFollowSmoothing() const { return m_followSmoothing; }
    
    // ========================================================================
    // BOUNDS
    // ========================================================================
    
    void setBounds(float minX, float minY, float maxX, float maxY) {
        m_hasBounds = true;
        m_minX = minX;
        m_minY = minY;
        m_maxX = maxX;
        m_maxY = maxY;
    }
    
    void clearBounds() { m_hasBounds = false; }
    
    // ========================================================================
    // COORDINATE CONVERSION
    // ========================================================================
    
    Vec2 worldToScreen(Vec2 worldPos) const;
    Vec2 screenToWorld(Vec2 screenPos) const;
    
private:
    int m_viewportWidth = 640;
    int m_viewportHeight = 400;
    float m_zoom = 1.0f;
    
    ActorObject* m_followTarget = nullptr;
    float m_followSmoothing = 0.1f;
    
    bool m_hasBounds = false;
    float m_minX = 0.0f;
    float m_minY = 0.0f;
    float m_maxX = 0.0f;
    float m_maxY = 0.0f;
};

} // namespace engine
