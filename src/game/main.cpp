/**
 * @file main.cpp
 * @brief Entry point för RetroAdventure Game
 */
#include "../Game.h"
#include "../utils/Logger.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    LOG_INFO("=== RetroAdventure Game Starting ===");
    
    Game game;
    if (game.init("Retro Adventure", 640, 400)) {
        game.run();
    }
    
    LOG_INFO("=== RetroAdventure Game Shutdown ===");
    return 0;
}
