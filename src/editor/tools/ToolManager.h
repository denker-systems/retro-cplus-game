/**
 * @file ToolManager.h
 * @brief Manages editor tools and tool switching
 */
#pragma once

#include "IEditorTool.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @brief Tool type enumeration
 */
enum class ToolType {
    Select,
    Scale,
    // Future tools
    // Rotate,
    // Pan,
    // Hotspot,
    Count
};

/**
 * @brief Manages all editor tools
 */
class ToolManager {
public:
    ToolManager();
    ~ToolManager() = default;
    
    // Tool management
    void registerTool(ToolType type, std::unique_ptr<IEditorTool> tool);
    void setActiveTool(ToolType type);
    IEditorTool* getActiveTool() const;
    ToolType getActiveToolType() const { return m_activeToolType; }
    
    // Get specific tool
    IEditorTool* getTool(ToolType type) const;
    
    // Input forwarding
    bool onMouseDown(float roomX, float roomY, ToolContext& ctx);
    bool onMouseUp(float roomX, float roomY, ToolContext& ctx);
    bool onMouseDrag(float roomX, float roomY, float deltaX, float deltaY, ToolContext& ctx);
    bool onMouseMove(float roomX, float roomY, ToolContext& ctx);
    bool onKeyDown(int key, ToolContext& ctx);
    
    // Rendering
    void renderOverlay(ImDrawList* drawList, const ToolContext& ctx);
    void renderToolbar();
    
    // Get all tools for iteration
    const std::vector<std::unique_ptr<IEditorTool>>& getTools() const { return m_tools; }
    
private:
    std::vector<std::unique_ptr<IEditorTool>> m_tools;
    ToolType m_activeToolType = ToolType::Select;
};
