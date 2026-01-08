/**
 * @file Viewport3DPanel.cpp
 * @brief 3D viewport panel implementation
 */
#include "Viewport3DPanel.h"
#include "editor/core/SelectionManager.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"

#ifdef HAS_IMGUI

#include <imgui.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <limits>

namespace editor {

// Basic vertex shader
static const char* BASIC_VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 u_MVP;
uniform mat4 u_Model;

out vec3 FragPos;
out vec3 Normal;

void main() {
    FragPos = vec3(u_Model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(u_Model))) * aNormal;
    gl_Position = u_MVP * vec4(aPos, 1.0);
}
)";

// Basic fragment shader
static const char* BASIC_FRAGMENT_SHADER = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 u_Color;

void main() {
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float diff = max(dot(normalize(Normal), lightDir), 0.0);
    vec3 ambient = 0.3 * u_Color;
    vec3 diffuse = diff * u_Color;
    FragColor = vec4(ambient + diffuse, 1.0);
}
)";

// Grid vertex shader
static const char* GRID_VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_MVP;

void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
}
)";

// Grid fragment shader
static const char* GRID_FRAGMENT_SHADER = R"(
#version 330 core
out vec4 FragColor;

uniform vec3 u_Color;

void main() {
    FragColor = vec4(u_Color, 0.5);
}
)";

Viewport3DPanel::Viewport3DPanel() = default;

Viewport3DPanel::~Viewport3DPanel() = default;

bool Viewport3DPanel::initialize() {
    if (m_initialized) return true;
    
    // Create framebuffer
    engine::FramebufferSpec spec;
    spec.width = 1280;
    spec.height = 720;
    spec.hasDepth = true;
    m_framebuffer = std::make_unique<engine::Framebuffer>(spec);
    
    if (!m_framebuffer->isValid()) {
        std::cerr << "Viewport3DPanel: Failed to create framebuffer" << std::endl;
        return false;
    }
    
    // Create camera
    m_camera = std::make_unique<EditorCamera3D>(45.0f, 16.0f / 9.0f);
    
    // Create shaders
    if (!createShaders()) {
        std::cerr << "Viewport3DPanel: Failed to create shaders" << std::endl;
        return false;
    }
    
    // Create meshes
    m_gridMesh = engine::Mesh::createGrid(20, 1.0f);
    m_cubeMesh = engine::Mesh::createCube();
    
    m_initialized = true;
    std::cout << "Viewport3DPanel: Initialized successfully" << std::endl;
    return true;
}

bool Viewport3DPanel::createShaders() {
    m_shader = std::make_unique<engine::Shader>(BASIC_VERTEX_SHADER, BASIC_FRAGMENT_SHADER);
    if (!m_shader->isValid()) {
        return false;
    }
    
    m_gridShader = std::make_unique<engine::Shader>(GRID_VERTEX_SHADER, GRID_FRAGMENT_SHADER);
    if (!m_gridShader->isValid()) {
        return false;
    }
    
    return true;
}

void Viewport3DPanel::update(float deltaTime) {
    if (m_camera) {
        m_camera->update(deltaTime);
    }
}

void Viewport3DPanel::render() {
    // Initialize on first render
    if (!m_initialized) {
        if (!initialize()) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Failed to initialize 3D viewport");
            ImGui::TextWrapped("OpenGL context may not be available.");
            return;
        }
    }
    
    // Get viewport size
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    
    // Resize framebuffer if needed
    if (viewportSize.x > 0 && viewportSize.y > 0) {
        if (viewportSize.x != m_viewportSize.x || viewportSize.y != m_viewportSize.y) {
            m_viewportSize = {viewportSize.x, viewportSize.y};
            m_framebuffer->resize(static_cast<int>(viewportSize.x), static_cast<int>(viewportSize.y));
            m_camera->setAspectRatio(viewportSize.x / viewportSize.y);
        }
    }
    
    // Handle input before rendering
    handleInput();
    
    // Render to framebuffer
    m_framebuffer->bind();
    m_framebuffer->clear(0.15f, 0.15f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    renderScene();
    
    m_framebuffer->unbind();
    
    // Display framebuffer as ImGui image
    ImVec2 pos = ImGui::GetCursorScreenPos();
    m_viewportPos = {pos.x, pos.y};
    
    // Flip Y for OpenGL texture
    ImGui::Image(
        (ImTextureID)(uintptr_t)m_framebuffer->getColorAttachment(),
        viewportSize,
        ImVec2(0, 1), ImVec2(1, 0)
    );
    
    // Track focus and hover state
    m_viewportFocused = ImGui::IsWindowFocused();
    m_viewportHovered = ImGui::IsItemHovered();
}

