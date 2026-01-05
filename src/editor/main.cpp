/**
 * @file main.cpp
 * @brief Entry point för RetroAdventure Editor (Standalone)
 */
#include "EditorApp.h"
#include "../utils/Logger.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    LOG_INFO("=== RetroAdventure Editor Starting ===");
    
    EditorApp editor;
    if (editor.init("Retro Adventure Editor", 800, 600)) {
        editor.run();
    }
    
    LOG_INFO("=== RetroAdventure Editor Shutdown ===");
    return 0;
}
