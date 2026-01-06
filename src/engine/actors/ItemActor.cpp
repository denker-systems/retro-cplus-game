/**
 * @file ItemActor.cpp
 * @brief Item Actor Implementation
 */
#include "ItemActor.h"

namespace engine {

// ============================================================================
// ITEM ACTOR
// ============================================================================

ItemActor::ItemActor(const std::string& name, const std::string& itemId)
    : ActorObject(name)
    , m_itemId(itemId)
    , m_displayName(name)
    , m_pickable(true)
{
}

// ============================================================================
// INVENTORY ITEM
// ============================================================================

InventoryItem::InventoryItem(const std::string& itemId)
    : ItemActor(itemId, itemId)
    , m_stackSize(1)
{
}

// ============================================================================
// WEAPON
// ============================================================================

Weapon::Weapon(const std::string& itemId)
    : InventoryItem(itemId)
    , m_damage(10)
{
}

void Weapon::use() {
    // Attack logic here
}

// ============================================================================
// CONSUMABLE
// ============================================================================

Consumable::Consumable(const std::string& itemId)
    : InventoryItem(itemId)
    , m_healAmount(20)
{
}

void Consumable::use() {
    // Consume logic here
    // This would typically heal the player
}

} // namespace engine
