/**
 * @file main.cpp
 * @brief Retro Engine Launcher entry point
 */
#include "LauncherApp.h"
#include <iostream>

int main(int argc, char* argv[]) {
    launcher::LauncherApp app;
    
    if (!app.initialize()) {
        std::cerr << "Failed to initialize Retro Engine Launcher" << std::endl;
        return 1;
    }
    
    app.run();
    app.shutdown();
    
    return 0;
}
