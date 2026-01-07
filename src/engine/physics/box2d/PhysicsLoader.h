/**
 * @file PhysicsLoader.h
 * @brief Utility for loading physics data from JSON into actors
 * 
 * Applies PhysicsData from GameData structs to ActorObjectExtended,
 * creating RigidBody2DComponent, Collider2DComponent, and TriggerComponent as needed.
 */
#pragma once

#include "engine/data/GameData.h"

namespace engine {

class ActorObjectExtended;

namespace physics {
class PhysicsWorld2D;
}

/**
 * @brief Loads physics components onto actors from data
 */
class PhysicsLoader {
public:
    /**
     * @brief Apply physics data to an actor
     * @param actor The actor to apply physics to
     * @param data The physics configuration data
     * @param world The physics world (required for body creation)
     * @return true if physics was applied successfully
     */
    static bool applyPhysics(ActorObjectExtended* actor, const PhysicsData& data, physics::PhysicsWorld2D* world);
    
    /**
     * @brief Remove physics components from an actor
     * @param actor The actor to remove physics from
     */
    static void removePhysics(ActorObjectExtended* actor);
    
    /**
     * @brief Check if actor has physics enabled
     * @param actor The actor to check
     * @return true if actor has RigidBody2DComponent
     */
    static bool hasPhysics(ActorObjectExtended* actor);
    
private:
    static void applyColliderData(class Collider2DComponent* collider, const ColliderData& data);
};

} // namespace engine
