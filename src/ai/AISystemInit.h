/**
 * @file AISystemInit.h
 * @brief Initialization and registration of AI system components
 */
#pragma once

namespace ai {

/**
 * @brief Initialize the AI system and register all tools
 * @return true if successful
 */
bool initializeAISystem();

/**
 * @brief Shutdown the AI system
 */
void shutdownAISystem();

/**
 * @brief Register all built-in editor tools
 */
void registerBuiltInTools();

} // namespace ai
