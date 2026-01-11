/**
 * @file ActorDetailsPanel.cpp
 * @brief Implementation of Actor Details Panel
 */
#include "ActorDetailsPanel.h"
#include "editor/core/EditorContext.h"
#include "editor/core/SelectionManager.h"
#include "engine/actors/Character3DActor.h"
#include "engine/actors/PlayerConfigActor.h"
#include "engine/actors/NPC3DActor.h"
#include "engine/components/CharacterController3DComponent.h"
#include "engine/components/SpriteComponent.h"
#include "engine/components/RigidBody3DComponent.h"
#include "engine/components/CameraComponent.h"
#include "engine/utils/Logger.h"
#include <iostream>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

ActorDetailsPanel::ActorDetailsPanel(EditorContext& context)
    : m_context(context) {
}

void ActorDetailsPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    // Position on right side on first use
    ImGuiIO& io = ImGui::GetIO();
    ImGui::SetNextWindowSize(ImVec2(400, io.DisplaySize.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x - 400, 0), ImGuiCond_FirstUseEver);
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_None;
    if (ImGui::Begin(m_title.c_str(), &m_visible, flags)) {
        if (!m_actor) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No actor selected");
            ImGui::Text("Select an actor in Hierarchy to edit");
        } else {
            renderHeader();
            ImGui::Separator();
            
            renderTransform();
            ImGui::Separator();
            
            renderComponents();
            ImGui::Separator();
            
            renderActorProperties();
        }
    }
    ImGui::End();
#endif
}

void ActorDetailsPanel::renderHeader() {
#ifdef HAS_IMGUI
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.8f, 0.2f, 1.0f));
    ImGui::Text("Actor: %s", m_actor->getName().c_str());
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    if (ImGui::SmallButton("Close")) {
        m_visible = false;
    }
#endif
}

void ActorDetailsPanel::renderTransform() {
#ifdef HAS_IMGUI
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Position
        auto pos = m_actor->getPosition();
        float posZ = m_actor->getZ();
        float position[3] = { pos.x, pos.y, posZ };
        
        if (ImGui::DragFloat3("Position", position, 1.0f)) {
            m_actor->setPosition(position[0], position[1], position[2]);
            m_context.markDirty();
        }
        
        // Rotation
        float rotation = m_actor->getRotation();
        if (ImGui::DragFloat("Rotation", &rotation, 1.0f, -360.0f, 360.0f, "%.1f°")) {
            m_actor->setRotation(rotation);
            m_context.markDirty();
        }
        
        // 3D Character yaw (if applicable)
        if (auto* character = dynamic_cast<engine::Character3DActor*>(m_actor)) {
            float yaw = character->getYaw();
            if (ImGui::DragFloat("Yaw (3D)", &yaw, 1.0f, -360.0f, 360.0f, "%.1f°")) {
                character->setYaw(yaw);
                m_context.markDirty();
            }
        }
        
        // Scale
        auto scale = m_actor->getScale();
        float scaleArray[2] = { scale.x, scale.y };
        if (ImGui::DragFloat2("Scale", scaleArray, 0.1f, 0.1f, 10.0f)) {
            m_actor->setScale(scaleArray[0], scaleArray[1]);
            m_context.markDirty();
        }
        
        // PlayerConfigActor camera settings
        if (auto* playerConfig = dynamic_cast<engine::PlayerConfigActor*>(m_actor)) {
            ImGui::Separator();
            ImGui::TextColored(ImVec4(0.2f, 0.9f, 0.9f, 1.0f), "Camera Offset (Play Mode)");
            
            glm::vec3 offset = playerConfig->getCameraOffset();
            float offsetArray[3] = { offset.x, offset.y, offset.z };
            if (ImGui::DragFloat3("Offset", offsetArray, 0.1f, -20.0f, 20.0f)) {
                glm::vec3 newOffset(offsetArray[0], offsetArray[1], offsetArray[2]);
                playerConfig->setCameraOffset(newOffset);
                std::cout << "[ActorDetails] Camera offset changed to (" 
                          << newOffset.x << "," << newOffset.y << "," << newOffset.z << ")" << std::endl;
                m_context.markDirty();
            }
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "X=right, Y=up, Z=forward");
            
            float followSpeed = playerConfig->getCameraFollowSpeed();
            if (ImGui::DragFloat("Follow Speed", &followSpeed, 0.1f, 0.1f, 20.0f)) {
                playerConfig->setCameraFollowSpeed(followSpeed);
                m_context.markDirty();
            }
        }
    }
#endif
}

