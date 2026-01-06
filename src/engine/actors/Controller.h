/**
 * @file Controller.h
 * @brief Controller - Controls Pawns (UE5-style)
 * 
 * Base class for controllers that possess and control Pawns.
 * Separates input/AI logic from physical representation.
 */
#pragma once

#include "engine/core/CoreRedirects.h"

namespace engine {

class Pawn;

/**
 * @brief Base class for controllers (like AController in UE5)
 * 
 * Controllers possess Pawns and send them commands.
 * This separates control logic from physical representation.
 * 
 * Controller Types:
 * - PlayerController: Handles player input
 * - AIController: Handles AI decision making
 */
class Controller : public Object {
public:
    Controller(const std::string& name);
    virtual ~Controller() = default;
    
    // ========================================================================
    // POSSESSION
    // ========================================================================
    
    /**
     * @brief Possess a pawn
     * @param pawn Pawn to possess
     */
    void possess(Pawn* pawn);
    
    /**
     * @brief Unpossess current pawn
     */
    void unpossess();
    
    /**
     * @brief Get possessed pawn
     * @return Pointer to pawn, or nullptr if not possessing
     */
    Pawn* getPawn() const { return m_possessedPawn; }
    
    /**
     * @brief Check if possessing a pawn
     * @return true if possessing, false otherwise
     */
    bool isPossessing() const { return m_possessedPawn != nullptr; }
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    /**
     * @brief Update controller logic
     * @param deltaTime Time since last frame
     */
    virtual void update(float deltaTime) {}
    
protected:
    Pawn* m_possessedPawn = nullptr;
};

} // namespace engine
