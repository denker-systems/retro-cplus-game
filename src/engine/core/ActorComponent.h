/**
 * @file ActorComponent.h
 * @brief Component System Base Classes (UE5-style)
 * 
 * Implements the component system foundation inspired by Unreal Engine:
 * - ActorComponent: Base for all components (like UActorComponent)
 * - SceneComponent: Components with transform (like USceneComponent)
 * 
 * Components are reusable pieces of functionality that can be attached
 * to ActorObjects to add behavior without inheritance.
 */
#pragma once

#include "CoreRedirects.h"
#include "Vec2.h"
#include <string>
#include <SDL.h>

namespace engine {

// Forward declarations
// Note: ActorObject is defined in CoreRedirects.h, no forward declaration needed
class SceneComponent;

// ============================================================================
// ACTOR COMPONENT (Base Component)
// ============================================================================

/**
 * @brief Base class for all components (like UActorComponent in UE5)
 * 
 * Components are reusable pieces of functionality that can be attached to
 * ActorObjects. They provide a composition-based alternative to inheritance.
 * 
 * Example:
 * @code
 * auto* healthComp = actor->addComponent<HealthComponent>();
 * healthComp->setMaxHealth(100);
 * @endcode
 */
class ActorComponent : public Object {
public:
    ActorComponent(const std::string& name = "ActorComponent");
    virtual ~ActorComponent() = default;
    
    // ========================================================================
    // OWNERSHIP
    // ========================================================================
    
    /**
     * @brief Set the owning actor
     * @param owner Pointer to the owning ActorObject
     */
    void setOwner(ActorObject* owner) { m_owner = owner; }
    
    /**
     * @brief Get the owning actor
     * @return Pointer to owner, or nullptr if no owner
     */
    ActorObject* getOwner() const { return m_owner; }
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    /**
     * @brief Called when component is first added to an actor
     * Override to perform initialization
     */
    virtual void initialize() {}
    
    /**
     * @brief Called every frame
     * @param deltaTime Time since last frame in seconds
     */
    virtual void update(float deltaTime) {}
    
    /**
     * @brief Called every frame for rendering
     * @param renderer SDL renderer
     */
    virtual void render(SDL_Renderer* renderer) {}
    
    /**
     * @brief Called when component is removed or actor is destroyed
     * Override to perform cleanup
     */
    virtual void shutdown() {}
    
    // ========================================================================
    // ENABLE/DISABLE
    // ========================================================================
    
    /**
     * @brief Check if component is enabled
     * @return true if enabled, false otherwise
     */
    bool isEnabled() const { return m_enabled; }
    
    /**
     * @brief Enable or disable component
     * @param enabled true to enable, false to disable
     * 
     * Disabled components don't receive update() or render() calls
     */
    void setEnabled(bool enabled) { m_enabled = enabled; }
    
protected:
    ActorObject* m_owner = nullptr; ///< Owning actor
    bool m_enabled = true;          ///< Is component enabled?
};

// ============================================================================
// SCENE COMPONENT (Component with Transform)
// ============================================================================

/**
 * @brief Component with transform (position, rotation, scale)
 * 
 * Like USceneComponent in UE5. Used for components that need to know their
 * position in the world (sprites, meshes, cameras, etc.)
 * 
 * Example:
 * @code
 * auto* sprite = actor->addComponent<SpriteComponent>();
 * sprite->setPosition(Vec2(100, 200));
 * sprite->setRotation(45.0f);
 * @endcode
 */
class SceneComponent : public ActorComponent {
public:
    SceneComponent(const std::string& name = "SceneComponent");
    virtual ~SceneComponent() = default;
    
    // ========================================================================
    // TRANSFORM
    // ========================================================================
    
    /**
     * @brief Get component position
     * @return Position vector
     */
    Vec2 getPosition() const { return m_position; }
    
    /**
     * @brief Set component position
     * @param pos New position
     */
    void setPosition(Vec2 pos) { m_position = pos; }
    
    /**
     * @brief Get component rotation (degrees)
     * @return Rotation in degrees
     */
    float getRotation() const { return m_rotation; }
    
    /**
     * @brief Set component rotation
     * @param rot Rotation in degrees
     */
    void setRotation(float rot) { m_rotation = rot; }
    
    /**
     * @brief Get component scale
     * @return Scale vector
     */
    Vec2 getScale() const { return m_scale; }
    
    /**
     * @brief Set component scale
     * @param scale New scale
     */
    void setScale(Vec2 scale) { m_scale = scale; }
    
    // ========================================================================
    // ATTACHMENT (Future: Full hierarchy support)
    // ========================================================================
    
    /**
     * @brief Attach to parent component
     * @param parent Parent to attach to
     * @note Not fully implemented yet
     */
    void attachTo(SceneComponent* parent) { m_parent = parent; }
    
    /**
     * @brief Get parent component
     * @return Parent component, or nullptr if no parent
     */
    SceneComponent* getParent() const { return m_parent; }
    
protected:
    Vec2 m_position{0, 0};      ///< Component position
    float m_rotation = 0.0f;    ///< Component rotation (degrees)
    Vec2 m_scale{1.0f, 1.0f};   ///< Component scale
    SceneComponent* m_parent = nullptr; ///< Parent component
};

} // namespace engine
