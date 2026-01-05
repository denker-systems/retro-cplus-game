#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;

    if (!game.init("Retro Adventure", Game::SCREEN_WIDTH, Game::SCREEN_HEIGHT)) {
        return 1;
    }

    game.run();
    return 0;
}
