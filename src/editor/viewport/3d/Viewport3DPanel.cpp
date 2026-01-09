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
#include "engine/actors/StaticMeshActor.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/actors/Character3DActor.h"
#include "editor/core/EditorPlayMode.h"
#include "engine/utils/Logger.h"

#ifdef HAS_IMGUI

#include <imgui.h>
#include <ImGuizmo.h>
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
        // Follow player during play mode
        if (m_playMode && m_playMode->isPlaying()) {
            if (auto* player = m_playMode->getPlayer()) {
                glm::vec3 playerPos = player->getPosition3D();
                
                // Debug logging
                static int updateLogCounter = 0;
                if (updateLogCounter++ % 60 == 0) {
                    std::cout << "[Viewport3D] Camera follow - playerPos=(" 
                              << playerPos.x << "," << playerPos.y << "," << playerPos.z << ")" << std::endl;
                }
                
                // Smooth camera follow
                glm::vec3 targetPos = playerPos + glm::vec3(0.0f, 3.0f, 5.0f);  // Behind and above player
                glm::vec3 currentPos = m_camera->getPosition();
                
                // Lerp for smooth movement
                float followSpeed = 5.0f * deltaTime;
                glm::vec3 newPos = currentPos + (targetPos - currentPos) * followSpeed;
                
                m_camera->setPosition(newPos);
                m_camera->setTarget(playerPos);  // Look at player
            }
        }
        
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
    
    // Handle drag-drop from Place Actors panel
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ACTOR_TEMPLATE")) {
            std::string templateName(static_cast<const char*>(payload->Data));
            handleActorDrop(templateName);
        }
        ImGui::EndDragDropTarget();
    }
    
    // Render transform gizmo for selected actor (all view levels)
    if (m_selectedActor && m_camera) {
        ImGuizmo::BeginFrame();
        bool gizmoUsed = m_gizmo.render(m_camera.get(), m_selectedActor, m_viewportPos, m_viewportSize);
        if (gizmoUsed) {
            m_isDragging = false;  // Gizmo takes precedence over drag
        }
    }
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
    m_actorBounds.clear();
    
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
        m_actorBounds.push_back(nullptr);  // Level, not actor
        
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
    
    // Render World-level actors (actors owned directly by World)
    float actorScale = 0.5f;
    for (const auto& actor : m_world->getActors()) {
        if (!actor) continue;
        
        auto pos2D = actor->getPosition();
        float posZ = actor->getZ();
        glm::vec3 actorPos(pos2D.x / 100.0f, posZ / 100.0f + actorScale / 2.0f, pos2D.y / 100.0f);
        
        // Store bounds for picking
        glm::vec3 boundsMin = actorPos - glm::vec3(actorScale / 2.0f);
        glm::vec3 boundsMax = actorPos + glm::vec3(actorScale / 2.0f);
        m_objectBounds.push_back({boundsMin, boundsMax});
        m_actorBounds.push_back(actor.get());
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), actorPos);
        model = glm::scale(model, glm::vec3(actorScale));
        glm::mat4 mvp = projection * view * model;
        
        m_shader->setMat4("u_MVP", mvp);
        m_shader->setMat4("u_Model", model);
        
        // Color based on selection
        glm::vec3 color;
        if (actor.get() == m_selectedActor) {
            color = glm::vec3(1.0f, 0.8f, 0.2f);  // Yellow for selected
        } else {
            auto* meshActor = dynamic_cast<engine::StaticMeshActor*>(actor.get());
            if (meshActor) {
                color = meshActor->getMeshColor();
            } else {
                color = glm::vec3(0.8f, 0.4f, 0.3f);  // Default actor color
            }
        }
        m_shader->setVec3("u_Color", color);
        
        m_cubeMesh->render();
    }
    
    m_shader->unbind();
}

