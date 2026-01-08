/**
 * @file ViewportRenderer.cpp
 * @brief Unified viewport renderer implementation
 */
#include "ViewportRenderer.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"

#ifdef HAS_IMGUI

#include <imgui.h>
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <limits>
#include <algorithm>

namespace editor {

// Shaders (same as Viewport3DPanel)
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

static const char* GRID_VERTEX_SHADER = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 u_MVP;
void main() {
    gl_Position = u_MVP * vec4(aPos, 1.0);
}
)";

static const char* GRID_FRAGMENT_SHADER = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 u_Color;
void main() {
    FragColor = vec4(u_Color, 1.0);
}
)";

ViewportRenderer::ViewportRenderer() = default;
ViewportRenderer::~ViewportRenderer() = default;

bool ViewportRenderer::initialize() {
    if (m_initialized) return true;
    
    // Create 3D camera
    m_camera3D = std::make_unique<EditorCamera3D>();
    m_camera3D->focusOn(glm::vec3(0.0f), 10.0f);
    
    // Create framebuffer
    engine::FramebufferSpec fbSpec;
    fbSpec.width = 800;
    fbSpec.height = 600;
    fbSpec.hasDepth = true;
    m_framebuffer = std::make_unique<engine::Framebuffer>(fbSpec);
    
    // Create shaders
    if (!createShaders()) {
        return false;
    }
    
    // Create meshes
    m_gridMesh = engine::Mesh::createGrid(20, 1.0f);
    m_cubeMesh = engine::Mesh::createCube();
    
    m_initialized = true;
    return true;
}

