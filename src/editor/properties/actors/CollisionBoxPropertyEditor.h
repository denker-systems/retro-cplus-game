/**
 * @file CollisionBoxPropertyEditor.h
 * @brief Property editor for collision boxes
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include "editor/core/EditorContext.h"
#include <memory>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace engine {
    class ActorObjectExtended;
}

/**
 * @brief Property editor for collision boxes
 */
class CollisionBoxPropertyEditor : public IPropertyEditor {
public:
    explicit CollisionBoxPropertyEditor(EditorContext& context);
    ~CollisionBoxPropertyEditor() = default;
    
    void setCollisionBox(engine::ActorObjectExtended* actor, int boxIndex);
    void render() override;
    bool validate(std::string& outError) const override;
    bool isDirty() const override;
    void revert() override;
    void apply() override;
    
private:
    void renderBasicProperties();
    void renderShapeProperties();
    
    EditorContext& m_context;
    engine::ActorObjectExtended* m_actor = nullptr;
    int m_boxIndex = -1;
    bool m_isDirty = false;
};