void Viewport3DPanel::renderScene() {
    if (!m_camera || !m_shader) return;
    
    // Render grid
    renderGrid();
    
    // Render based on view level
    switch (m_viewLevel) {
        case View3DLevel::World:
            renderWorldView();
            break;
        case View3DLevel::Level:
            renderLevelView();
            break;
        case View3DLevel::Scene:
        default:
            renderSceneView();
            break;
    }
}

void Viewport3DPanel::renderWorldView() {
    if (!m_world || !m_shader || !m_cubeMesh) return;
    
    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 projection = m_camera->getProjectionMatrix();
    
    // Clear and rebuild bounds
    m_objectBounds.clear();
    
    m_shader->bind();
    
    // Render each level as a box
    const auto& levels = m_world->getLevels();
    float spacing = 5.0f;
    int index = 0;
    
    for (const auto& level : levels) {
        // Position levels in a row
        float x = (index - (int)levels.size() / 2.0f) * spacing;
        glm::vec3 pos(x, 0.5f, 0.0f);
        glm::vec3 scale(2.0f, 1.0f, 2.0f);
        
        // Store bounds for picking
        glm::vec3 boxMin = pos - scale * 0.5f;
        glm::vec3 boxMax = pos + scale * 0.5f;
        m_objectBounds.push_back({boxMin, boxMax});
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);
        
        glm::mat4 mvp = projection * view * model;
        
        m_shader->setMat4("u_MVP", mvp);
        m_shader->setMat4("u_Model", model);
        
        // Color: base, hovered, or selected
        glm::vec3 color;
        if (index == m_selectedIndex) {
            color = glm::vec3(1.0f, 0.8f, 0.2f);  // Yellow for selected
        } else if (index == m_hoveredIndex) {
            color = glm::vec3(0.5f, 0.8f, 1.0f);  // Light blue for hovered
        } else {
            float hue = (float)index / std::max(1, (int)levels.size());
            color = glm::vec3(0.3f + hue * 0.4f, 0.5f, 0.7f - hue * 0.3f);
        }
        m_shader->setVec3("u_Color", color);
        
        m_cubeMesh->render();
        index++;
    }
    
    m_shader->unbind();
}

void Viewport3DPanel::renderLevelView() {
    if (!m_level || !m_shader || !m_cubeMesh) return;
    
    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 projection = m_camera->getProjectionMatrix();
    
    // Clear and rebuild bounds
    m_objectBounds.clear();
    
    m_shader->bind();
    
    // Render each scene as a tile
    const auto& scenes = m_level->getScenes();
    int gridSize = std::max(1, (int)std::ceil(std::sqrt((float)scenes.size())));
    float spacing = 4.0f;
    int index = 0;
    
    for (const auto& scene : scenes) {
        int row = index / gridSize;
        int col = index % gridSize;
        
        float x = (col - gridSize / 2.0f) * spacing;
        float z = (row - gridSize / 2.0f) * spacing;
        glm::vec3 pos(x, 0.1f, z);
        glm::vec3 scale(3.0f, 0.2f, 3.0f);
        
        // Store bounds for picking
        glm::vec3 boxMin = pos - scale * 0.5f;
        glm::vec3 boxMax = pos + scale * 0.5f;
        m_objectBounds.push_back({boxMin, boxMax});
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::scale(model, scale);
        
        glm::mat4 mvp = projection * view * model;
        
        m_shader->setMat4("u_MVP", mvp);
        m_shader->setMat4("u_Model", model);
        
        // Color: base, hovered, or selected
        glm::vec3 color;
        if (index == m_selectedIndex) {
            color = glm::vec3(1.0f, 0.8f, 0.2f);  // Yellow for selected
        } else if (index == m_hoveredIndex) {
            color = glm::vec3(0.5f, 0.8f, 1.0f);  // Light blue for hovered
        } else {
            float hue = (float)index / std::max(1, (int)scenes.size());
            color = glm::vec3(0.4f + hue * 0.3f, 0.6f, 0.5f);
        }
        m_shader->setVec3("u_Color", color);
        
        m_cubeMesh->render();
        index++;
    }
    
    m_shader->unbind();
}