void Viewport3DPanel::renderLevelView() {
    if (!m_level || !m_shader || !m_cubeMesh) return;
    
    glm::mat4 view = m_camera->getViewMatrix();
    glm::mat4 projection = m_camera->getProjectionMatrix();
    
    // Clear and rebuild bounds
    m_objectBounds.clear();
    m_actorBounds.clear();
    
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
        m_actorBounds.push_back(nullptr);  // Scene tile, not actor
        
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
    
    // Render Level-level actors (actors owned directly by Level)
    float actorScale = 0.5f;
    for (const auto& actor : m_level->getActors()) {
        if (!actor) continue;
        
        auto pos2D = actor->getPosition();
        float posZ = actor->getZ();
        glm::vec3 actorPos(pos2D.x / 100.0f, posZ / 100.0f + actorScale / 2.0f, pos2D.y / 100.0f);
        
        // Store bounds for picking
        glm::vec3 boundsMin = actorPos - glm::vec3(actorScale / 2.0f);
        glm::vec3 boundsMax = actorPos + glm::vec3(actorScale / 2.0f);
        m_objectBounds.push_back({boundsMin, boundsMax});
        m_actorBounds.push_back(actor.get());
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), actorPos);
        model = glm::scale(model, glm::vec3(actorScale));
        glm::mat4 mvp = projection * view * model;
        
        m_shader->setMat4("u_MVP", mvp);
        m_shader->setMat4("u_Model", model);
        
        // Color based on selection
        glm::vec3 color;
        if (actor.get() == m_selectedActor) {
            color = glm::vec3(1.0f, 0.8f, 0.2f);  // Yellow for selected
        } else {
            auto* meshActor = dynamic_cast<engine::StaticMeshActor*>(actor.get());
            if (meshActor) {
                color = meshActor->getMeshColor();
            } else {
                color = glm::vec3(0.8f, 0.4f, 0.3f);  // Default actor color
            }
        }
        m_shader->setVec3("u_Color", color);
        
        m_cubeMesh->render();
    }
    
    m_shader->unbind();
}

