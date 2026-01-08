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
#include "tools/HotspotTools.h"
#include "tools/DialogTools.h"
#include "tools/ItemTools.h"
#include "tools/CommandTools.h"
#include "tools/ContextTools.h"
#include "tools/LevelTools.h"
#include "tools/QuestTools.h"

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
    
    // Hotspot tools
    LOG_DEBUG("[AI] Registering Hotspot tools");
    registry.registerTool<ListHotspotsTool>();
    registry.registerTool<CreateHotspotTool>();
    registry.registerTool<ModifyHotspotTool>();
    registry.registerTool<DeleteHotspotTool>();
    
    // Dialog tools
    LOG_DEBUG("[AI] Registering Dialog tools");
    registry.registerTool<ListDialogsTool>();
    registry.registerTool<GetDialogTool>();
    registry.registerTool<CreateDialogTool>();
    registry.registerTool<AddDialogNodeTool>();
    
    // Item tools
    LOG_DEBUG("[AI] Registering Item tools");
    registry.registerTool<ListItemsTool>();
    registry.registerTool<GetItemTool>();
    registry.registerTool<CreateItemTool>();
    registry.registerTool<ModifyItemTool>();
    
    // Command tools
    LOG_DEBUG("[AI] Registering Command tools");
    registry.registerTool<ExecuteCommandTool>();
    registry.registerTool<ListCommandsTool>();
    
    // Context tools
    LOG_DEBUG("[AI] Registering Context tools");
    registry.registerTool<GetEditorContextTool>();
    registry.registerTool<SelectSceneTool>();
    registry.registerTool<SelectActorTool>();
    
    // Level/World tools
    LOG_DEBUG("[AI] Registering Level/World tools");
    registry.registerTool<ListLevelsTool>();
    registry.registerTool<CreateLevelTool>();
    registry.registerTool<AddSceneToLevelTool>();
    registry.registerTool<SetStartSceneTool>();
    registry.registerTool<GetWorldInfoTool>();
    
    // Quest tools
    LOG_DEBUG("[AI] Registering Quest tools");
    registry.registerTool<ListQuestsTool>();
    registry.registerTool<GetQuestTool>();
    registry.registerTool<CreateQuestTool>();
    registry.registerTool<AddQuestObjectiveTool>();
    registry.registerTool<LinkQuestToNPCTool>();
    
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
