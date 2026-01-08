/**
 * @file ActorPropertyEditor.cpp
 * @brief Property editor for Actor objects implementation
 */
#include "ActorPropertyEditor.h"
#include "editor/core/EditorContext.h"
#include "editor/properties/core/PropertyEditorUtils.h"
#include "engine/components/SpriteComponent.h"
#include "engine/components/AnimationComponent.h"
#include "editor/properties/actors/LockableComponent.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

ActorPropertyEditor::ActorPropertyEditor(EditorContext& context)
    : m_context(context) {
}

void ActorPropertyEditor::render() {
#ifdef HAS_IMGUI
    if (!m_actor) {
        ImGui::Text("No actor selected");
        return;
    }
    
    renderBasicProperties();
    renderTransformProperties();
    renderComponentProperties();
#endif
}

void ActorPropertyEditor::renderBasicProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Basic Properties");
    
    // Lock state
    auto* lockable = m_actor->getComponent<engine::LockableComponent>();
    bool isLocked = lockable ? lockable->isLocked() : false;
    if (ImGui::Checkbox("Locked", &isLocked)) {
        if (!lockable) {
            lockable = m_actor->addComponent<engine::LockableComponent>();
        }
        lockable->setLocked(isLocked);
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Show lock icon if locked
    if (isLocked) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "🔒");
    }
    
    // Name (disabled if locked)
    std::string name = m_actor->getName();
    if (PropertyEditorUtils::InputTextDisabled("Name", name, isLocked)) {
        m_actor->setName(name);
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Active (disabled if locked)
    bool active = m_actor->isActive();
    if (ImGui::Checkbox("Active", &active)) {
        if (!isLocked) {
            m_actor->setActive(active);
            m_isDirty = true;
            m_context.markDirty();
        }
    }
    
    // Visible (disabled if locked)
    bool visible = m_actor->isVisible();
    if (ImGui::Checkbox("Visible", &visible)) {
        if (!isLocked) {
            m_actor->setVisible(visible);
            m_isDirty = true;
            m_context.markDirty();
        }
    }
#endif
}

void ActorPropertyEditor::renderTransformProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Transform");
    
    // Check if locked
    auto* lockable = m_actor->getComponent<engine::LockableComponent>();
    bool isLocked = lockable ? lockable->isLocked() : false;
    
    // Position (disabled if locked)
    engine::Vec2 pos = m_actor->getPosition();
    float position[2] = { pos.x, pos.y };
    if (ImGui::DragFloat2("Position", position, 1.0f, -1000.0f, 1000.0f, "%.0f")) {
        if (!isLocked) {
            m_actor->setPosition(position[0], position[1]);
            m_isDirty = true;
            m_context.markDirty();
        }
    }
    
    // Rotation (disabled if locked)
    float rotation = m_actor->getRotation();
    if (ImGui::DragFloat("Rotation", &rotation, 1.0f, -360.0f, 360.0f, "%.1f°")) {
        if (!isLocked) {
            m_actor->setRotation(rotation);
            m_isDirty = true;
            m_context.markDirty();
        }
    }
    
    // Scale (disabled if locked)
    engine::Vec2 scale = m_actor->getScale();
    float scaleArray[2] = { scale.x, scale.y };
    if (ImGui::DragFloat2("Scale", scaleArray, 0.1f, 0.1f, 10.0f, "%.2f")) {
        if (!isLocked) {
            m_actor->setScale(scaleArray[0], scaleArray[1]);
            m_isDirty = true;
            m_context.markDirty();
        }
    }
    
    // Z-index/Render Order
    int renderOrder = m_actor->getRenderOrder();
    if (ImGui::DragInt("Render Order", &renderOrder, 1, -100, 100)) {
        if (!isLocked) {
            m_actor->setRenderOrder(renderOrder);
            m_isDirty = true;
            m_context.markDirty();
        }
    }
#endif
}

void ActorPropertyEditor::renderComponentProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Components");
    
    // Check if locked
    auto* lockable = m_actor->getComponent<engine::LockableComponent>();
    bool isLocked = lockable ? lockable->isLocked() : false;
    
    // Sprite Component
    if (auto* sprite = m_actor->getComponent<engine::SpriteComponent>()) {
        if (ImGui::TreeNode("Sprite Component")) {
            // Texture path
            std::string path = sprite->getTexturePath();
            if (PropertyEditorUtils::InputTextDisabled("Texture", path, isLocked)) {
                sprite->setTexturePath(path);
                m_isDirty = true;
                m_context.markDirty();
            }
            
            // Size
            float size[2] = { static_cast<float>(sprite->getWidth()), 
                            static_cast<float>(sprite->getHeight()) };
            if (ImGui::DragFloat2("Size", size, 1.0f, 1.0f, 1000.0f, "%.0f")) {
                sprite->setSize(static_cast<int>(size[0]), static_cast<int>(size[1]));
                m_isDirty = true;
                m_context.markDirty();
            }
            
            ImGui::TreePop();
        }
    }
    
    // Animation Component
    if (auto* anim = m_actor->getComponent<engine::AnimationComponent>()) {
        if (ImGui::TreeNode("Animation Component")) {
            // Animation properties
            bool playing = anim->isPlaying();
            if (ImGui::Checkbox("Playing", &playing)) {
                if (playing) {
                    anim->play();
                } else {
                    anim->pause();
                }
            }
            
            float speed = anim->getSpeed();
            if (ImGui::DragFloat("Speed", &speed, 0.1f, 0.1f, 10.0f, "%.2f")) {
                anim->setSpeed(speed);
                m_isDirty = true;
                m_context.markDirty();
            }
            
            ImGui::TreePop();
        }
    }
#endif
}

bool ActorPropertyEditor::validate(std::string& outError) const {
    if (!m_actor) {
        outError = "No actor selected";
        return false;
    }
    
    if (m_actor->getName().empty()) {
        outError = "Actor name cannot be empty";
        return false;
    }
    
    return true;
}

bool ActorPropertyEditor::isDirty() const {
    return m_isDirty;
}

void ActorPropertyEditor::revert() {
    // TODO: Implement revert to original state
    m_isDirty = false;
}

void ActorPropertyEditor::apply() {
    // TODO: Apply changes to scene
    m_isDirty = false;
}

void ActorPropertyEditor::setActor(engine::ActorObjectExtended* actor) {
    m_actor = actor;
    m_isDirty = false;
}
