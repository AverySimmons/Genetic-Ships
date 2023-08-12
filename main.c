#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "game.h"

int main() {
    srand(time(NULL));
    int write_rate = 25;
    Game * game = createGame(100, (Vector2){500, 500}, 1, 2, 0.4, 20, 4, 2 * 3.1415 / 3, 3000, 150);
    writeGameHeader(game, write_rate);
    for (int epoch = 0; epoch < game->max_epochs; epoch++) {
        game->crashes = 0;
        if (epoch % write_rate == 0 || epoch == game->max_epochs - 1) {
            writeEpochHeader(game);
        }
        for (int tick = 0; tick < game->max_ticks; tick++) {
            gameTick(game);
            if (epoch % write_rate == 0 || epoch == game->max_epochs - 1) {
                writePositions(game);
            }
        }
        printf("epoch %i: ", epoch);
        gameEnd(game);
    }
    freeGame(game);
}