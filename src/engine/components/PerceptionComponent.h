/**
 * @file PerceptionComponent.h
 * @brief Perception Component (AI sight/hearing)
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/core/Vec2.h"
#include <vector>

namespace engine {

// ActorObject is defined in CoreRedirects.h, no forward declaration needed

/**
 * @brief Component for AI perception (sight, hearing)
 */
class PerceptionComponent : public ActorComponent {
public:
    PerceptionComponent(const std::string& name = "PerceptionComponent");
    virtual ~PerceptionComponent() = default;
    
    void setSightRange(float range) { m_sightRange = range; }
    float getSightRange() const { return m_sightRange; }
    
    void setHearingRange(float range) { m_hearingRange = range; }
    float getHearingRange() const { return m_hearingRange; }
    
    void setFieldOfView(float fov) { m_fieldOfView = fov; }
    float getFieldOfView() const { return m_fieldOfView; }
    
    bool canSee(ActorObject* target) const;
    bool canHear(Vec2 soundPosition) const;
    
    void update(float deltaTime) override;
    
    const std::vector<ActorObject*>& getVisibleActors() const { return m_visibleActors; }
    
private:
    float m_sightRange = 200.0f;
    float m_hearingRange = 150.0f;
    float m_fieldOfView = 90.0f; // degrees
    
    std::vector<ActorObject*> m_visibleActors;
};

} // namespace engine
