/**
 * @file MigrationConfig.cpp
 * @brief Migration Configuration Implementation
 */
#include "MigrationConfig.h"

namespace engine {

// Initialize all migration flags to false (safe default)
bool MigrationConfig::useComponentSystem = false;
bool MigrationConfig::usePawnSystem = false;
bool MigrationConfig::useControllerSystem = false;

} // namespace engine
