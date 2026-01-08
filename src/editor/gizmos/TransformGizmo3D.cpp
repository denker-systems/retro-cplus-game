/**
 * @file TransformGizmo3D.cpp
 * @brief 3D transform gizmo implementation
 */
#include "TransformGizmo3D.h"
#include "editor/viewport/EditorCamera3D.h"
#include "engine/core/ActorObjectExtended.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#include <ImGuizmo.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace editor {

bool TransformGizmo3D::render(EditorCamera3D* camera, engine::ActorObjectExtended* actor,
                               const glm::vec2& viewportPos, const glm::vec2& viewportSize) {
    if (!camera || !actor) return false;
    
    // Set ImGuizmo rect to match viewport
    ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);
    ImGuizmo::SetOrthographic(false);
    ImGuizmo::SetDrawlist();
    
    // Get camera matrices
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();
    
    // Build model matrix from actor transform
    auto pos2D = actor->getPosition();
    float posZ = actor->getZ();
    glm::vec3 pos(pos2D.x / 100.0f, posZ / 100.0f, pos2D.y / 100.0f);
    
    auto scale2D = actor->getScale();
    glm::vec3 scale(scale2D.x, 1.0f, scale2D.y);
    
    float rotation = actor->getRotation();
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, scale);
    
    // Convert operation enum
    ImGuizmo::OPERATION op;
    switch (m_operation) {
        case GizmoOperation::Rotate:
            op = ImGuizmo::ROTATE;
            break;
        case GizmoOperation::Scale:
            op = ImGuizmo::SCALE;
            break;
        case GizmoOperation::Translate:
        default:
            op = ImGuizmo::TRANSLATE;
            break;
    }
    
    // Convert space enum
    ImGuizmo::MODE mode = (m_space == GizmoSpace::Local) ? ImGuizmo::LOCAL : ImGuizmo::WORLD;
    
    // Get snap values
    float* snapPtr = nullptr;
    float snapValues[3] = {0.0f, 0.0f, 0.0f};
    if (m_snapEnabled) {
        switch (m_operation) {
            case GizmoOperation::Translate:
                snapValues[0] = snapValues[1] = snapValues[2] = m_translateSnap;
                break;
            case GizmoOperation::Rotate:
                snapValues[0] = snapValues[1] = snapValues[2] = m_rotateSnap;
                break;
            case GizmoOperation::Scale:
                snapValues[0] = snapValues[1] = snapValues[2] = m_scaleSnap;
                break;
        }
        snapPtr = snapValues;
    }
    
    // Manipulate
    bool manipulated = ImGuizmo::Manipulate(
        glm::value_ptr(view),
        glm::value_ptr(projection),
        op,
        mode,
        glm::value_ptr(model),
        nullptr,
        snapPtr
    );
    
    m_isUsing = ImGuizmo::IsUsing();
    
    // Apply transform back to actor if manipulated
    if (manipulated) {
        glm::vec3 newScale, newTranslation, skew;
        glm::quat newRotation;
        glm::vec4 perspective;
        glm::decompose(model, newScale, newRotation, newTranslation, skew, perspective);
        
        // Convert back to 2D coordinates
        float newX = newTranslation.x * 100.0f;
        float newY = newTranslation.z * 100.0f;  // Z in 3D = Y in 2D
        float newZ = newTranslation.y * 100.0f;  // Y in 3D = Z (elevation)
        
        actor->setPosition(newX, newY, newZ);
        actor->setScale(newScale.x, newScale.z);
        
        // Extract Y rotation
        glm::vec3 euler = glm::eulerAngles(newRotation);
        actor->setRotation(glm::degrees(euler.y));
    }
    
    return manipulated;
}

void TransformGizmo3D::renderUI() {
    ImGui::Text("Gizmo");
    ImGui::SameLine();
    
    // Operation buttons
    bool isTranslate = (m_operation == GizmoOperation::Translate);
    bool isRotate = (m_operation == GizmoOperation::Rotate);
    bool isScale = (m_operation == GizmoOperation::Scale);
    
    if (isTranslate) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
    if (ImGui::Button("T##translate")) m_operation = GizmoOperation::Translate;
    if (isTranslate) ImGui::PopStyleColor();
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Translate (W)");
    
    ImGui::SameLine();
    if (isRotate) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
    if (ImGui::Button("R##rotate")) m_operation = GizmoOperation::Rotate;
    if (isRotate) ImGui::PopStyleColor();
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Rotate (E)");
    
    ImGui::SameLine();
    if (isScale) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
    if (ImGui::Button("S##scale")) m_operation = GizmoOperation::Scale;
    if (isScale) ImGui::PopStyleColor();
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Scale (R)");
    
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    
    // Space toggle
    const char* spaceLabel = (m_space == GizmoSpace::Local) ? "Local" : "World";
    if (ImGui::Button(spaceLabel)) {
        m_space = (m_space == GizmoSpace::Local) ? GizmoSpace::World : GizmoSpace::Local;
    }
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Toggle Local/World space");
    
    ImGui::SameLine();
    ImGui::Checkbox("Snap", &m_snapEnabled);
}

} // namespace editor

#endif // HAS_IMGUI
