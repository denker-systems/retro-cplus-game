/**
 * @file Pawn.h
 * @brief Pawn - Controllable Entity (UE5-style)
 * 
 * Base class for entities that can be controlled by Controllers.
 * Separates the physical representation (Pawn) from control logic (Controller).
 */
#pragma once

#include "engine/core/CoreRedirects.h"

namespace engine {

class Controller;

/**
 * @brief Base class for controllable entities (like APawn in UE5)
 * 
 * Pawns are entities that can be possessed by Controllers.
 * The Controller sends input/commands, the Pawn executes them.
 * 
 * Separation of Concerns:
 * - Pawn = Physical representation, movement, rendering
 * - Controller = Input handling, AI logic, decision making
 */
class Pawn : public ActorObject {
public:
    Pawn(const std::string& name);
    virtual ~Pawn() = default;
    
    // ========================================================================
    // POSSESSION
    // ========================================================================
    
    /**
     * @brief Possess this pawn with a controller
     * @param controller Controller to possess with
     */
    void possess(Controller* controller);
    
    /**
     * @brief Unpossess this pawn from current controller
     */
    void unpossess();
    
    /**
     * @brief Get current controller
     * @return Pointer to controller, or nullptr if not possessed
     */
    Controller* getController() const { return m_controller; }
    
    /**
     * @brief Check if pawn is possessed
     * @return true if possessed, false otherwise
     */
    bool isPossessed() const { return m_controller != nullptr; }
    
    // ========================================================================
    // INPUT (Called by Controller)
    // ========================================================================
    
    /**
     * @brief Move forward/backward
     * @param value -1.0 to 1.0 (negative = backward)
     */
    virtual void moveForward(float value) {}
    
    /**
     * @brief Move right/left
     * @param value -1.0 to 1.0 (negative = left)
     */
    virtual void moveRight(float value) {}
    
    /**
     * @brief Interact with world
     */
    virtual void interact() {}
    
    /**
     * @brief Look at position
     * @param x X coordinate
     * @param y Y coordinate
     */
    virtual void lookAt(float x, float y) {}
    
protected:
    Controller* m_controller = nullptr;
};

} // namespace engine
