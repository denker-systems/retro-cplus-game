#include "StateManager.h"
#include "IState.h"

void StateManager::pushState(std::unique_ptr<IState> state) {
    if (!m_states.empty()) {
        m_states.top()->exit();
    }
    m_states.push(std::move(state));
    m_states.top()->enter();
}

void StateManager::popState() {
    if (!m_states.empty()) {
        m_states.top()->exit();
        m_states.pop();
        
        if (!m_states.empty()) {
            m_states.top()->enter();
        }
    }
}

void StateManager::changeState(std::unique_ptr<IState> state) {
    while (!m_states.empty()) {
        m_states.top()->exit();
        m_states.pop();
    }
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
