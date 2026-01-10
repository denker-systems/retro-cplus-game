/**
 * @file PhysicsManager.h
 * @brief Physics manager for 3D physics (PhysX only)
 * 
 * Manages PhysX 3D physics world.
 * Box2D removed - using unified 3D architecture.
 */
#pragma once

#include "IPhysicsWorld.h"
#include "physx/PhysicsWorld3D.h"
#include <memory>

namespace engine {
namespace physics {

/**
 * @brief Unified physics manager
 * 
 * Usage:
 *   PhysicsManager manager;
 *   manager.initialize(PhysicsDimension::Physics3D);
 *   manager.step(deltaTime);
 */
class PhysicsManager {
public:
    PhysicsManager() = default;
    ~PhysicsManager() = default;
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    void initialize3D() {
        m_world3D = std::make_unique<PhysicsWorld3D>();
        m_world3D->initialize();
        m_dimension = PhysicsDimension::Physics3D;
    }
    
    // Legacy compatibility (calls initialize3D)
    void initializeBoth() {
        initialize3D();
    }
    
    void shutdown() {
        if (m_world3D) m_world3D->shutdown();
    }
    
    // ========================================================================
    // SIMULATION
    // ========================================================================
    
    void step(float deltaTime) {
        if (m_world3D && m_world3D->isInitialized()) {
            m_world3D->step(deltaTime);
        }
    }
    
    void step3D(float deltaTime) {
        if (m_world3D && m_world3D->isInitialized()) {
            m_world3D->step(deltaTime);
        }
    }
    
    // ========================================================================
    // ACCESS
    // ========================================================================
    
    PhysicsWorld3D* getWorld3D() const { return m_world3D.get(); }
    
    PhysicsDimension getActiveDimension() const { return m_dimension; }
    void setActiveDimension(PhysicsDimension dim) { m_dimension = dim; }
    
    // ========================================================================
    // STATE
    // ========================================================================
    
    bool is3DInitialized() const { return m_world3D && m_world3D->isInitialized(); }
    bool isGPUAccelerationAvailable() const { 
        return m_world3D && m_world3D->isGPUAccelerationAvailable(); 
    }
    
    // ========================================================================
    // DEBUG
    // ========================================================================
    
    void setDebugDraw(bool enabled) {
        if (m_world3D) m_world3D->setDebugDraw(enabled);
    }
    
private:
    std::unique_ptr<PhysicsWorld3D> m_world3D;
    PhysicsDimension m_dimension = PhysicsDimension::Physics3D;
};

} // namespace physics
} // namespace engine
