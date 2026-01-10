/**
 * @file PhysicsWorld3D_2DMode.cpp
 * @brief 2D mode implementation for PhysX (locks Z-axis)
 */
#include "PhysicsWorld3D.h"
#include "engine/utils/Logger.h"
#include <PxPhysicsAPI.h>

namespace engine {
namespace physics {

void PhysicsWorld3D::enable2DMode() {
    if (m_is2DMode) return;
    
    LOG_INFO("[PhysicsWorld3D] Enabling 2D mode (locking Z-axis)...");
    
    m_is2DMode = true;
    
    // Lock Z-axis for all existing bodies
    if (m_scene) {
        physx::PxU32 numActors = m_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
        if (numActors > 0) {
            std::vector<physx::PxRigidActor*> actors(numActors);
            m_scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, 
                              reinterpret_cast<physx::PxActor**>(actors.data()), 
                              numActors);
            
            for (auto* actor : actors) {
                if (auto* dynamicActor = actor->is<physx::PxRigidDynamic>()) {
                    // Lock Z-axis translation and rotation
                    dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, true);
                    dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
                    dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
                    
                    // Reset Z position to 0
                    physx::PxTransform transform = dynamicActor->getGlobalPose();
                    transform.p.z = 0.0f;
                    dynamicActor->setGlobalPose(transform);
                }
            }
            
            LOG_INFO("[PhysicsWorld3D] Locked Z-axis for " + std::to_string(numActors) + " dynamic bodies");
        }
    }
    
    LOG_INFO("[PhysicsWorld3D] 2D mode enabled");
}

void PhysicsWorld3D::disable2DMode() {
    if (!m_is2DMode) return;
    
    LOG_INFO("[PhysicsWorld3D] Disabling 2D mode (unlocking Z-axis)...");
    
    m_is2DMode = false;
    
    // Unlock Z-axis for all existing bodies
    if (m_scene) {
        physx::PxU32 numActors = m_scene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
        if (numActors > 0) {
            std::vector<physx::PxRigidActor*> actors(numActors);
            m_scene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, 
                              reinterpret_cast<physx::PxActor**>(actors.data()), 
                              numActors);
            
            for (auto* actor : actors) {
                if (auto* dynamicActor = actor->is<physx::PxRigidDynamic>()) {
                    // Unlock Z-axis translation and rotation
                    dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, false);
                    dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, false);
                    dynamicActor->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, false);
                }
            }
            
            LOG_INFO("[PhysicsWorld3D] Unlocked Z-axis for " + std::to_string(numActors) + " dynamic bodies");
        }
    }
    
    LOG_INFO("[PhysicsWorld3D] 2D mode disabled");
}

} // namespace physics
} // namespace engine
