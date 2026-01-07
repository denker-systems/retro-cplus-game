/**
 * @file InventoryComponent.h
 * @brief Inventory Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace engine {

/**
 * @brief Component for inventory management
 */
class InventoryComponent : public ActorComponent {
public:
    struct InventorySlot {
        std::string itemId;
        int quantity = 0;
    };
    
    InventoryComponent(const std::string& name = "InventoryComponent");
    virtual ~InventoryComponent() = default;
    
    void setMaxSlots(int slots) { m_maxSlots = slots; }
    int getMaxSlots() const { return m_maxSlots; }
    
    bool addItem(const std::string& itemId, int quantity = 1);
    bool removeItem(const std::string& itemId, int quantity = 1);
    
    int getItemCount(const std::string& itemId) const;
    bool hasItem(const std::string& itemId) const;
    
    const std::vector<InventorySlot>& getItems() const { return m_items; }
    
    bool isFull() const { return m_items.size() >= m_maxSlots; }
    
private:
    std::vector<InventorySlot> m_items;
    int m_maxSlots = 20;
};

} // namespace engine
