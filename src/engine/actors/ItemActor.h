/**
 * @file ItemActor.h
 * @brief Item actors for inventory and world items
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include <string>

namespace engine {

/**
 * @brief Base class for all item actors
 */
class ItemActor : public ActorObjectExtended {
public:
    ItemActor(const std::string& name, const std::string& itemId);
    virtual ~ItemActor() = default;
    
    const std::string& getItemId() const { return m_itemId; }
    const std::string& getDisplayName() const { return m_displayName; }
    const std::string& getDescription() const { return m_description; }
    
    void setDisplayName(const std::string& name) { m_displayName = name; }
    void setDescription(const std::string& desc) { m_description = desc; }
    
    bool isPickable() const { return m_pickable; }
    void setPickable(bool pickable) { m_pickable = pickable; }
    
    virtual void use() {}
    
protected:
    std::string m_itemId;
    std::string m_displayName;
    std::string m_description;
    bool m_pickable = true;
};

/**
 * @brief Inventory item
 */
class InventoryItem : public ItemActor {
public:
    InventoryItem(const std::string& itemId);
    virtual ~InventoryItem() = default;
    
    int getStackSize() const { return m_stackSize; }
    void setStackSize(int size) { m_stackSize = size; }
    
protected:
    int m_stackSize = 1;
};

/**
 * @brief Weapon item
 */
class Weapon : public InventoryItem {
public:
    Weapon(const std::string& itemId);
    virtual ~Weapon() = default;
    
    int getDamage() const { return m_damage; }
    void setDamage(int damage) { m_damage = damage; }
    
    void use() override;
    
protected:
    int m_damage = 10;
};

/**
 * @brief Consumable item
 */
class Consumable : public InventoryItem {
public:
    Consumable(const std::string& itemId);
    virtual ~Consumable() = default;
    
    int getHealAmount() const { return m_healAmount; }
    void setHealAmount(int amount) { m_healAmount = amount; }
    
    void use() override;
    
protected:
    int m_healAmount = 20;
};

} // namespace engine
