/**
 * @file CoreRedirects.h
 * @brief Type aliases for migration from Node to Actor system
 * 
 * ⚠️ DEPRECATED: This file is for backward compatibility only.
 * 
 * Migration Status (2026-01-05):
 * - Object: Real class (src/engine/core/Object.h) - USE THIS
 * - ActorObject: Real class (src/engine/core/ActorObject.h) - USE THIS
 * - ActorObjectExtended: Real class with components - USE THIS
 * - Node: DEPRECATED - Use ActorObject instead
 * - Node2D: DEPRECATED - Use ActorObject instead
 * 
 * New code should NOT use Node or Node2D.
 * Legacy nodes (Sprite, Label, etc.) still inherit from Node2D temporarily.
 * STATUS: Object and ActorObject are now REAL classes (not aliases)
 */
#pragma once

#include "Object.h"
#include "ActorObject.h"
// Node.h and Node2D.h REMOVED from project - use ActorObject instead

namespace engine {

// ============================================================================
// NODE SYSTEM REMOVED
// ============================================================================
// Node and Node2D have been completely removed from the project.
// All code must use ActorObject instead.

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
