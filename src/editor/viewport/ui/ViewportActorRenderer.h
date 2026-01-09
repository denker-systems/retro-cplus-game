/**
 * @file ViewportActorRenderer.h
 * @brief Renders actors in scene view
 */
#pragma once

#include "../ViewportTypes.h"
#include <vector>

// Forward declarations
struct ImDrawList;
namespace engine {
    class Scene;
    class ActorObjectExtended;
}

namespace editor {
namespace viewport {

/**
 * @class ViewportActorRenderer
 * @brief Handles rendering of actors with sprites, placeholders, and debug info
 */
class ViewportActorRenderer {
public:
    ViewportActorRenderer() = default;
    ~ViewportActorRenderer() = default;
    
    /**
     * @brief Render all actors in a scene
     * @param drawList ImGui draw list
     * @param scene Scene containing actors
     * @param ctx Render context
     * @param offsetX Screen X offset
     * @param offsetY Screen Y offset
     * @param selectedActor Currently selected actor (can be null)
     * @param draggedActor Currently dragged actor (can be null)
     */
    void renderActors(ImDrawList* drawList, engine::Scene* scene, 
                      const RenderContext& ctx, float offsetX, float offsetY,
                      engine::ActorObjectExtended* selectedActor,
                      engine::ActorObjectExtended* draggedActor);

private:
    void renderActorSprite(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                          float worldX, float worldY, float zoom, engine::Scene* scene);
    void renderActorPlaceholder(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                               float worldX, float worldY, float zoom, engine::Scene* scene);
    void renderActorSelection(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                             float worldX, float worldY, float zoom, bool isDragged);
    void renderActorLockIcon(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                            float worldX, float worldY);
    void renderActorPhysicsDebug(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                float worldX, float worldY, float zoom);
};

} // namespace viewport
} // namespace editor
