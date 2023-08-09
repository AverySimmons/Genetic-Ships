#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include "game.h"

int main() {
    srand(time(NULL));
    Game * game = createGame(100, (Vector2){500, 500}, 1, 5, 0.4, 20, 7, 2 * 3.1415 / 3, 3000, 150);
    writeGameHeader(game);
    for (int epoch = 0; epoch < game->max_epochs; epoch++) {
        game->crashes = 0;
        for (int tick = 0; tick < game->max_ticks; tick++) {
            gameTick(game);
            if (epoch == game->max_epochs - 1) {
                writePositions(game);
            }
        }
        printf("epoch %i: ", epoch);
        gameEnd(game);
    }
    freeGame(game);
}