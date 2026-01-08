/**
 * @file ItemTools.cpp
 * @brief Implementation of AI tools for item manipulation
 */
#include "ItemTools.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"

namespace ai {

ToolResult ListItemsTool::execute(const nlohmann::json& params) {
    const auto& items = DataLoader::instance().getItems();
    
    nlohmann::json itemList = nlohmann::json::array();
    for (const auto& item : items) {
        itemList.push_back({
            {"id", item.id},
            {"name", item.name},
            {"description", item.description},
            {"icon", item.icon},
            {"combinable", item.combinable}
        });
    }
    
    nlohmann::json result = {
        {"item_count", items.size()},
        {"items", itemList}
    };
    
    return ToolResult::ok("Found " + std::to_string(items.size()) + " items", result);
}

ToolResult GetItemTool::execute(const nlohmann::json& params) {
    std::string itemId = params.value("item_id", "");
    
    if (itemId.empty()) {
        return ToolResult::error("item_id is required");
    }
    
    const ItemData* item = DataLoader::getItemById(itemId);
    if (!item) {
        return ToolResult::error("Item not found: " + itemId);
    }
    
    nlohmann::json result = {
        {"id", item->id},
        {"name", item->name},
        {"description", item->description},
        {"icon", item->icon},
        {"combinable", item->combinable},
        {"combines_with", item->combinesWith},
        {"combine_result", item->combineResult}
    };
    
    return ToolResult::ok("Item '" + item->name + "'", result);
}

ToolResult CreateItemTool::execute(const nlohmann::json& params) {
    std::string id = params.value("id", "");
    std::string name = params.value("name", "");
    std::string description = params.value("description", "");
    
    if (id.empty() || name.empty() || description.empty()) {
        return ToolResult::error("id, name, and description are required");
    }
    
    // Check for duplicate ID
    if (DataLoader::getItemById(id) != nullptr) {
        return ToolResult::error("Item with ID '" + id + "' already exists");
    }
    
    // Create new item
    ItemData item;
    item.id = id;
    item.name = name;
    item.description = description;
    item.icon = params.value("icon", "items/default.png");
    item.combinable = params.value("combinable", false);
    item.combinesWith = params.value("combines_with", "");
    item.combineResult = params.value("combine_result", "");
    
    // Add to DataLoader
    if (!DataLoader::addItem(item)) {
        return ToolResult::error("Failed to add item");
    }
    
    // Save to file
    if (!DataLoader::saveItems()) {
        return ToolResult::error("Failed to save item data");
    }
    
    LOG_INFO("[AI] Created item '" + name + "' with ID '" + id + "'");
    
    nlohmann::json result = {
        {"id", item.id},
        {"name", item.name},
        {"description", item.description}
    };
    
    return ToolResult::ok("Created item '" + name + "'", result);
}

ToolResult ModifyItemTool::execute(const nlohmann::json& params) {
    std::string itemId = params.value("item_id", "");
    
    if (itemId.empty()) {
        return ToolResult::error("item_id is required");
    }
    
    ItemData* item = DataLoader::getItemByIdMutable(itemId);
    if (!item) {
        return ToolResult::error("Item not found: " + itemId);
    }
    
    // Apply modifications
    if (params.contains("name")) item->name = params["name"].get<std::string>();
    if (params.contains("description")) item->description = params["description"].get<std::string>();
    if (params.contains("icon")) item->icon = params["icon"].get<std::string>();
    if (params.contains("combinable")) item->combinable = params["combinable"].get<bool>();
    if (params.contains("combines_with")) item->combinesWith = params["combines_with"].get<std::string>();
    if (params.contains("combine_result")) item->combineResult = params["combine_result"].get<std::string>();
    
    // Save to file
    if (!DataLoader::saveItems()) {
        return ToolResult::error("Failed to save item data");
    }
    
    LOG_INFO("[AI] Modified item '" + itemId + "'");
    
    nlohmann::json result = {
        {"item_id", itemId},
        {"modified", true}
    };
    
    return ToolResult::ok("Modified item '" + itemId + "'", result);
}

} // namespace ai
