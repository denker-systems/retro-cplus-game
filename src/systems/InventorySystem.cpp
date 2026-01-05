/**
 * @file InventorySystem.cpp
 * @brief Implementation av inventory system
 */
#include "InventorySystem.h"
#include <iostream>
#include <algorithm>

InventorySystem& InventorySystem::instance() {
    static InventorySystem instance;
    return instance;
}

void InventorySystem::registerItem(const Item& item) {
    m_itemDefs[item.id] = item;
    std::cout << "Registered item: " << item.id << " (" << item.name << ")" << std::endl;
}

bool InventorySystem::addItem(const std::string& itemId) {
    // Kolla om item finns registrerat
    if (m_itemDefs.find(itemId) == m_itemDefs.end()) {
        std::cerr << "Unknown item: " << itemId << std::endl;
        return false;
    }
    
    // Kolla om inventory är fullt
    if (m_inventory.size() >= MAX_ITEMS) {
        std::cout << "Inventory full!" << std::endl;
        return false;
    }
    
    // Kolla om redan har item
    if (hasItem(itemId)) {
        std::cout << "Already have: " << itemId << std::endl;
        return false;
    }
    
    m_inventory.push_back(itemId);
    std::cout << "Picked up: " << m_itemDefs[itemId].name << std::endl;
    return true;
}

bool InventorySystem::removeItem(const std::string& itemId) {
    auto it = std::find(m_inventory.begin(), m_inventory.end(), itemId);
    if (it != m_inventory.end()) {
        m_inventory.erase(it);
        std::cout << "Removed: " << itemId << std::endl;
        return true;
    }
    return false;
}

bool InventorySystem::hasItem(const std::string& itemId) const {
    return std::find(m_inventory.begin(), m_inventory.end(), itemId) != m_inventory.end();
}

bool InventorySystem::combineItems(const std::string& item1, const std::string& item2) {
    const Item* def1 = getItemDef(item1);
    const Item* def2 = getItemDef(item2);
    
    if (!def1 || !def2) return false;
    
    // Kolla om item1 kan kombineras med item2
    if (def1->combinable && def1->combinesWith == item2) {
        removeItem(item1);
        removeItem(item2);
        addItem(def1->combineResult);
        std::cout << "Combined " << def1->name << " + " << def2->name << std::endl;
        return true;
    }
    
    // Kolla omvänt
    if (def2->combinable && def2->combinesWith == item1) {
        removeItem(item1);
        removeItem(item2);
        addItem(def2->combineResult);
        std::cout << "Combined " << def2->name << " + " << def1->name << std::endl;
        return true;
    }
    
    std::cout << "Cannot combine these items" << std::endl;
    return false;
}

const Item* InventorySystem::getItemDef(const std::string& itemId) const {
    auto it = m_itemDefs.find(itemId);
    return (it != m_itemDefs.end()) ? &it->second : nullptr;
}

std::vector<Item> InventorySystem::getItems() const {
    std::vector<Item> items;
    for (const auto& itemId : m_inventory) {
        const Item* def = getItemDef(itemId);
        if (def) {
            items.push_back(*def);
        }
    }
    return items;
}