void Viewport3DPanel::renderSceneView() {
    if (!m_shader || !m_cubeMesh) return;
    
    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 projection = m_camera->getProjectionMatrix();
    
    m_shader->bind();
    m_objectBounds.clear();
    m_actorBounds.clear();
    
    if (m_scene) {
        // Render actors from scene
        const auto& actors = m_scene->getActors();
        int index = 0;
        float scale = 0.5f;
        
        for (const auto& actor : actors) {
            if (!actor) continue;
            
            // Get actor position (convert from 2D to 3D)
            auto pos2D = actor->getPosition();
            // Scale down from pixel coords to world coords
            glm::vec3 pos(pos2D.x / 100.0f, scale / 2.0f, pos2D.y / 100.0f);
            
            // Store bounds and actor pointer for picking
            glm::vec3 boundsMin = pos - glm::vec3(scale / 2.0f);
            glm::vec3 boundsMax = pos + glm::vec3(scale / 2.0f);
            m_objectBounds.push_back({boundsMin, boundsMax});
            m_actorBounds.push_back(actor.get());
            
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
            model = glm::scale(model, glm::vec3(scale));
            glm::mat4 mvp = projection * view * model;
            
            m_shader->setMat4("u_MVP", mvp);
            m_shader->setMat4("u_Model", model);
            
            // Color based on selection
            glm::vec3 color;
            if (index == m_selectedIndex) {
                color = glm::vec3(1.0f, 0.8f, 0.2f);  // Yellow for selected
            } else if (index == m_hoveredIndex) {
                color = glm::vec3(0.4f, 0.7f, 1.0f);  // Blue for hovered
            } else {
                // Vary color by actor type/index
                float hue = (float)index / std::max(1, (int)actors.size());
                color = glm::vec3(0.8f - hue * 0.3f, 0.4f + hue * 0.2f, 0.3f + hue * 0.4f);
            }
            m_shader->setVec3("u_Color", color);
            
            m_cubeMesh->render();
            index++;
        }
        
        // If no actors, show placeholder
        if (actors.empty()) {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.25f, 0.0f));
            model = glm::scale(model, glm::vec3(0.5f));
            glm::mat4 mvp = projection * view * model;
            
            m_shader->setMat4("u_MVP", mvp);
            m_shader->setMat4("u_Model", model);
            m_shader->setVec3("u_Color", glm::vec3(0.5f, 0.5f, 0.5f));
            m_cubeMesh->render();
        }
    } else {
        // No scene - show placeholder
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
        glm::mat4 mvp = projection * view * model;
        
        m_shader->setMat4("u_MVP", mvp);
        m_shader->setMat4("u_Model", model);
        m_shader->setVec3("u_Color", glm::vec3(0.8f, 0.3f, 0.2f));
        m_cubeMesh->render();
    }
    
    m_shader->unbind();
}

void Viewport3DPanel::renderGrid() {
    if (!m_gridShader || !m_gridMesh || !m_camera) return;
    
    m_gridShader->bind();
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = m_camera->getViewProjection() * model;
    
    m_gridShader->setMat4("u_MVP", mvp);
    m_gridShader->setVec3("u_Color", glm::vec3(0.5f, 0.5f, 0.5f));
    
    m_gridMesh->render();
    
    m_gridShader->unbind();
}

void Viewport3DPanel::handleInput() {
    if (!m_viewportHovered || !m_camera) return;
    
    ImGuiIO& io = ImGui::GetIO();
    glm::vec2 mousePos = {io.MousePos.x, io.MousePos.y};
    glm::vec2 delta = mousePos - m_lastMousePos;
    
    // Right mouse button - orbit
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        if (!m_isOrbiting) {
            m_isOrbiting = true;
        } else {
            m_camera->onMouseMove(delta.x, delta.y, true);
        }
    } else {
        m_isOrbiting = false;
    }
    
    // Middle mouse button - pan
    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        if (!m_isPanning) {
            m_isPanning = true;
        } else {
            m_camera->onPan(delta.x, delta.y);
        }
    } else {
        m_isPanning = false;
    }
    
    // Scroll wheel - zoom
    if (io.MouseWheel != 0.0f) {
        m_camera->onMouseScroll(io.MouseWheel);
    }
    
    m_lastMousePos = mousePos;
    
    // Handle picking on left click
    handlePicking();
}

