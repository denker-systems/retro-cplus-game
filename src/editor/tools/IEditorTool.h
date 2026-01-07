/**
 * @file IEditorTool.h
 * @brief Interface for editor tools (Select, Move, Scale, etc.)
 */
#pragma once

#include <string>

struct SDL_Renderer;
struct ImDrawList;
struct ImVec2;

namespace engine {
    class Scene;
    class ActorObjectExtended;
}

/**
 * @brief Tool context passed to all tools
 */
struct ToolContext {
    engine::Scene* scene = nullptr;
    engine::ActorObjectExtended* selectedActor = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    float zoom = 1.0f;
    float panX = 0.0f;
    float panY = 0.0f;
    
    bool snapToGrid = true;
    int gridSize = 32;
    
    // Viewport info
    float viewportX = 0.0f;
    float viewportY = 0.0f;
    float viewportW = 640.0f;
    float viewportH = 400.0f;
};

/**
 * @brief Base interface for all editor tools
 */
class IEditorTool {
public:
    virtual ~IEditorTool() = default;
    
    // Tool identification
    virtual const char* getName() const = 0;
    virtual const char* getIcon() const = 0;
    virtual const char* getTooltip() const = 0;
    
    // Lifecycle
    virtual void activate() {}
    virtual void deactivate() {}
    
    // Input handling (return true if handled)
    virtual bool onMouseDown(float roomX, float roomY, ToolContext& ctx) { return false; }
    virtual bool onMouseUp(float roomX, float roomY, ToolContext& ctx) { return false; }
    virtual bool onMouseDrag(float roomX, float roomY, float deltaX, float deltaY, ToolContext& ctx) { return false; }
    virtual bool onMouseMove(float roomX, float roomY, ToolContext& ctx) { return false; }
    virtual bool onKeyDown(int key, ToolContext& ctx) { return false; }
    
    // Rendering
    virtual void renderOverlay(ImDrawList* drawList, const ToolContext& ctx) {}
    virtual void renderUI() {}
    
    // State
    virtual bool isActive() const { return m_active; }
    virtual void setActive(bool active) { m_active = active; }
    
protected:
    bool m_active = false;
};
