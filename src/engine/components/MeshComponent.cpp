/**
 * @file MeshComponent.cpp
 * @brief 3D Mesh component implementation
 */
#include "MeshComponent.h"
#include "engine/utils/Logger.h"

namespace engine {

MeshComponent::MeshComponent(const std::string& name)
    : ActorComponent(name)
{
}

void MeshComponent::initialize() {
    LOG_DEBUG("[MeshComponent] Initialized: " + getName());
}

void MeshComponent::shutdown() {
    LOG_DEBUG("[MeshComponent] Shutdown: " + getName());
}

void MeshComponent::update(float deltaTime) {
    // Mesh rendering handled by 3D viewport
}

void MeshComponent::setPrimitive(PrimitiveMeshType type) {
    m_primitiveType = type;
    m_meshPath.clear();  // Clear custom path when using primitive
    
    // Set default scale based on primitive
    switch (type) {
        case PrimitiveMeshType::Cube:
            m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
            break;
        case PrimitiveMeshType::Sphere:
            m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
            break;
        case PrimitiveMeshType::Capsule:
            m_scale = glm::vec3(0.5f, 1.0f, 0.5f);
            break;
        case PrimitiveMeshType::Cylinder:
            m_scale = glm::vec3(0.5f, 1.0f, 0.5f);
            break;
        case PrimitiveMeshType::Plane:
            m_scale = glm::vec3(2.0f, 0.1f, 2.0f);
            break;
        default:
            break;
    }
    
    LOG_DEBUG("[MeshComponent] Set primitive: " + std::to_string(static_cast<int>(type)));
}

void MeshComponent::setMeshPath(const std::string& path) {
    m_meshPath = path;
    m_primitiveType = PrimitiveMeshType::Custom;
    LOG_DEBUG("[MeshComponent] Set mesh path: " + path);
}

} // namespace engine
