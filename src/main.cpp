/**
 * @file main.cpp
 * @brief Program entry point
 */
#include "Game.h"

int main(int argc, char* argv[]) {
    (void)argc;  // Undvik unused parameter warning
    (void)argv;
    
    Game game;

    // Initiera SDL och skapa fullscreen fönster
    if (!game.init("Retro Adventure", Game::GAME_WIDTH, Game::GAME_HEIGHT)) {
        return 1;  // Fel vid initiering
    }

    // Starta spelloop (blockerar tills spelet avslutas)
    game.run();
    return 0;
}
