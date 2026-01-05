/**
 * @file InventorySystem.h
 * @brief Inventory system för items
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>

/**
 * @brief Ett item i inventoryt
 */
struct Item {
    std::string id;
    std::string name;
    std::string description;
    bool combinable = false;
    std::string combinesWith;   // ID på item att kombinera med
    std::string combineResult;  // ID på resultat-item
};

/**
 * @brief Hanterar spelarens inventory
 */
class InventorySystem {
public:
    static InventorySystem& instance();
    
    /** @brief Registrera ett item (definition) */
    void registerItem(const Item& item);
    
    /** @brief Lägg till item i inventory */
    bool addItem(const std::string& itemId);
    
    /** @brief Ta bort item från inventory */
    bool removeItem(const std::string& itemId);
    
    /** @brief Kolla om item finns i inventory */
    bool hasItem(const std::string& itemId) const;
    
    /** @brief Försök kombinera två items */
    bool combineItems(const std::string& item1, const std::string& item2);
    
    /** @brief Hämta alla items i inventory */
    std::vector<Item> getItems() const;
    
    /** @brief Rensa inventoryt */
    void clear() { m_inventory.clear(); }
    
    /** @brief Hämta item-definition */
    const Item* getItemDef(const std::string& itemId) const;
    
    /** @brief Antal items */
    size_t getItemCount() const { return m_inventory.size(); }
    
    /** @brief Max antal items */
    static constexpr int MAX_ITEMS = 8;

private:
    InventorySystem() = default;
    
    std::unordered_map<std::string, Item> m_itemDefs;  // Item-definitioner
    std::vector<std::string> m_inventory;              // Spelarens items
};
