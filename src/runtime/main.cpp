/**
 * @file main.cpp
 * @brief Entry point för RetroGame Runtime
 */
#include "RuntimeApp.h"
#include "engine/utils/Logger.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize logger
    Logger::instance().init("assets/logs/runtime.log");
    
    LOG_INFO("========================================");
    LOG_INFO("=== RETRO GAME RUNTIME STARTING ===");
    LOG_INFO("========================================");
    
    RuntimeApp app;
    
    if (!app.init()) {
        LOG_ERROR("Failed to initialize RuntimeApp");
        return -1;
    }
    
    app.run();
    app.shutdown();
    
    LOG_INFO("========================================");
    LOG_INFO("=== RETRO GAME RUNTIME SHUTDOWN ===");
    LOG_INFO("========================================");
    
    return 0;
}