bool ViewportRenderer::createShaders() {
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

void ViewportRenderer::update(float deltaTime) {
    // Update camera or animations if needed
}

void ViewportRenderer::render(ImDrawList* drawList, const glm::vec2& viewportPos, const glm::vec2& viewportSize) {
    m_viewportPos = viewportPos;
    m_viewportSize = viewportSize;
    m_pickableObjects.clear();
    
    if (m_renderMode == RenderMode::Mode2D) {
        render2D(drawList, viewportPos, viewportSize);
    } else {
        render3D(viewportPos, viewportSize);
    }
}

// ============================================================================
// 2D RENDERING
// ============================================================================

void ViewportRenderer::render2D(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size) {
    // Background
    drawList->AddRectFilled(
        ImVec2(offset.x, offset.y),
        ImVec2(offset.x + size.x, offset.y + size.y),
        IM_COL32(30, 35, 45, 255)
    );
    
    // Grid
    render2DGrid(drawList, offset, size);
    
    // Content based on hierarchy level
    switch (m_hierarchyLevel) {
        case HierarchyLevel::World:
            render2DWorld(drawList, offset, size);
            break;
        case HierarchyLevel::Level:
            render2DLevel(drawList, offset, size);
            break;
        case HierarchyLevel::Scene:
            render2DScene(drawList, offset, size);
            break;
    }
}

void ViewportRenderer::render2DWorld(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size) {
    if (!m_world) return;
    
    const auto& levels = m_world->getLevels();
    float spacing = 150.0f * m_zoom2D;
    int index = 0;
    
    for (const auto& level : levels) {
        float x = offset.x + size.x / 2.0f + (index - (int)levels.size() / 2.0f) * spacing + m_pan2D.x;
        float y = offset.y + size.y / 2.0f + m_pan2D.y;
        float boxW = 120.0f * m_zoom2D;
        float boxH = 80.0f * m_zoom2D;
        
        // Store bounds for picking
        PickableObject obj;
        obj.boundsMin = glm::vec3(x - boxW/2, y - boxH/2, 0);
        obj.boundsMax = glm::vec3(x + boxW/2, y + boxH/2, 0);
        obj.userData = level.get();
        obj.index = index;
        m_pickableObjects.push_back(obj);
        
        // Colors
        ImU32 fillColor = (index == m_selectedIndex) ? IM_COL32(255, 200, 50, 255) :
                          (index == m_hoveredIndex) ? IM_COL32(100, 180, 255, 255) :
                          IM_COL32(60, 100, 150, 255);
        ImU32 borderColor = IM_COL32(100, 150, 200, 255);
        
        drawList->AddRectFilled(
            ImVec2(x - boxW/2, y - boxH/2),
            ImVec2(x + boxW/2, y + boxH/2),
            fillColor, 4.0f
        );
        drawList->AddRect(
            ImVec2(x - boxW/2, y - boxH/2),
            ImVec2(x + boxW/2, y + boxH/2),
            borderColor, 4.0f, 0, 2.0f
        );
        
        // Label
        const char* name = level->getName().c_str();
        ImVec2 textSize = ImGui::CalcTextSize(name);
        drawList->AddText(
            ImVec2(x - textSize.x/2, y - textSize.y/2),
            IM_COL32(255, 255, 255, 255), name
        );
        
        index++;
    }
}

void ViewportRenderer::render2DLevel(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size) {
    if (!m_activeLevel) return;
    
    const auto& scenes = m_activeLevel->getScenes();
    int gridSize = std::max(1, (int)std::ceil(std::sqrt((float)scenes.size())));
    float spacing = 120.0f * m_zoom2D;
    int index = 0;
    
    for (const auto& scene : scenes) {
        int row = index / gridSize;
        int col = index % gridSize;
        
        float x = offset.x + size.x / 2.0f + (col - gridSize / 2.0f) * spacing + m_pan2D.x;
        float y = offset.y + size.y / 2.0f + (row - gridSize / 2.0f) * spacing + m_pan2D.y;
        float boxW = 100.0f * m_zoom2D;
        float boxH = 70.0f * m_zoom2D;
        
        // Store bounds for picking
        PickableObject obj;
        obj.boundsMin = glm::vec3(x - boxW/2, y - boxH/2, 0);
        obj.boundsMax = glm::vec3(x + boxW/2, y + boxH/2, 0);
        obj.userData = scene.get();
        obj.index = index;
        m_pickableObjects.push_back(obj);
        
        // Colors
        ImU32 fillColor = (index == m_selectedIndex) ? IM_COL32(255, 200, 50, 255) :
                          (index == m_hoveredIndex) ? IM_COL32(100, 180, 255, 255) :
                          IM_COL32(80, 120, 80, 255);
        ImU32 borderColor = IM_COL32(120, 180, 120, 255);
        
        drawList->AddRectFilled(
            ImVec2(x - boxW/2, y - boxH/2),
            ImVec2(x + boxW/2, y + boxH/2),
            fillColor, 4.0f
        );
        drawList->AddRect(
            ImVec2(x - boxW/2, y - boxH/2),
            ImVec2(x + boxW/2, y + boxH/2),
            borderColor, 4.0f, 0, 2.0f
        );
        
        // Label
        const char* name = scene->getName().c_str();
        ImVec2 textSize = ImGui::CalcTextSize(name);
        float fontSize = std::min(1.0f, boxW / (textSize.x + 10.0f));
        drawList->AddText(
            ImVec2(x - textSize.x/2 * fontSize, y - textSize.y/2),
            IM_COL32(255, 255, 255, 255), name
        );
        
        index++;
    }
}

void ViewportRenderer::render2DScene(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size) {
    if (!m_activeScene) return;
    
    // TODO: Render actors from scene
    // For now, show placeholder
    const char* sceneName = m_activeScene->getName().c_str();
    ImVec2 textSize = ImGui::CalcTextSize(sceneName);
    drawList->AddText(
        ImVec2(offset.x + size.x/2 - textSize.x/2, offset.y + 20),
        IM_COL32(200, 200, 200, 255), sceneName
    );
    
    // Get actors and render them
    // const auto& actors = m_activeScene->getActors();
    // render2DActors(drawList, offset, actors);
}

void ViewportRenderer::render2DActors(ImDrawList* drawList, const glm::vec2& offset,
                                       const std::vector<engine::ActorObjectExtended*>& actors) {
    int index = 0;
    for (const auto* actor : actors) {
        if (!actor) continue;
        
        glm::vec2 pos = glm::vec2(actor->getPosition().x, actor->getPosition().y);
        pos = pos * m_zoom2D + m_pan2D + offset;
        
        float actorSize = 32.0f * m_zoom2D;
        
        // Store bounds
        PickableObject obj;
        obj.boundsMin = glm::vec3(pos.x - actorSize/2, pos.y - actorSize/2, 0);
        obj.boundsMax = glm::vec3(pos.x + actorSize/2, pos.y + actorSize/2, 0);
        obj.userData = const_cast<engine::ActorObjectExtended*>(actor);
        obj.index = index;
        m_pickableObjects.push_back(obj);
        
        // Draw actor box
        ImU32 color = (index == m_selectedIndex) ? IM_COL32(255, 200, 50, 255) :
                      (index == m_hoveredIndex) ? IM_COL32(100, 180, 255, 255) :
                      IM_COL32(200, 100, 100, 255);
        
        drawList->AddRectFilled(
            ImVec2(pos.x - actorSize/2, pos.y - actorSize/2),
            ImVec2(pos.x + actorSize/2, pos.y + actorSize/2),
            color, 2.0f
        );
        
        index++;
    }
}

void ViewportRenderer::render2DGrid(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size) {
    float gridSize = 32.0f * m_zoom2D;
    ImU32 gridColor = IM_COL32(50, 55, 65, 255);
    
    float startX = std::fmod(m_pan2D.x, gridSize);
    float startY = std::fmod(m_pan2D.y, gridSize);
    
    for (float x = startX; x < size.x; x += gridSize) {
        drawList->AddLine(
            ImVec2(offset.x + x, offset.y),
            ImVec2(offset.x + x, offset.y + size.y),
            gridColor
        );
    }
    for (float y = startY; y < size.y; y += gridSize) {
        drawList->AddLine(
            ImVec2(offset.x, offset.y + y),
            ImVec2(offset.x + size.x, offset.y + y),
            gridColor
        );
    }
}

// ============================================================================
// 3D RENDERING
// ============================================================================

void ViewportRenderer::render3D(const glm::vec2& viewportPos, const glm::vec2& viewportSize) {
    if (!m_initialized) {
        if (!initialize()) return;
    }
    
    // Resize framebuffer if needed
    if (m_viewportSize.x > 0 && m_viewportSize.y > 0) {
        if (m_framebuffer->getWidth() != (int)m_viewportSize.x ||
            m_framebuffer->getHeight() != (int)m_viewportSize.y) {
            m_framebuffer->resize((int)m_viewportSize.x, (int)m_viewportSize.y);
            m_camera3D->setAspectRatio(m_viewportSize.x / m_viewportSize.y);
        }
    }
    
    // Render to framebuffer
    m_framebuffer->bind();
    glViewport(0, 0, m_framebuffer->getWidth(), m_framebuffer->getHeight());
    glClearColor(0.1f, 0.12f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    render3DGrid();
    
    switch (m_hierarchyLevel) {
        case HierarchyLevel::World:
            render3DWorld();
            break;
        case HierarchyLevel::Level:
            render3DLevel();
            break;
        case HierarchyLevel::Scene:
            render3DScene();
            break;
    }
    
    m_framebuffer->unbind();
    
    // Display framebuffer as ImGui image
    ImGui::Image(
        (ImTextureID)(uintptr_t)m_framebuffer->getColorAttachment(),
        ImVec2(m_viewportSize.x, m_viewportSize.y),
        ImVec2(0, 1), ImVec2(1, 0)
    );
}

void ViewportRenderer::render3DWorld() {
    if (!m_world || !m_shader || !m_cubeMesh) return;
    
    glm::mat4 view = m_camera3D->getViewMatrix();
    glm::mat4 projection = m_camera3D->getProjectionMatrix();
    
    m_shader->bind();
    
    const auto& levels = m_world->getLevels();
    float spacing = 5.0f;
    int index = 0;
    
    for (const auto& level : levels) {
        float x = (index - (int)levels.size() / 2.0f) * spacing;
        glm::vec3 pos(x, 0.5f, 0.0f);
        glm::vec3 scale(2.0f, 1.0f, 2.0f);
        
        // Store bounds
        PickableObject obj;
        obj.boundsMin = pos - scale * 0.5f;
        obj.boundsMax = pos + scale * 0.5f;
        obj.userData = level.get();
        obj.index = index;
        m_pickableObjects.push_back(obj);
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, scale);
        glm::mat4 mvp = projection * view * model;
        
        m_shader->setMat4("u_MVP", mvp);
        m_shader->setMat4("u_Model", model);
        
        glm::vec3 color = getColorForIndex(index, (int)levels.size(), 
                                            index == m_selectedIndex, index == m_hoveredIndex);
        m_shader->setVec3("u_Color", color);
        
        m_cubeMesh->render();
        index++;
    }
    
    m_shader->unbind();
}

void ViewportRenderer::render3DLevel() {
    if (!m_activeLevel || !m_shader || !m_cubeMesh) return;
    
    glm::mat4 view = m_camera3D->getViewMatrix();
    glm::mat4 projection = m_camera3D->getProjectionMatrix();
    
    m_shader->bind();
    
    const auto& scenes = m_activeLevel->getScenes();
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
        
        // Store bounds
        PickableObject obj;
        obj.boundsMin = pos - scale * 0.5f;
        obj.boundsMax = pos + scale * 0.5f;
        obj.userData = scene.get();
        obj.index = index;
        m_pickableObjects.push_back(obj);
        
        glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
        model = glm::scale(model, scale);
        glm::mat4 mvp = projection * view * model;
        
        m_shader->setMat4("u_MVP", mvp);
        m_shader->setMat4("u_Model", model);
        
        glm::vec3 color = getColorForIndex(index, (int)scenes.size(),
                                            index == m_selectedIndex, index == m_hoveredIndex);
        m_shader->setVec3("u_Color", color);
        
        m_cubeMesh->render();
        index++;
    }
    
    m_shader->unbind();
}

