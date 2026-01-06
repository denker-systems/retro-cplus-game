/**
 * @file CoreRedirects.h
 * @brief Core Redirects - Backward Compatibility Aliases
 * 
 * This file provides type aliases for migrating from the legacy Node-based
 * hierarchy to the new UE5-style Object/ActorObject hierarchy.
 * 
 * The aliases allow old code to continue working while new code can use
 * the improved naming conventions.
 * 
 * Migration Strategy: Strangler Fig Pattern
 * - Old code uses Node/Node2D → works via aliases
 * - New code uses Object/ActorObject → clearer naming
 * - Gradual migration over 6-8 weeks
 */
#pragma once

#include "Node.h"
#include "Node2D.h"

namespace engine {

// ============================================================================
// CORE REDIRECTS (UE5-style naming)
// ============================================================================

/**
 * @brief Object is the universal base for everything (like UObject in UE5)
 * 
 * Currently aliased to Node for backward compatibility.
 * Future: Will become standalone base class with serialization, reflection.
 */
using Object = Node;

/**
 * @brief ActorObject is the base for objects in the world (like AActor in UE5)
 * 
 * Currently aliased to Node2D for backward compatibility.
 * Future: Will add component support and ownership semantics.
 */
using ActorObject = Node2D;

// ============================================================================
// LEGACY TYPEDEFS (For explicit legacy code marking)
// ============================================================================

/**
 * @brief Legacy Node typedef - marks code for migration
 * @deprecated Use Object instead
 */
using LegacyNode = Node;

/**
 * @brief Legacy Node2D typedef - marks code for migration
 * @deprecated Use ActorObject instead
 */
using LegacyNode2D = Node2D;

} // namespace engine

/**
 * @note Migration Timeline:
 * 
 * Sprint 1-2: Add aliases, both systems work
 * Sprint 3-5: Gradually migrate code to new names
 * Sprint 6+:  Deprecate old names, eventually remove
 * 
 * All changes are backward compatible - old code continues to work!
 */
