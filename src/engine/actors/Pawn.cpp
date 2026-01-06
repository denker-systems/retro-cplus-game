/**
 * @file Pawn.cpp
 * @brief Pawn Implementation
 */
#include "Pawn.h"
#include "Controller.h"

namespace engine {

Pawn::Pawn(const std::string& name)
    : ActorObject(name)
    , m_controller(nullptr)
{
}

void Pawn::possess(Controller* controller) {
    if (m_controller) {
        m_controller->unpossess();
    }
    m_controller = controller;
}

void Pawn::unpossess() {
    m_controller = nullptr;
}

} // namespace engine
