/**
 * @file ActorPropertyEditor.h
 * @brief Property editor for Actor objects
 */
#pragma once

#include "editor/properties/core/IPropertyEditor.h"
#include "engine/core/ActorObjectExtended.h"

class EditorContext;

/**
 * @brief Editor för Actor properties
 */
class ActorPropertyEditor : public IPropertyEditor {
public:
    explicit ActorPropertyEditor(EditorContext& context);
    
    void render() override;
    bool validate(std::string& outError) const override;
    bool isDirty() const override;
    void revert() override;
    void apply() override;
    
    // Actor-specifika metoder
    void setActor(engine::ActorObjectExtended* actor);
    engine::ActorObjectExtended* getActor() const { return m_actor; }
    
private:
    void renderBasicProperties();
    void renderTransformProperties();
    void renderComponentProperties();
    
    EditorContext& m_context;
    engine::ActorObjectExtended* m_actor = nullptr;
    bool m_isDirty = false;
};
