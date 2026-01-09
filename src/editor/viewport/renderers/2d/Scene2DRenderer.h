/**
 * @file Scene2DRenderer.h
 * @brief 2D renderer for Scene view - inherits from Level2DRenderer
 */
#pragma once

#include "Level2DRenderer.h"
#include "../../ui/ViewportActorRenderer.h"
#include <memory>

namespace engine {
    class Scene;
    class ActorObjectExtended;
}

namespace editor {
namespace viewport {

class ViewportActorRenderer;

/**
 * @class Scene2DRenderer
 * @brief Renders scene in 2D (actors) - Inherits Level+World functionality
 * 
 * Full inheritance chain: World2DRenderer → Level2DRenderer → Scene2DRenderer
 */
class Scene2DRenderer : public Level2DRenderer {
public:
    Scene2DRenderer();
    ~Scene2DRenderer() override;
    
    // Override inherited setters to expose them publicly
    void setWorld(engine::World* world) { World2DRenderer::setWorld(world); }
    void setLevel(engine::Level* level) { Level2DRenderer::setLevel(level); }
    void setScene(engine::Scene* scene) { m_scene = scene; }
    
    engine::ActorObjectExtended* getSelectedActor() const { return m_selectedActor; }
    void deleteSelectedActor();
    void duplicateSelectedActor();

protected:
    void render2DContent(ImDrawList* drawList, const RenderContext& ctx) override;
    void renderSceneContent(ImDrawList* drawList, const RenderContext& ctx);
    void handle2DInput(const RenderContext& ctx, bool hovered) override;
    
    engine::Scene* m_scene = nullptr;
    std::unique_ptr<ViewportActorRenderer> m_actorRenderer;
    
    engine::ActorObjectExtended* m_selectedActor = nullptr;
    engine::ActorObjectExtended* m_draggedActor = nullptr;
    float m_actorDragOffsetX = 0.0f;
    float m_actorDragOffsetY = 0.0f;
    
    int m_gridSize = 32;
    bool m_snapToGrid = true;
};

} // namespace viewport
} // namespace editor
