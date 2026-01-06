/**
 * @file InventoryComponent.cpp
 * @brief Inventory Component Implementation
 */
#include "InventoryComponent.h"

namespace engine {

InventoryComponent::InventoryComponent(const std::string& name)
    : ActorComponent(name)
{
}

bool InventoryComponent::addItem(const std::string& itemId, int quantity) {
    // Check if item already exists
    for (auto& slot : m_items) {
        if (slot.itemId == itemId) {
            slot.quantity += quantity;
            return true;
        }
    }
    
    // Add new item
    if (isFull()) return false;
    
    InventorySlot slot;
    slot.itemId = itemId;
    slot.quantity = quantity;
    m_items.push_back(slot);
    return true;
}

bool InventoryComponent::removeItem(const std::string& itemId, int quantity) {
    for (auto it = m_items.begin(); it != m_items.end(); ++it) {
        if (it->itemId == itemId) {
            it->quantity -= quantity;
            if (it->quantity <= 0) {
                m_items.erase(it);
            }
            return true;
        }
    }
    return false;
}

int InventoryComponent::getItemCount(const std::string& itemId) const {
    for (const auto& slot : m_items) {
        if (slot.itemId == itemId) {
            return slot.quantity;
        }
    }
    return 0;
}

bool InventoryComponent::hasItem(const std::string& itemId) const {
    return getItemCount(itemId) > 0;
}

} // namespace engine
