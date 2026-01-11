/**
 * @file RuntimeInventory.h
 * @brief Inventory system för runtime
 */
#pragma once

#include <string>
#include <vector>
#include <functional>

struct Item {
    std::string id;
    std::string name;
    std::string description;
    std::string icon;
    bool combinable = false;
    std::string combinesWith;
    std::string combineResult;
};

/**
 * @brief Player inventory management
 */
class RuntimeInventory {
public:
    RuntimeInventory();
    ~RuntimeInventory() = default;
    
    void loadItemDefinitions();
    
    bool addItem(const std::string& itemId);
    bool removeItem(const std::string& itemId);
    bool hasItem(const std::string& itemId) const;
    
    bool combineItems(const std::string& item1, const std::string& item2);
    bool useItem(const std::string& itemId, const std::string& targetId);
    
    const std::vector<Item>& getInventory() const { return m_inventory; }
    const Item* getItemDefinition(const std::string& id) const;
    
    void setOnItemAdded(std::function<void(const Item&)> cb) { m_onAdd = cb; }
    void setOnItemRemoved(std::function<void(const Item&)> cb) { m_onRemove = cb; }

private:
    std::vector<Item> m_itemDefinitions;
    std::vector<Item> m_inventory;
    
    std::function<void(const Item&)> m_onAdd;
    std::function<void(const Item&)> m_onRemove;
};
