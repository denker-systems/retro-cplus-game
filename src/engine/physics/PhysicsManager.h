/**
 * @file PhysicsManager.h
 * @brief Unified physics manager for 2D and 3D physics
 * 
 * Manages both Box2D (2D) and PhysX (3D) physics worlds.
 * Provides easy access to the appropriate physics system.
 */
#pragma once

#include "IPhysicsWorld.h"
#include "box2d/PhysicsWorld2D.h"
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
    
    void initialize(PhysicsDimension dimension = PhysicsDimension::Physics3D) {
        m_dimension = dimension;
        
        if (dimension == PhysicsDimension::Physics2D) {
            m_world2D = std::make_unique<PhysicsWorld2D>();
            m_world2D->initialize();
        } else {
            m_world3D = std::make_unique<PhysicsWorld3D>();
            m_world3D->initialize();
        }
    }
    
    void initializeBoth() {
        m_world2D = std::make_unique<PhysicsWorld2D>();
        m_world2D->initialize();
        
        m_world3D = std::make_unique<PhysicsWorld3D>();
        m_world3D->initialize();
        
        m_dimension = PhysicsDimension::Physics3D;
    }
    
    void shutdown() {
        if (m_world2D) m_world2D->shutdown();
        if (m_world3D) m_world3D->shutdown();
    }
    
    // ========================================================================
    // SIMULATION
    // ========================================================================
    
    void step(float deltaTime) {
        if (m_world2D && m_world2D->isInitialized()) {
            m_world2D->step(deltaTime);
        }
        if (m_world3D && m_world3D->isInitialized()) {
            m_world3D->step(deltaTime);
        }
    }
    
    void step2D(float deltaTime) {
        if (m_world2D && m_world2D->isInitialized()) {
            m_world2D->step(deltaTime);
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
    
    PhysicsWorld2D* getWorld2D() const { return m_world2D.get(); }
    PhysicsWorld3D* getWorld3D() const { return m_world3D.get(); }
    
    PhysicsDimension getActiveDimension() const { return m_dimension; }
    void setActiveDimension(PhysicsDimension dim) { m_dimension = dim; }
    
    // ========================================================================
    // STATE
    // ========================================================================
    
    bool is2DInitialized() const { return m_world2D && m_world2D->isInitialized(); }
    bool is3DInitialized() const { return m_world3D && m_world3D->isInitialized(); }
    bool isGPUAccelerationAvailable() const { 
        return m_world3D && m_world3D->isGPUAccelerationAvailable(); 
    }
    
    // ========================================================================
    // DEBUG
    // ========================================================================
    
    void setDebugDraw(bool enabled) {
        if (m_world2D) m_world2D->setDebugDraw(enabled);
        if (m_world3D) m_world3D->setDebugDraw(enabled);
    }
    
private:
    std::unique_ptr<PhysicsWorld2D> m_world2D;
    std::unique_ptr<PhysicsWorld3D> m_world3D;
    PhysicsDimension m_dimension = PhysicsDimension::Physics3D;
};

} // namespace physics
} // namespace engine