void ViewportRenderer::render3DScene() {
    if (!m_shader || !m_cubeMesh) return;
    
    glm::mat4 view = m_camera3D->getViewMatrix();
    glm::mat4 projection = m_camera3D->getProjectionMatrix();
    
    // Render placeholder cube for now
    m_shader->bind();
    
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 0.0f));
    glm::mat4 mvp = projection * view * model;
    
    m_shader->setMat4("u_MVP", mvp);
    m_shader->setMat4("u_Model", model);
    m_shader->setVec3("u_Color", glm::vec3(0.8f, 0.3f, 0.2f));
    
    m_cubeMesh->render();
    
    m_shader->unbind();
}

void ViewportRenderer::render3DActors(const std::vector<engine::ActorObjectExtended*>& actors) {
    // TODO: Render actors as 3D billboards or meshes
}

void ViewportRenderer::render3DGrid() {
    if (!m_gridShader || !m_gridMesh || !m_camera3D) return;
    
    m_gridShader->bind();
    
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 mvp = m_camera3D->getViewProjection() * model;
    
    m_gridShader->setMat4("u_MVP", mvp);
    m_gridShader->setVec3("u_Color", glm::vec3(0.4f, 0.4f, 0.4f));
    
    m_gridMesh->render();
    
    m_gridShader->unbind();
}