void ActorDetailsPanel::renderComponents() {
#ifdef HAS_IMGUI
    if (ImGui::CollapsingHeader("Components", ImGuiTreeNodeFlags_DefaultOpen)) {
        // CharacterController3D
        if (auto* controller = m_actor->getComponent<engine::CharacterController3DComponent>()) {
            renderCharacterController3D(controller);
        }
        
        // SpriteComponent
        if (auto* sprite = m_actor->getComponent<engine::SpriteComponent>()) {
            renderSpriteComponent(sprite);
        }
        
        // RigidBody3D
        if (auto* rb = m_actor->getComponent<engine::RigidBody3DComponent>()) {
            renderRigidBody3D(rb);
        }
        
        // CameraComponent
        if (auto* camera = m_actor->getComponent<engine::CameraComponent>()) {
            renderCameraComponent(camera);
        }
        
        // InteractionVolume for NPC3DActor
        if (dynamic_cast<engine::NPC3DActor*>(m_actor)) {
            renderInteractionVolume();
        }
        
        ImGui::Spacing();
        renderAddComponentButton();
    }
#endif
}

void ActorDetailsPanel::renderCharacterController3D(engine::CharacterController3DComponent* comp) {
#ifdef HAS_IMGUI
    if (ImGui::TreeNode("CharacterController3D")) {
        // Move Speed
        float moveSpeed = comp->getMoveSpeed();
        if (ImGui::DragFloat("Move Speed", &moveSpeed, 0.1f, 0.0f, 20.0f)) {
            comp->setMoveSpeed(moveSpeed);
            m_context.markDirty();
        }
        
        // Jump Velocity
        float jumpVel = comp->getJumpVelocity();
        if (ImGui::DragFloat("Jump Velocity", &jumpVel, 0.1f, 0.0f, 30.0f)) {
            comp->setJumpVelocity(jumpVel);
            m_context.markDirty();
        }
        
        // Gravity
        float gravity = comp->getGravity();
        if (ImGui::DragFloat("Gravity", &gravity, 0.1f, 0.0f, 50.0f)) {
            comp->setGravity(gravity);
            m_context.markDirty();
        }
        
        // Status (read-only)
        ImGui::Separator();
        ImGui::Text("Status:");
        ImGui::Text("  Grounded: %s", comp->isGrounded() ? "YES" : "NO");
        ImGui::Text("  Initialized: %s", comp->isInitialized() ? "YES" : "NO");
        
        ImGui::TreePop();
    }
#endif
}

void ActorDetailsPanel::renderSpriteComponent(engine::SpriteComponent* comp) {
#ifdef HAS_IMGUI
    if (ImGui::TreeNode("SpriteComponent")) {
        // Texture path
        std::string path = comp->getTexturePath();
        char buffer[256];
        strncpy_s(buffer, path.c_str(), sizeof(buffer) - 1);
        if (ImGui::InputText("Texture", buffer, sizeof(buffer))) {
            comp->setTexturePath(buffer);
            m_context.markDirty();
        }
        
        // Size
        float sizeArray[2] = { static_cast<float>(comp->getWidth()), static_cast<float>(comp->getHeight()) };
        if (ImGui::DragFloat2("Size", sizeArray, 1.0f, 1.0f, 1000.0f, "%.0f")) {
            comp->setSize(static_cast<int>(sizeArray[0]), static_cast<int>(sizeArray[1]));
            m_context.markDirty();
        }
        
        ImGui::TreePop();
    }
#endif
}

void ActorDetailsPanel::renderRigidBody3D(engine::RigidBody3DComponent* comp) {
#ifdef HAS_IMGUI
    if (ImGui::TreeNode("RigidBody3D")) {
        ImGui::Text("Mass: %.2f", comp->getMass());
        ImGui::Text("Initialized: %s", comp->isInitialized() ? "YES" : "NO");
        
        ImGui::TreePop();
    }
#endif
}

void ActorDetailsPanel::renderAddComponentButton() {
#ifdef HAS_IMGUI
    if (ImGui::Button("+ Add Component", ImVec2(-1, 0))) {
        ImGui::OpenPopup("add_component_popup");
    }
    
    if (ImGui::BeginPopup("add_component_popup")) {
        ImGui::Text("Select Component Type:");
        ImGui::Separator();
        
        if (ImGui::Selectable("CharacterController3D")) {
            addComponent("CharacterController3D");
            ImGui::CloseCurrentPopup();
        }
        
        if (ImGui::Selectable("SpriteComponent")) {
            addComponent("SpriteComponent");
            ImGui::CloseCurrentPopup();
        }
        
        if (ImGui::Selectable("RigidBody3D")) {
            addComponent("RigidBody3D");
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
#endif
}

void ActorDetailsPanel::renderActorProperties() {
#ifdef HAS_IMGUI
    if (ImGui::CollapsingHeader("Actor Properties")) {
        // Name
        char nameBuffer[128];
        strncpy_s(nameBuffer, m_actor->getName().c_str(), sizeof(nameBuffer) - 1);
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            m_actor->setName(nameBuffer);
            m_context.markDirty();
        }
        
        // Active
        bool active = m_actor->isActive();
        if (ImGui::Checkbox("Active", &active)) {
            m_actor->setActive(active);
            m_context.markDirty();
        }
        
        // Visible
        bool visible = m_actor->isVisible();
        if (ImGui::Checkbox("Visible", &visible)) {
            m_actor->setVisible(visible);
            m_context.markDirty();
        }
        
        // Render Order
        int renderOrder = m_actor->getRenderOrder();
        if (ImGui::DragInt("Render Order", &renderOrder, 1, -100, 100)) {
            m_actor->setRenderOrder(renderOrder);
            m_context.markDirty();
        }
        
        // PlayerConfigActor-specific settings
        if (auto* playerConfig = dynamic_cast<engine::PlayerConfigActor*>(m_actor)) {
            ImGui::Separator();
            ImGui::Text("Camera Settings (Play Mode)");
            
            // Camera Offset
            glm::vec3 offset = playerConfig->getCameraOffset();
            float offsetArray[3] = { offset.x, offset.y, offset.z };
            if (ImGui::DragFloat3("Camera Offset", offsetArray, 0.1f, -20.0f, 20.0f)) {
                playerConfig->setCameraOffset(glm::vec3(offsetArray[0], offsetArray[1], offsetArray[2]));
                m_context.markDirty();
            }
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "X=right, Y=up, Z=forward");
            
            // Camera Follow Speed
            float followSpeed = playerConfig->getCameraFollowSpeed();
            if (ImGui::DragFloat("Follow Speed", &followSpeed, 0.1f, 0.1f, 20.0f)) {
                playerConfig->setCameraFollowSpeed(followSpeed);
                m_context.markDirty();
            }
        }
    }
#endif
}

