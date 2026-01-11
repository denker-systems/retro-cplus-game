/**
 * @file RuntimeInventory.cpp
 * @brief Implementation av inventory system
 */
#include "RuntimeInventory.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"

RuntimeInventory::RuntimeInventory() = default;

void RuntimeInventory::loadItemDefinitions() {
    auto& dataLoader = DataLoader::instance();
    const auto& itemData = dataLoader.getItems();
    
    m_itemDefinitions.clear();
    
    for (const auto& i : itemData) {
        Item item;
        item.id = i.id;
        item.name = i.name;
        item.description = i.description;
        item.icon = i.icon;
        item.combinable = i.combinable;
        item.combinesWith = i.combinesWith;
        item.combineResult = i.combineResult;
        m_itemDefinitions.push_back(item);
    }
    
    LOG_INFO("[RuntimeInventory] Loaded " + std::to_string(m_itemDefinitions.size()) + " item definitions");
}

bool RuntimeInventory::addItem(const std::string& itemId) {
    const Item* def = getItemDefinition(itemId);
    if (!def) {
        LOG_WARNING("[RuntimeInventory] Unknown item: " + itemId);
        return false;
    }
    
    m_inventory.push_back(*def);
    LOG_INFO("[RuntimeInventory] Added item: " + def->name);
    
    if (m_onAdd) {
        m_onAdd(*def);
    }
    
    return true;
}

bool RuntimeInventory::removeItem(const std::string& itemId) {
    for (auto it = m_inventory.begin(); it != m_inventory.end(); ++it) {
        if (it->id == itemId) {
            Item removed = *it;
            m_inventory.erase(it);
            LOG_INFO("[RuntimeInventory] Removed item: " + removed.name);
            
            if (m_onRemove) {
                m_onRemove(removed);
            }
            return true;
        }
    }
    return false;
}

bool RuntimeInventory::hasItem(const std::string& itemId) const {
    for (const auto& item : m_inventory) {
        if (item.id == itemId) {
            return true;
        }
    }
    return false;
}

bool RuntimeInventory::combineItems(const std::string& item1, const std::string& item2) {
    const Item* def1 = getItemDefinition(item1);
    const Item* def2 = getItemDefinition(item2);
    
    if (!def1 || !def2) return false;
    if (!hasItem(item1) || !hasItem(item2)) return false;
    
    // Check if items can combine
    if (def1->combinable && def1->combinesWith == item2) {
        removeItem(item1);
        removeItem(item2);
        addItem(def1->combineResult);
        LOG_INFO("[RuntimeInventory] Combined " + def1->name + " + " + def2->name);
        return true;
    }
    
    if (def2->combinable && def2->combinesWith == item1) {
        removeItem(item1);
        removeItem(item2);
        addItem(def2->combineResult);
        LOG_INFO("[RuntimeInventory] Combined " + def2->name + " + " + def1->name);
        return true;
    }
    
    LOG_INFO("[RuntimeInventory] Cannot combine these items");
    return false;
}

bool RuntimeInventory::useItem(const std::string& itemId, const std::string& targetId) {
    if (!hasItem(itemId)) {
        LOG_WARNING("[RuntimeInventory] Don't have item: " + itemId);
        return false;
    }
    
    // TODO: Implement item usage logic based on target
    LOG_INFO("[RuntimeInventory] Used " + itemId + " on " + targetId);
    return true;
}

const Item* RuntimeInventory::getItemDefinition(const std::string& id) const {
    for (const auto& item : m_itemDefinitions) {
        if (item.id == id) {
            return &item;
        }
    }
    return nullptr;
}
