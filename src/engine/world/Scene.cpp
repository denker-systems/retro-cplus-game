/**
 * @file Scene.cpp
 * @brief Implementation of Scene
 */
#include "Scene.h"

namespace engine {

Scene::Scene() : Node("Scene") {}

Scene::Scene(const std::string& name) : Node(name) {}

void Scene::update(float deltaTime) {
    if (m_isPaused) return;
    
    Node::update(deltaTime);
    
    // Update camera if it's not in the scene tree
    if (m_activeCamera && m_activeCamera->getParent() == nullptr) {
        m_activeCamera->update(deltaTime);
    }
}

void Scene::render(SDL_Renderer* renderer) {
    if (!renderer) return;
    
    // TODO: Apply camera transform to rendering
    // For now, just render children normally
    Node::render(renderer);
}

Camera2D* Scene::createDefaultCamera() {
    auto camera = std::make_unique<Camera2D>("DefaultCamera");
    m_activeCamera = camera.get();
    addChild(std::move(camera));
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
