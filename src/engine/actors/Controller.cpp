/**
 * @file Controller.cpp
 * @brief Controller Implementation
 */
#include "Controller.h"
#include "Pawn.h"

namespace engine {

Controller::Controller(const std::string& name)
    : Object(name)
    , m_possessedPawn(nullptr)
{
}

void Controller::possess(Pawn* pawn) {
    if (!pawn) return;
    
    // Unpossess current pawn if any
    if (m_possessedPawn) {
        m_possessedPawn->unpossess();
    }
    
    // Possess new pawn
    m_possessedPawn = pawn;
    pawn->possess(this);
}

void Controller::unpossess() {
    if (m_possessedPawn) {
        m_possessedPawn->unpossess();
        m_possessedPawn = nullptr;
    }
}

} // namespace engine
