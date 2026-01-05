/**
 * @file StateManager.cpp
 * @brief Implementation av state stack hantering
 */
#include "StateManager.h"
#include "IState.h"

void StateManager::pushState(std::unique_ptr<IState> state) {
    // Pausa nuvarande state om den finns
    if (!m_states.empty()) {
        m_states.top()->exit();
    }
    // Lägg till ny state och aktivera
    m_states.push(std::move(state));
    m_states.top()->enter();
}

void StateManager::popState() {
    if (!m_states.empty()) {
        // Avsluta och ta bort översta
        m_states.top()->exit();
        m_states.pop();
        
        // Återaktivera föregående state om den finns
        if (!m_states.empty()) {
            m_states.top()->enter();
        }
    }
}

void StateManager::changeState(std::unique_ptr<IState> state) {
    // Ta bort alla befintliga states
    while (!m_states.empty()) {
        m_states.top()->exit();
        m_states.pop();
    }
    // Lägg till den nya
    m_states.push(std::move(state));
    m_states.top()->enter();
}

void StateManager::update(float deltaTime) {
    if (!m_states.empty()) {
        m_states.top()->update(deltaTime);
    }
}

void StateManager::render(SDL_Renderer* renderer) {
    if (!m_states.empty()) {
        m_states.top()->render(renderer);
    }
}

void StateManager::handleEvent(const SDL_Event& event) {
    if (!m_states.empty()) {
        m_states.top()->handleEvent(event);
    }
}

IState* StateManager::getCurrentState() const {
    if (m_states.empty()) return nullptr;
    return m_states.top().get();
}
