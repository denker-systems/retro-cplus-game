/**
 * @file AISystemInit.cpp
 * @brief Implementation of AI system initialization
 */
#include "AISystemInit.h"
#include "core/AIAgentSystem.h"
#include "core/EditorToolRegistry.h"
#include "ui/AIChatPanel.h"
#include "engine/utils/Logger.h"

// Tools
#include "tools/SceneTools.h"
#include "tools/ActorTools.h"

namespace ai {

void registerBuiltInTools() {
    LOG_INFO("[AI] Registering built-in tools...");
    auto& registry = EditorToolRegistry::instance();
    
    // Scene tools
    LOG_DEBUG("[AI] Registering Scene tools");
    registry.registerTool<ListScenesTool>();
    registry.registerTool<GetSceneTool>();
    registry.registerTool<CreateSceneTool>();
    registry.registerTool<ModifySceneTool>();
    
    // Actor tools
    LOG_DEBUG("[AI] Registering Actor tools");
    registry.registerTool<ListActorsTool>();
    registry.registerTool<GetActorTool>();
    registry.registerTool<CreateActorTool>();
    registry.registerTool<ModifyActorTool>();
    registry.registerTool<DeleteActorTool>();
    registry.registerTool<AddComponentTool>();
    
    LOG_INFO("[AI] Registered " + std::to_string(registry.getToolCount()) + " tools");
}

bool initializeAISystem() {
    LOG_INFO("[AI] ========== AI System Initialization ==========");
    
    // Register tools
    registerBuiltInTools();
    
    // Initialize agent system
    LOG_INFO("[AI] Initializing AIAgentSystem...");
    if (!AIAgentSystem::instance().initialize()) {
        LOG_ERROR("[AI] Failed to initialize AIAgentSystem!");
        return false;
    }
    LOG_INFO("[AI] AIAgentSystem initialized successfully");
    
    // Initialize UI
    LOG_INFO("[AI] Initializing AIChatPanel...");
    AIChatPanel::instance().initialize();
    LOG_INFO("[AI] AIChatPanel initialized");
    
    LOG_INFO("[AI] ========== AI System Ready ==========");
    return true;
}

void shutdownAISystem() {
    LOG_INFO("[AI] Shutting down AI system...");
    AIChatPanel::instance().shutdown();
    AIAgentSystem::instance().shutdown();
    EditorToolRegistry::instance().clear();
    LOG_INFO("[AI] AI system shutdown complete");
}

} // namespace ai
