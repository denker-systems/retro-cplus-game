/**
 * @file MigrationConfig.h
 * @brief Feature Flags for Migration Control
 * 
 * Allows toggling migration features ON/OFF for safe rollback.
 * Can disable new systems if problems occur.
 */
#pragma once

namespace engine {

/**
 * @brief Migration configuration and feature flags
 * 
 * Controls which parts of the migration are active.
 * All flags default to false (legacy system) for safety.
 * 
 * Usage:
 * @code
 * // Enable all migration features
 * MigrationConfig::enableMigration(true);
 * 
 * // Enable only component system
 * MigrationConfig::useComponentSystem = true;
 * @endcode
 */
struct MigrationConfig {
    /**
     * @brief Use new component system
     * 
     * When true: ActorObjectExtended uses components
     * When false: Legacy Node2D behavior
     */
    static bool useComponentSystem;
    
    /**
     * @brief Use new Pawn/Controller system
     * 
     * When true: Characters are Pawns with Controllers
     * When false: Legacy Entity/Character behavior
     */
    static bool usePawnSystem;
    
    /**
     * @brief Use new Controller system
     * 
     * When true: Input handled by Controllers
     * When false: Legacy direct input
     */
    static bool useControllerSystem;
    
    /**
     * @brief Enable or disable all migration features
     * @param enable true to enable, false to disable
     */
    static void enableMigration(bool enable) {
        useComponentSystem = enable;
        usePawnSystem = enable;
        useControllerSystem = enable;
    }
    
    /**
     * @brief Check if any migration features are enabled
     * @return true if any feature is enabled
     */
    static bool isAnyMigrationEnabled() {
        return useComponentSystem || usePawnSystem || useControllerSystem;
    }
};

} // namespace engine