// ============================================================================
// PICKING
// ============================================================================

int ViewportRenderer::pickObjectAt(const glm::vec2& screenPos, const glm::vec2& viewportPos, 
                                    const glm::vec2& viewportSize) {
    m_hoveredIndex = -1;
    
    if (m_renderMode == RenderMode::Mode2D) {
        // 2D picking - simple bounds test
        for (const auto& obj : m_pickableObjects) {
            if (screenPos.x >= obj.boundsMin.x && screenPos.x <= obj.boundsMax.x &&
                screenPos.y >= obj.boundsMin.y && screenPos.y <= obj.boundsMax.y) {
                m_hoveredIndex = obj.index;
                return obj.index;
            }
        }
    } else {
        // 3D picking - ray casting
        if (!m_camera3D) return -1;
        
        glm::vec2 mousePos = screenPos - viewportPos;
        float ndcX = (2.0f * mousePos.x) / viewportSize.x - 1.0f;
        float ndcY = 1.0f - (2.0f * mousePos.y) / viewportSize.y;
        
        glm::mat4 invVP = glm::inverse(m_camera3D->getViewProjection());
        glm::vec4 rayClipNear = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
        glm::vec4 rayClipFar = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);
        
        glm::vec4 rayWorldNear = invVP * rayClipNear;
        glm::vec4 rayWorldFar = invVP * rayClipFar;
        rayWorldNear /= rayWorldNear.w;
        rayWorldFar /= rayWorldFar.w;
        
        glm::vec3 rayOrigin = glm::vec3(rayWorldNear);
        glm::vec3 rayDir = glm::normalize(glm::vec3(rayWorldFar - rayWorldNear));
        
        float closestT = std::numeric_limits<float>::max();
        
        for (const auto& obj : m_pickableObjects) {
            float t;
            if (rayIntersectsAABB(rayOrigin, rayDir, obj.boundsMin, obj.boundsMax, t)) {
                if (t < closestT) {
                    closestT = t;
                    m_hoveredIndex = obj.index;
                }
            }
        }
    }
    
    return m_hoveredIndex;
}

bool ViewportRenderer::rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                                          const glm::vec3& boxMin, const glm::vec3& boxMax, float& t) {
    float tMin = 0.0f;
    float tMax = std::numeric_limits<float>::max();
    
    for (int i = 0; i < 3; i++) {
        if (std::abs(rayDir[i]) < 0.0001f) {
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

glm::vec3 ViewportRenderer::getColorForIndex(int index, int total, bool selected, bool hovered) {
    if (selected) return glm::vec3(1.0f, 0.8f, 0.2f);
    if (hovered) return glm::vec3(0.5f, 0.8f, 1.0f);
    
    float hue = (float)index / std::max(1, total);
    return glm::vec3(0.3f + hue * 0.4f, 0.5f, 0.7f - hue * 0.3f);
}

} // namespace editor

#endif // HAS_IMGUI
