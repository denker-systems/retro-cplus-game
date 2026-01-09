/**
 * @file LightActor.h
 * @brief 3D Light Actor for scene lighting
 * 
 * Similar to ALight/APointLight/ADirectionalLight in Unreal Engine.
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include <glm/glm.hpp>

namespace engine {

/**
 * @enum LightType
 * @brief Types of lights supported
 */
enum class LightType {
    Directional,
    Point,
    Spot
};

/**
 * @class LightActor
 * @brief Actor with 3D light functionality
 * 
 * Provides lighting for 3D scenes.
 */
class LightActor : public ActorObjectExtended {
public:
    LightActor(const std::string& name = "LightActor", LightType type = LightType::Point);
    virtual ~LightActor() = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // ========================================================================
    // LIGHT TYPE
    // ========================================================================
    
    LightType getLightType() const { return m_lightType; }
    void setLightType(LightType type) { m_lightType = type; }
    
    // ========================================================================
    // LIGHT PROPERTIES
    // ========================================================================
    
    void setColor(const glm::vec3& color) { m_color = color; }
    glm::vec3 getColor() const { return m_color; }
    
    void setIntensity(float intensity) { m_intensity = intensity; }
    float getIntensity() const { return m_intensity; }
    
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }
    
    // ========================================================================
    // 3D POSITION AND DIRECTION
    // ========================================================================
    
    void setPosition3D(const glm::vec3& pos);
    glm::vec3 getPosition3D() const { return m_position3D; }
    
    void setDirection(const glm::vec3& dir) { m_direction = glm::normalize(dir); }
    glm::vec3 getDirection() const { return m_direction; }
    
    // ========================================================================
    // POINT LIGHT PROPERTIES
    // ========================================================================
    
    void setRadius(float radius) { m_radius = radius; }
    float getRadius() const { return m_radius; }
    
    void setAttenuation(float constant, float linear, float quadratic) {
        m_attenuationConstant = constant;
        m_attenuationLinear = linear;
        m_attenuationQuadratic = quadratic;
    }
    
    float getAttenuationConstant() const { return m_attenuationConstant; }
    float getAttenuationLinear() const { return m_attenuationLinear; }
    float getAttenuationQuadratic() const { return m_attenuationQuadratic; }
    
    // ========================================================================
    // SPOT LIGHT PROPERTIES
    // ========================================================================
    
    void setInnerConeAngle(float angle) { m_innerConeAngle = angle; }
    float getInnerConeAngle() const { return m_innerConeAngle; }
    
    void setOuterConeAngle(float angle) { m_outerConeAngle = angle; }
    float getOuterConeAngle() const { return m_outerConeAngle; }
    
    // ========================================================================
    // SHADOWS
    // ========================================================================
    
    void setCastShadows(bool cast) { m_castShadows = cast; }
    bool getCastShadows() const { return m_castShadows; }

private:
    LightType m_lightType = LightType::Point;
    
    // Common properties
    glm::vec3 m_color{1.0f, 1.0f, 1.0f};
    float m_intensity = 1.0f;
    bool m_enabled = true;
    
    // 3D position and direction
    glm::vec3 m_position3D{0.0f, 5.0f, 0.0f};
    glm::vec3 m_direction{0.0f, -1.0f, 0.0f};
    
    // Point/Spot light
    float m_radius = 10.0f;
    float m_attenuationConstant = 1.0f;
    float m_attenuationLinear = 0.09f;
    float m_attenuationQuadratic = 0.032f;
    
    // Spot light
    float m_innerConeAngle = 12.5f;
    float m_outerConeAngle = 17.5f;
    
    // Shadows
    bool m_castShadows = false;
};

} // namespace engine
