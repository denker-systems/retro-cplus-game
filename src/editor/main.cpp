/**
 * @file main.cpp
 * @brief Entry point för RetroAdventure Editor (Standalone)
 */
#include "game/Game.h"
#include "editor/EditorState.h"
#include "engine/utils/Logger.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    LOG_INFO("=== RetroAdventure Editor Starting ===");
    
    Game game;
    if (game.init("Retro Adventure Editor", 640, 400)) {
        // Ersätt MenuState med EditorState (inte push ovanpå)
        game.changeState(std::make_unique<EditorState>());
        game.run();
    }
    
    LOG_INFO("=== RetroAdventure Editor Shutdown ===");
    return 0;
}