void Viewport3DPanel::renderSceneView() {
    if (!m_shader || !m_cubeMesh) return;
    
    // Debug: Check play mode status
    static int sceneViewLogCounter = 0;
    if (sceneViewLogCounter++ % 120 == 0) {
        std::cout << "[Viewport3D] renderSceneView called - m_playMode=" 
                  << (m_playMode ? "SET" : "NULL")
                  << " isPlaying=" << (m_playMode && m_playMode->isPlaying() ? "YES" : "NO")
                  << " hasPlayer=" << (m_playMode && m_playMode->getPlayer() ? "YES" : "NO")
                  << std::endl;
    }
    
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
            float posZ = actor->getZ();
            // Scale down from pixel coords to world coords
            // X stays X, Z in 3D = Y in 2D, Y in 3D = actor's Z component
            glm::vec3 pos(pos2D.x / 100.0f, posZ / 100.0f + scale / 2.0f, pos2D.y / 100.0f);
            
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
            
            // Check if this is a StaticMeshActor for custom rendering
            auto* meshActor = dynamic_cast<engine::StaticMeshActor*>(actor.get());
            
            // Color based on selection or actor type
            glm::vec3 color;
            if (index == m_selectedIndex || actor.get() == m_selectedActor) {
                color = glm::vec3(1.0f, 0.8f, 0.2f);  // Yellow for selected
            } else if (index == m_hoveredIndex) {
                color = glm::vec3(0.4f, 0.7f, 1.0f);  // Blue for hovered
            } else if (meshActor) {
                // Use mesh color from StaticMeshActor
                color = meshActor->getMeshColor();
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
    
    // Render player during play mode
    if (m_playMode && m_playMode->isPlaying()) {
        if (auto* player = m_playMode->getPlayer()) {
            glm::vec3 playerPos = player->getPosition3D();
            
            // Debug: Log player rendering
            static int renderLogCounter = 0;
            if (renderLogCounter++ % 60 == 0) {
                std::cout << "[Viewport3D] Rendering player at (" 
                          << playerPos.x << "," << playerPos.y << "," << playerPos.z << ")" << std::endl;
            }
            
            // Render player as a colored capsule/box with rotation
            glm::mat4 model = glm::translate(glm::mat4(1.0f), playerPos);
            model = glm::rotate(model, glm::radians(player->getYaw()), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate around Y-axis
            model = glm::scale(model, glm::vec3(0.4f, 0.9f, 0.4f));  // Capsule-ish
            glm::mat4 mvp = projection * view * model;
            
            m_shader->setMat4("u_MVP", mvp);
            m_shader->setMat4("u_Model", model);
            m_shader->setVec3("u_Color", glm::vec3(0.2f, 0.8f, 0.2f));  // Green for player
            m_cubeMesh->render();
        } else {
            static bool loggedOnce = false;
            if (!loggedOnce) {
                std::cout << "[Viewport3D] WARNING: m_playMode->getPlayer() returned null!" << std::endl;
                loggedOnce = true;
            }
        }
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
    
    bool rmb = ImGui::IsMouseDown(ImGuiMouseButton_Right);
    bool lmb = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool mmb = ImGui::IsMouseDown(ImGuiMouseButton_Middle);
    bool alt = io.KeyAlt;
    
    // ===== Unreal Engine-style Navigation =====
    
    // Debug log input state
    static int logCounter = 0;
    if (rmb || lmb || mmb || alt) {
        if (logCounter++ % 30 == 0) {  // Log every 30 frames to avoid spam
            std::cout << "[Viewport3D Input] RMB:" << rmb << " LMB:" << lmb 
                      << " MMB:" << mmb << " Alt:" << alt 
                      << " Delta:(" << delta.x << "," << delta.y << ")" << std::endl;
        }
    }
    
    // Check if in play mode - disable camera fly mode during play
    bool isPlayMode = m_playMode && m_playMode->isPlaying();
    
    // RMB + Mouse during play mode: Rotate player
    if (rmb && !alt && !lmb && isPlayMode) {
        if (delta.x != 0 || delta.y != 0) {
            std::cout << "[Viewport3D] Mouse look during play mode - delta=(" 
                      << delta.x << "," << delta.y << ")" << std::endl;
            m_playMode->handleMouseLook(delta.x, delta.y);
        }
    }
    // RMB + Mouse: Look around (fly mode rotation) - disabled during play mode
    else if (rmb && !alt && !lmb && !isPlayMode) {
        if (!m_isOrbiting) {
            m_isOrbiting = true;
            std::cout << "[Viewport3D] Started fly mode (RMB)" << std::endl;
        } else if (delta.x != 0 || delta.y != 0) {
            m_camera->onLookAround(delta.x, delta.y);
        }
    } else {
        if (m_isOrbiting) {
            std::cout << "[Viewport3D] Ended fly mode" << std::endl;
        }
        m_isOrbiting = false;
    }
    
    // Alt + LMB: Orbit around focus point
    if (alt && lmb && !rmb) {
        if (delta.x != 0 || delta.y != 0) {
            std::cout << "[Viewport3D] Orbit (Alt+LMB)" << std::endl;
            m_camera->onOrbit(delta.x, delta.y);
        }
    }
    
    // Alt + RMB: Dolly (zoom in/out)
    if (alt && rmb && !lmb) {
        if (delta.y != 0) {
            std::cout << "[Viewport3D] Dolly (Alt+RMB) delta:" << delta.y << std::endl;
            m_camera->onDolly(-delta.y * 0.05f);
        }
    }
    
    // LMB + RMB: Dolly (forward/backward)
    if (lmb && rmb && !alt) {
        if (delta.y != 0) {
            std::cout << "[Viewport3D] Dolly (LMB+RMB) delta:" << delta.y << std::endl;
            m_camera->onDolly(-delta.y * 0.05f);
        }
    }
    
    // MMB or Alt + MMB: Pan
    if (mmb || (alt && mmb)) {
        if (!m_isPanning) {
            m_isPanning = true;
            std::cout << "[Viewport3D] Started panning (MMB)" << std::endl;
        } else if (delta.x != 0 || delta.y != 0) {
            m_camera->onPan(delta.x, delta.y);
        }
    } else {
        if (m_isPanning) {
            std::cout << "[Viewport3D] Ended panning" << std::endl;
        }
        m_isPanning = false;
    }
    
    // Scroll wheel
    if (io.MouseWheel != 0.0f) {
        if (rmb) {
            // RMB + Scroll: Adjust fly speed
            std::cout << "[Viewport3D] Adjust speed (RMB+Scroll): " << io.MouseWheel << std::endl;
            m_camera->adjustMoveSpeed(io.MouseWheel);
        } else {
            // Normal scroll: Zoom
            std::cout << "[Viewport3D] Zoom (Scroll): " << io.MouseWheel << std::endl;
            m_camera->onMouseScroll(io.MouseWheel);
        }
    }
    
    // During play mode: WASD controls player via SDL (no camera control)
    // Outside play mode: RMB + WASD controls camera
    if (!isPlayMode && rmb && m_viewportFocused) {
        // Camera fly mode (only when NOT in play mode)
        float forward = 0.0f, right = 0.0f, up = 0.0f;
        
        if (ImGui::IsKeyDown(ImGuiKey_W)) forward += 1.0f;
        if (ImGui::IsKeyDown(ImGuiKey_S)) forward -= 1.0f;
        if (ImGui::IsKeyDown(ImGuiKey_D)) right += 1.0f;
        if (ImGui::IsKeyDown(ImGuiKey_A)) right -= 1.0f;
        if (ImGui::IsKeyDown(ImGuiKey_E)) up += 1.0f;
        if (ImGui::IsKeyDown(ImGuiKey_Q)) up -= 1.0f;
        
        if (forward != 0.0f || right != 0.0f || up != 0.0f) {
            std::cout << "[Viewport3D] WASD move: F=" << forward << " R=" << right << " U=" << up << std::endl;
            m_camera->onKeyboardMove(forward, right, up, io.DeltaTime);
        }
    }
    
    // F key: Focus on selected actor
    if (ImGui::IsKeyPressed(ImGuiKey_F) && m_selectedActor) {
        auto pos2D = m_selectedActor->getPosition();
        float posZ = m_selectedActor->getZ();
        glm::vec3 targetPos(pos2D.x / 100.0f, posZ / 100.0f, pos2D.y / 100.0f);
        std::cout << "[Viewport3D] Focus on actor at (" << targetPos.x << "," << targetPos.y << "," << targetPos.z << ")" << std::endl;
        m_camera->focusOn(targetPos, 5.0f);
    }
    
    m_lastMousePos = mousePos;
    
    // Handle picking on left click (only if not holding Alt for orbit)
    if (!alt) {
        handlePicking();
    }
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
    
    // Left click - select and start drag
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (m_hoveredIndex >= 0) {
            m_selectedIndex = m_hoveredIndex;
            
            // Get selected actor and sync with SelectionManager (works for all view levels)
            if (m_hoveredIndex < (int)m_actorBounds.size() && 
                m_actorBounds[m_hoveredIndex]) {
                m_selectedActor = m_actorBounds[m_hoveredIndex];
                if (m_selectionManager) {
                    std::cout << "[Viewport3DPanel] Actor selected: " << m_selectedActor->getName() << std::endl;
                    m_selectionManager->selectActor(m_selectedActor);
                }
                
                // Start drag - get initial positions
                auto pos2D = m_selectedActor->getPosition();
                float scale = 0.5f;
                m_actorStartPos = glm::vec3(pos2D.x / 100.0f, scale / 2.0f, pos2D.y / 100.0f);
                m_dragStartPos = getWorldPosOnPlane(m_actorStartPos.y);
                m_isDragging = true;
                m_dragAxis = -1;  // XZ plane by default
            }
        } else {
            // Click on empty space - deselect
            m_selectedIndex = -1;
            m_selectedActor = nullptr;
            m_isDragging = false;
            if (m_selectionManager) {
                m_selectionManager->clearSelection();
            }
        }
    }
    
    // Release drag
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        m_isDragging = false;
    }
    
    // Handle dragging
    if (m_isDragging && m_selectedActor) {
        handleDragging();
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

glm::vec3 Viewport3DPanel::getWorldPosOnPlane(float planeY) {
    if (!m_camera) return glm::vec3(0.0f);
    
    ImGuiIO& io = ImGui::GetIO();
    glm::vec2 mousePos = {io.MousePos.x - m_viewportPos.x, io.MousePos.y - m_viewportPos.y};
    
    // Normalize to [-1, 1]
    float ndcX = (2.0f * mousePos.x) / m_viewportSize.x - 1.0f;
    float ndcY = 1.0f - (2.0f * mousePos.y) / m_viewportSize.y;
    
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
    
    // Intersect with Y=planeY plane
    if (std::abs(rayDir.y) < 0.0001f) {
        // Ray parallel to plane
        return glm::vec3(rayOrigin.x, planeY, rayOrigin.z);
    }
    
    float t = (planeY - rayOrigin.y) / rayDir.y;
    return rayOrigin + rayDir * t;
}

void Viewport3DPanel::handleDragging() {
    if (!m_selectedActor || !m_isDragging) return;
    
    // Get current world position on drag plane
    glm::vec3 currentWorldPos = getWorldPosOnPlane(m_actorStartPos.y);
    
    // Calculate delta from drag start
    glm::vec3 delta = currentWorldPos - m_dragStartPos;
    
    // Hold Shift for Y-axis movement (vertical = Z in actor coordinates)
    ImGuiIO& io = ImGui::GetIO();
    if (io.KeyShift) {
        // Y-axis drag (3D) = Z-axis (actor) - use vertical mouse movement
        float yDelta = -io.MouseDelta.y * 1.0f;  // 1 pixel = 1 unit
        
        // Update actor's Z position
        float newZ = m_selectedActor->getZ() + yDelta;
        m_selectedActor->setZ(newZ);
        
        // Update drag start for continuous movement
        m_actorStartPos.y = newZ / 100.0f + 0.25f;
    } else {
        // XZ plane drag - moves actor in X/Y space
        glm::vec3 newPos3D = m_actorStartPos + delta;
        
        // Convert 3D world position back to 2D pixel coordinates
        float newX = newPos3D.x * 100.0f;
        float newY = newPos3D.z * 100.0f;  // Z in 3D = Y in 2D
        
        // Update actor position (keep Z unchanged)
        m_selectedActor->setPosition(newX, newY);
    }
}

void Viewport3DPanel::handleActorDrop(const std::string& templateName) {
    // Add actor to current view level container
    engine::WorldContainer* targetContainer = nullptr;
    std::string containerName;
    
    switch (m_viewLevel) {
        case View3DLevel::World:
            if (m_world) {
                targetContainer = m_world;
                containerName = "World: " + m_world->getName();
            }
            break;
        case View3DLevel::Level:
            if (m_level) {
                targetContainer = m_level;
                containerName = "Level: " + m_level->getName();
            }
            break;
        case View3DLevel::Scene:
            if (m_scene) {
                targetContainer = m_scene;
                containerName = "Scene: " + m_scene->getName();
            }
            break;
    }
    
    if (!targetContainer) {
        LOG_WARNING("[Viewport3D] Cannot drop actor - no container available for current view level");
        return;
    }
    
    LOG_INFO("[Viewport3D] Dropping actor to " + containerName + ": " + templateName);
    
    // Calculate drop position in 3D world (in front of camera)
    glm::vec3 dropPos(0.0f, 2.0f, 0.0f);  // Default position above ground
    if (m_camera) {
        glm::vec3 camPos = m_camera->getPosition();
        glm::vec3 target = m_camera->getTarget();
        glm::vec3 forward = glm::normalize(target - camPos);
        dropPos = camPos + forward * 5.0f;
    }
    
    // Parse template name for StaticMeshActor variants
    // Format: "StaticMeshActor:Primitive:BodyType" e.g., "StaticMeshActor:Cube:Dynamic"
    if (templateName.find("StaticMeshActor") != std::string::npos || 
        templateName == "Static Mesh" ||
        templateName == "Physics Cube" ||
        templateName == "Physics Sphere" ||
        templateName == "Static Platform") {
        
        // Generate unique name
        static int actorCounter = 0;
        std::string actorName = "StaticMesh_" + std::to_string(++actorCounter);
        
        auto actor = std::make_unique<engine::StaticMeshActor>(actorName);
        
        // Configure based on template
        if (templateName == "Physics Cube" || templateName.find(":Cube:Dynamic") != std::string::npos) {
            actor->setMeshPrimitive(engine::PrimitiveMeshType::Cube);
            actor->setMeshColor(glm::vec3(0.8f, 0.4f, 0.1f));  // Orange
            actor->setPhysicsEnabled(true);
            actor->setBodyType(engine::physics::BodyType::Dynamic);
            actor->setMass(1.0f);
            actor->setUseGravity(true);
            LOG_INFO("[Viewport3D] Created Physics Cube (Dynamic)");
        }
        else if (templateName == "Physics Sphere" || templateName.find(":Sphere:Dynamic") != std::string::npos) {
            actor->setMeshPrimitive(engine::PrimitiveMeshType::Sphere);
            actor->setMeshColor(glm::vec3(0.1f, 0.6f, 0.8f));  // Cyan
            actor->setPhysicsEnabled(true);
            actor->setBodyType(engine::physics::BodyType::Dynamic);
            actor->setMass(1.0f);
            actor->setUseGravity(true);
            LOG_INFO("[Viewport3D] Created Physics Sphere (Dynamic)");
        }
        else if (templateName == "Static Platform" || templateName.find(":Cube:Static") != std::string::npos) {
            actor->setMeshPrimitive(engine::PrimitiveMeshType::Cube);
            actor->setMeshScale(glm::vec3(4.0f, 0.5f, 4.0f));  // Wide platform
            actor->setMeshColor(glm::vec3(0.3f, 0.3f, 0.35f)); // Gray
            actor->setPhysicsEnabled(true);
            actor->setBodyType(engine::physics::BodyType::Static);
            dropPos.y = 0.0f;  // Place on ground
            LOG_INFO("[Viewport3D] Created Static Platform");
        }
        else {
            // Default Static Mesh
            actor->setMeshPrimitive(engine::PrimitiveMeshType::Cube);
            actor->setMeshColor(glm::vec3(0.6f, 0.6f, 0.7f));
            LOG_INFO("[Viewport3D] Created Static Mesh (no physics)");
        }
        
        // Set position (setPosition3D handles the coordinate mapping)
        actor->setPosition3D(dropPos);
        
        // Add to target container (World, Level, or Scene)
        engine::ActorObjectExtended* actorPtr = actor.get();
        targetContainer->addActor(std::move(actor));
        
        // Select the new actor
        m_selectedActor = actorPtr;
        if (m_selectionManager) {
            m_selectionManager->selectActor(actorPtr);
        }
        
        LOG_INFO("[Viewport3D] Actor added to " + containerName + " at (" + 
                 std::to_string(dropPos.x) + ", " + 
                 std::to_string(dropPos.y) + ", " + 
                 std::to_string(dropPos.z) + ")");
    }
    else if (templateName == "PlayerStart" || templateName == "Player Start") {
        // Create PlayerStart actor
        static int playerStartCounter = 0;
        std::string actorName = "PlayerStart_" + std::to_string(++playerStartCounter);
        
        auto actor = std::make_unique<engine::PlayerStartActor>(actorName);
        actor->setSpawnPosition(dropPos);
        
        engine::ActorObjectExtended* actorPtr = actor.get();
        targetContainer->addActor(std::move(actor));
        
        m_selectedActor = actorPtr;
        if (m_selectionManager) {
            m_selectionManager->selectActor(actorPtr);
        }
        
        LOG_INFO("[Viewport3D] Created PlayerStart at (" + 
                 std::to_string(dropPos.x) + ", " + 
                 std::to_string(dropPos.y) + ", " + 
                 std::to_string(dropPos.z) + ")");
    }
    else if (templateName == "Player3D" || templateName == "Player 3D") {
        // Create Player3D actor
        auto actor = std::make_unique<engine::Player3DActor>();
        actor->setPosition3D(dropPos);
        
        engine::ActorObjectExtended* actorPtr = actor.get();
        targetContainer->addActor(std::move(actor));
        
        m_selectedActor = actorPtr;
        if (m_selectionManager) {
            m_selectionManager->selectActor(actorPtr);
        }
        
        LOG_INFO("[Viewport3D] Created Player3D at (" + 
                 std::to_string(dropPos.x) + ", " + 
                 std::to_string(dropPos.y) + ", " + 
                 std::to_string(dropPos.z) + ")");
    }
    else {
        LOG_WARNING("[Viewport3D] Unknown template: " + templateName);
    }
}

engine::Level* Viewport3DPanel::getSelectedLevel() const {
    // Only valid in World view when a level is selected
    if (m_viewLevel != View3DLevel::World || m_selectedIndex < 0 || !m_world) {
        return nullptr;
    }
    
    const auto& levels = m_world->getLevels();
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(levels.size())) {
        return levels[m_selectedIndex].get();
    }
    return nullptr;
}

engine::Scene* Viewport3DPanel::getSelectedScene() const {
    // Only valid in Level view when a scene is selected
    if (m_viewLevel != View3DLevel::Level || m_selectedIndex < 0 || !m_level) {
        return nullptr;
    }
    
    const auto& scenes = m_level->getScenes();
    if (m_selectedIndex >= 0 && m_selectedIndex < static_cast<int>(scenes.size())) {
        return scenes[m_selectedIndex].get();
    }
    return nullptr;
}

void Viewport3DPanel::setPlayMode(EditorPlayMode* playMode) {
    std::cout << "[Viewport3DPanel] setPlayMode called - playMode=" 
              << (playMode ? "SET" : "NULL") << std::endl;
    m_playMode = playMode;
}

} // namespace editor

#endif // HAS_IMGUI
