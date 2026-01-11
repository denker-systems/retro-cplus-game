/**
 * @file ActorDetailsPanel.h
 * @brief Detaljerad actor editor i separat tab (liknande Unreal Blueprint Editor)
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <string>
#include <memory>

class EditorContext;
class SelectionManager;

namespace engine {
    class ActorObjectExtended;
    class CharacterController3DComponent;
    class SpriteComponent;
    class RigidBody3DComponent;
    class CameraComponent;
}

/**
 * @brief Detaljerad editor för actors i separat tab
 * 
 * Öppnas när användaren dubbelklickar på en actor i Hierarchy.
 * Visar transform, components, och actor-specifika properties.
 * Liknande Unreal's Details panel eller Unity's Inspector.
 */
class ActorDetailsPanel : public IEditorPanel {
public:
    explicit ActorDetailsPanel(EditorContext& context);
    ~ActorDetailsPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
    // Actor selection
    void setActor(engine::ActorObjectExtended* actor);
    engine::ActorObjectExtended* getActor() const { return m_actor; }
    
    // Selection manager integration
    void setSelectionManager(SelectionManager* selMgr) { m_selectionManager = selMgr; }
    
private:
    // Rendering sections
    void renderHeader();
    void renderTransform();
    void renderComponents();
    void renderAddComponentButton();
    void renderActorProperties();
    
    // Component-specific editors
    void renderCharacterController3D(engine::CharacterController3DComponent* comp);
    void renderSpriteComponent(engine::SpriteComponent* comp);
    void renderRigidBody3D(engine::RigidBody3DComponent* comp);
    void renderCameraComponent(engine::CameraComponent* comp);
    void renderInteractionVolume();  // For NPC3DActor
    
    // Component management
    void addComponent(const std::string& componentType);
    void removeComponent(const std::string& componentType);
    
    EditorContext& m_context;
    SelectionManager* m_selectionManager = nullptr;
    engine::ActorObjectExtended* m_actor = nullptr;
    
    std::string m_id = "actor_details";
    std::string m_title = "Actor Details";
    
    // UI state
    bool m_transformExpanded = true;
    bool m_componentsExpanded = true;
    bool m_propertiesExpanded = true;
    int m_selectedComponentType = 0;
};
