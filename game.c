#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "brain.h"

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    Vector2 position;
    Vector2 direction;
    NeuralNetwork brain;
} Entity;

typedef struct {
    Vector2 size;
    int entity_num;
    Entity ** entities;
} Game;

Game * createGame(int entity_num, Vector2 size) {
    Game * new_game = (Game *) malloc(sizeof(Game));
    new_game->size = size;
    new_game->entity_num = entity_num;
    new_game->entities = (Entity **) malloc(sizeof(Entity *) * entity_num);
    for (int i = 0; i < entity_num; i++) {
        new_game->entities[i] = (Entity *) malloc(sizeof(Entity));
        new_game->entities[i]->position = (Vector2){rand() % (int)size.x, rand() % (int)size.y};
        new_game->entities[i]->direction = (Vector2){0, 0};
    }
    return new_game;
}

void gameTick(Game * game) {
    // sort into grid
    // for each entity get input and calculate new direction
    // move entities based on direction
}

int main() {
    
}