void Viewport3DPanel::handlePicking() {
    if (!m_viewportHovered || !m_camera) return;
    if (m_objectBounds.empty()) return;
    
    ImGuiIO& io = ImGui::GetIO();
    
    // Calculate mouse position relative to viewport
    glm::vec2 mousePos = {io.MousePos.x - m_viewportPos.x, io.MousePos.y - m_viewportPos.y};
    
    // Normalize to [-1, 1]
    float ndcX = (2.0f * mousePos.x) / m_viewportSize.x - 1.0f;
    float ndcY = 1.0f - (2.0f * mousePos.y) / m_viewportSize.y;  // Flip Y
    
    // Create ray from camera
    glm::mat4 invVP = glm::inverse(m_camera->getViewProjection());
    glm::vec4 rayClipNear = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
    glm::vec4 rayClipFar = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
    
    glm::vec4 rayWorldNear = invVP * rayClipNear;
    glm::vec4 rayWorldFar = invVP * rayClipFar;
    rayWorldNear /= rayWorldNear.w;
    rayWorldFar /= rayWorldFar.w;
    
    glm::vec3 rayOrigin = glm::vec3(rayWorldNear);
    glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorldFar - rayWorldNear));
    
    // Find closest hit
    m_hoveredIndex = -1;
    float closestT = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < m_objectBounds.size(); i++) {
        float t;
        if (rayIntersectsAABB(rayOrigin, rayDir, m_objectBounds[i].first, m_objectBounds[i].second, t)) {
            if (t < closestT) {
                closestT = t;
                m_hoveredIndex = (int)i;
            }
        }
    }
    
    // Left click - select
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (m_hoveredIndex >= 0) {
            m_selectedIndex = m_hoveredIndex;
            
            // Get selected actor and sync with SelectionManager
            if (m_viewLevel == View3DLevel::Scene && 
                m_hoveredIndex < (int)m_actorBounds.size() && 
                m_actorBounds[m_hoveredIndex]) {
                m_selectedActor = m_actorBounds[m_hoveredIndex];
                if (m_selectionManager) {
                    std::cout << "[Viewport3DPanel] Actor selected: " << m_selectedActor->getName() << std::endl;
                    m_selectionManager->selectActor(m_selectedActor);
                }
            }
        } else {
            // Click on empty space - deselect
            m_selectedIndex = -1;
            m_selectedActor = nullptr;
            if (m_selectionManager) {
                m_selectionManager->clearSelection();
            }
        }
    }
    
    // Double click - navigate
    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && m_selectedIndex >= 0) {
        m_doubleClicked = true;
    }
}

bool Viewport3DPanel::rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                         const glm::vec3& boxMin, const glm::vec3& boxMax, float& t) {
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();
    
    for (int i = 0; i < 3; i++) {
        if (std::abs(rayDir[i]) < 0.0001f) {
            // Ray is parallel to slab
            if (rayOrigin[i] < boxMin[i] || rayOrigin[i] > boxMax[i]) {
                return false;
            }
        } else {
            float invD = 1.0f / rayDir[i];
            float t1 = (boxMin[i] - rayOrigin[i]) * invD;
            float t2 = (boxMax[i] - rayOrigin[i]) * invD;
            
            if (t1 > t2) std::swap(t1, t2);
            
            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);
            
            if (tMin > tMax) return false;
        }
    }
    
    t = tMin;
    return true;
}

void Viewport3DPanel::setSelectionManager(SelectionManager* selMgr) {
    m_selectionManager = selMgr;
    
    // Register callback to sync selection when changed from hierarchy/other panels
    if (m_selectionManager) {
        m_selectionManager->registerSelectionChangedCallback([this]() {
            syncSelectionFromManager();
        });
    }
}

void Viewport3DPanel::syncSelectionFromManager() {
    if (!m_selectionManager) return;
    
    auto* selectedActor = m_selectionManager->getSelectedActor();
    if (selectedActor != m_selectedActor) {
        m_selectedActor = selectedActor;
        
        // Find matching index in actorBounds
        m_selectedIndex = -1;
        for (size_t i = 0; i < m_actorBounds.size(); i++) {
            if (m_actorBounds[i] == selectedActor) {
                m_selectedIndex = (int)i;
                std::cout << "[Viewport3DPanel] Synced selection: " << selectedActor->getName() 
                          << " at index " << m_selectedIndex << std::endl;
                break;
            }
        }
    }
}

} // namespace editor

#endif // HAS_IMGUI
