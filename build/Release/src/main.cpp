/**
 * @file main.cpp
 * @brief MyGame entry point
 */
#include "game/Game.h"
#include "game/states/MenuState.h"
#include "game/states/PlayState.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Game game;
    
    if (!game.init("MyGame", 1280, 720, false)) {
        std::cerr << "Failed to initialize game" << std::endl;
        return 1;
    }
    
    // Push initial state
    game.getStateManager().pushState(std::make_unique<MenuState>());
    
    game.run();
    game.shutdown();
    
    return 0;
}