void ActorDetailsPanel::setActor(engine::ActorObjectExtended* actor) {
    m_actor = actor;
    if (actor) {
        m_title = "Actor Details: " + actor->getName();
        m_visible = true;  // Auto-open when actor is set
    } else {
        m_title = "Actor Details";
    }
}

void ActorDetailsPanel::addComponent(const std::string& componentType) {
    if (!m_actor) return;
    
    if (componentType == "CharacterController3D") {
        m_actor->addComponent<engine::CharacterController3DComponent>();
        LOG_INFO("[ActorDetailsPanel] Added CharacterController3D to " + m_actor->getName());
    } else if (componentType == "SpriteComponent") {
        m_actor->addComponent<engine::SpriteComponent>();
        LOG_INFO("[ActorDetailsPanel] Added SpriteComponent to " + m_actor->getName());
    } else if (componentType == "RigidBody3D") {
        m_actor->addComponent<engine::RigidBody3DComponent>();
        LOG_INFO("[ActorDetailsPanel] Added RigidBody3D to " + m_actor->getName());
    }
    
    m_context.markDirty();
}

void ActorDetailsPanel::removeComponent(const std::string& componentType) {
    if (!m_actor) return;
    
    // TODO: Implement removeComponent in ActorObjectExtended
    LOG_WARNING("[ActorDetailsPanel] removeComponent not yet implemented");
}

void ActorDetailsPanel::renderInteractionVolume() {
#ifdef HAS_IMGUI
    if (auto* npc = dynamic_cast<engine::NPC3DActor*>(m_actor)) {
        if (ImGui::TreeNode("InteractionVolume")) {
            glm::vec3 volume = npc->getInteractionVolume();
            float volumeArray[3] = { volume.x, volume.y, volume.z };
            
            if (ImGui::DragFloat3("Size (X, Y, Z)", volumeArray, 0.1f, 0.1f, 20.0f, "%.1f")) {
                npc->setInteractionVolume(glm::vec3(volumeArray[0], volumeArray[1], volumeArray[2]));
                m_context.markDirty();
            }
            
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "3D bounding box for interaction");
            
            ImGui::TreePop();
        }
    }
#endif
}

void ActorDetailsPanel::renderCameraComponent(engine::CameraComponent* comp) {
#ifdef HAS_IMGUI
    if (ImGui::TreeNode("CameraComponent")) {
        // Viewport Size
        int viewport[2] = { comp->getViewportWidth(), comp->getViewportHeight() };
        if (ImGui::DragInt2("Viewport Size", viewport, 1.0f, 1, 4096)) {
            comp->setViewportSize(viewport[0], viewport[1]);
            m_context.markDirty();
        }
        
        // Zoom
        float zoom = comp->getZoom();
        if (ImGui::DragFloat("Zoom", &zoom, 0.01f, 0.1f, 10.0f, "%.2f")) {
            comp->setZoom(zoom);
            m_context.markDirty();
        }
        
        // Follow Smoothing
        float smoothing = comp->getFollowSmoothing();
        if (ImGui::DragFloat("Follow Smoothing", &smoothing, 0.01f, 0.0f, 1.0f, "%.2f")) {
            comp->setFollowSmoothing(smoothing);
            m_context.markDirty();
        }
        
        // Follow Target (read-only for now)
        ImGui::Separator();
        ImGui::Text("Follow Target: %s", 
                    comp->getFollowTarget() ? comp->getFollowTarget()->getName().c_str() : "None");
        
        ImGui::TreePop();
    }
#endif
}
