/**
 * @file Scene.cpp
 * @brief Implementation of Scene (UE5-style)
 */
#include "Scene.h"

namespace engine {

// Constructors now in header

void Scene::update(float deltaTime) {
    if (m_isPaused) return;
    
    // Update camera actor
    if (m_cameraActor && m_cameraActor->isActive()) {
        m_cameraActor->update(deltaTime);
    }
    
    // Update all actors
    for (auto& actor : m_actors) {
        if (actor->isActive()) {
            actor->update(deltaTime);
        }
    }
}

void Scene::render(SDL_Renderer* renderer) {
    if (!renderer) return;
    
    // Render all actors
    for (const auto& actor : m_actors) {
        if (actor && actor->isActive()) {
            actor->render(renderer);
        }
    }
}

CameraComponent* Scene::createDefaultCamera() {
    // Create camera actor
    m_cameraActor = std::make_unique<ActorObjectExtended>("CameraActor");
    
    // Create and attach camera component using template syntax
    m_activeCamera = m_cameraActor->addComponent<CameraComponent>("MainCamera");
    
    return m_activeCamera;
}

void Scene::onSceneEnter() {
    // Override in subclass
}

void Scene::onSceneExit() {
    // Override in subclass
}

void Scene::onScenePause() {
    m_isPaused = true;
}

void Scene::onSceneResume() {
    m_isPaused = false;
}

} // namespace engine
