/**
 * @file MeshComponent.h
 * @brief 3D Mesh rendering component
 * 
 * Handles mesh loading and rendering for 3D actors.
 * Works with the 3D viewport and OpenGL rendering system.
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <glm/glm.hpp>
#include <string>

namespace engine {

/**
 * @brief Primitive mesh types for quick creation
 */
enum class PrimitiveMeshType {
    Cube,
    Sphere,
    Capsule,
    Cylinder,
    Plane,
    Custom  // Loaded from file
};

/**
 * @brief 3D Mesh component for rendering
 * 
 * Usage:
 *   auto* mesh = actor->addComponent<MeshComponent>();
 *   mesh->setPrimitive(PrimitiveMeshType::Cube);
 *   mesh->setColor(glm::vec3(1.0f, 0.5f, 0.0f));
 */
class MeshComponent : public ActorComponent {
public:
    MeshComponent(const std::string& name = "MeshComponent");
    virtual ~MeshComponent() = default;
    
    // ========================================================================
    // ACTORCOMPONENT LIFECYCLE
    // ========================================================================
    
    void initialize() override;
    void shutdown() override;
    void update(float deltaTime) override;
    
    // ========================================================================
    // MESH SETTINGS
    // ========================================================================
    
    void setPrimitive(PrimitiveMeshType type);
    PrimitiveMeshType getPrimitive() const { return m_primitiveType; }
    
    void setMeshPath(const std::string& path);
    const std::string& getMeshPath() const { return m_meshPath; }
    
    // ========================================================================
    // APPEARANCE
    // ========================================================================
    
    void setColor(const glm::vec3& color) { m_color = color; }
    glm::vec3 getColor() const { return m_color; }
    
    void setScale(const glm::vec3& scale) { m_scale = scale; }
    glm::vec3 getScale() const { return m_scale; }
    
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    
    // ========================================================================
    // BOUNDS (for physics shape generation)
    // ========================================================================
    
    glm::vec3 getHalfExtents() const { return m_scale * 0.5f; }
    float getRadius() const { return glm::max(m_scale.x, glm::max(m_scale.y, m_scale.z)) * 0.5f; }
    
private:
    PrimitiveMeshType m_primitiveType = PrimitiveMeshType::Cube;
    std::string m_meshPath;
    
    glm::vec3 m_color{0.8f, 0.8f, 0.8f};
    glm::vec3 m_scale{1.0f, 1.0f, 1.0f};
    bool m_visible = true;
};

} // namespace engine
