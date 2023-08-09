#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include "brain.h"

typedef struct {
    float x;
    float y;
} Vector2;

typedef struct {
    Vector2 position;
    Vector2 direction;
    NeuralNetwork * brain;
    float fitness;
    float current_speed;
} Entity;

typedef struct {
    Vector2 size;
    int entity_num;
    Entity ** entities;
    float entity_speed;
    float entity_size;
    float turn_speed;
    float ray_length;
    int ray_number;
    float ray_angle;
    int max_ticks;
    int max_epochs;
    int crashes;
} Game;

typedef struct ListNode {
    Entity * entity;
    struct ListNode * next;
} ListNode;

typedef struct {
    int cell_size;
    int grid_width;
    int grid_height;
    ListNode *** grid;
} Grid;

float lengthVector2(Vector2 vect) {
    return sqrt(pow(vect.x, 2) + pow(vect.y, 2));
}

Vector2 addVector2(Vector2 vect1, Vector2 vect2) {
    vect1.x += vect2.x;
    vect1.y += vect2.y;
    return vect1;
}

Vector2 subVector2(Vector2 vect1, Vector2 vect2) {
    vect1.x -= vect2.x;
    vect1.y -= vect2.y;
    return vect1;
}

Vector2 flatMultiplyVector2(Vector2 vect, float flat) {
    vect.x *= flat;
    vect.y *= flat;
    return vect;
}

Vector2 normalizeVector2(Vector2 vect) {
    float length = lengthVector2(vect);
    if (length == 0) {
        return (Vector2){0, 0};
    }
    vect.x /= length;
    vect.y /= length;
    return vect;
}

Vector2 rotateVector2(Vector2 vect, float theta) {
    Vector2 new_vect;
    new_vect.x = vect.x * cos(theta) - vect.y * sin(theta);
    new_vect.y = vect.y * cos(theta) + vect.x * sin(theta);
    return new_vect;
}

bool dotLineCollision(Vector2 dot, Vector2 line_start, Vector2 line_end) {
    const float buffer = 0.02;
    float line_length = lengthVector2(subVector2(line_end, line_start));
    float start_length = lengthVector2(subVector2(dot, line_start));
    float end_length = lengthVector2(subVector2(dot, line_end));
    return start_length + end_length <= line_length + buffer;
}

float lineLineCollision(Vector2 line_start1, Vector2 line_start2, Vector2 line_end1, Vector2 line_end2) {
    float uA = ((line_end2.x - line_start2.x) * (line_start1.y - line_start2.y) - (line_end2.y - line_start2.y) * 
    (line_start1.x - line_start2.x)) / ((line_end2.y - line_start2.y) * (line_end1.x - line_start1.x) - 
     (line_end2.x - line_start2.x) * (line_end1.y - line_start1.y));
    float uB = ((line_end1.x - line_start1.x) * (line_start1.y - line_start2.y) - (line_end1.y - line_start1.y) * 
     (line_start1.x - line_start2.x)) / ((line_end2.y - line_start2.y) * (line_end1.x - line_start1.x) - 
     (line_end2.x - line_start2.x) * (line_end1.y - line_start1.y));
    if (uA >= 0 && uA <= 1 && uB >= 0 && uB <= 1) {
        Vector2 intersect;
        intersect.x = line_start1.x + (uA * (line_end1.x - line_start1.x));
        intersect.y = line_start1.y + (uA * (line_end1.y - line_start1.y));
        return lengthVector2(subVector2(line_start1, intersect));
    }
    return lengthVector2(subVector2(line_start1, line_end1));
}

float lineCircleCollision(Vector2 circ, float radius, Vector2 line_start, Vector2 line_end) {
    if (lengthVector2(subVector2(circ, line_start)) <= radius) {
        return 0;
    }
    bool on_endpoint = lengthVector2(subVector2(circ, line_end)) <= radius;
    float line_length = lengthVector2(subVector2(line_end, line_start));
    float dot =  (((circ.x - line_start.x) * (line_end.x - line_start.x)) + ((circ.y - line_start.y) * (line_end.y - line_start.y))) / pow(line_length, 2);
    float closest_x = line_start.x + dot * (line_end.x - line_start.x);
    float closest_y = line_start.y + dot * (line_end.y - line_start.y);
    Vector2 closest_point = {closest_x, closest_y};
    if (!on_endpoint && !dotLineCollision((Vector2){closest_x, closest_y}, line_start, line_end)) {
        return line_length;
    }
    float distance_to_collision = sqrt(pow(radius, 2) - pow(lengthVector2(subVector2(circ, closest_point)), 2));
    return lengthVector2(subVector2(closest_point, line_start)) - distance_to_collision;
}

void spawnEntity(Entity * ent, Game * game) {
    ent->position = (Vector2){rand() % (int)game->size.x, rand() % (int)game->size.y};
    ent->direction = normalizeVector2((Vector2){rand(), rand()});
}

Game * createGame(int entity_num, Vector2 size, float entity_speed, float entity_size,
 float turn_speed, float ray_length, int ray_number, float ray_angle, int max_ticks, int max_epochs) {
    Game * new_game = (Game *) malloc(sizeof(Game));
    new_game->size = size;
    new_game->entity_speed = entity_speed;
    new_game->entity_size = entity_size;
    new_game->turn_speed = turn_speed;
    new_game->entity_num = entity_num;
    new_game->ray_length = ray_length;
    new_game->ray_number = ray_number;
    new_game->ray_angle = ray_angle;
    new_game->max_ticks = max_ticks;
    new_game->max_epochs = max_epochs;
    new_game->entities = (Entity **) malloc(sizeof(Entity *) * entity_num);
    for (int i = 0; i < entity_num; i++) {
        new_game->entities[i] = (Entity *) malloc(sizeof(Entity));
        new_game->entities[i]->brain = createNetwork(new_game->ray_number, 2, (int[]){4, 2});
        new_game->entities[i]->fitness = 0;
        spawnEntity(new_game->entities[i], new_game);
    }
    return new_game;
}

void freeGame(Game * game) {
    for (int i = 0; i < game->entity_num; i++) {
        freeNetwork(game->entities[i]->brain);
        free(game->entities[i]);
    }
    free(game->entities);
    free(game);
}

Grid * createGrid(int cell_size, int grid_width, int grid_height) {
    Grid * new_grid = (Grid *) malloc(sizeof(Grid));
    new_grid->cell_size = cell_size;
    new_grid->grid_width = grid_width;
    new_grid->grid_height = grid_height;
    new_grid->grid = (ListNode ***) malloc(sizeof(ListNode **) * grid_width);
    for (int i = 0; i < grid_width; i++) {
        new_grid->grid[i] = (ListNode **) malloc(sizeof(ListNode *) * grid_height);
        for (int j = 0; j < grid_height; j++) {
            new_grid->grid[i][j] = NULL;
        }
    }
    return new_grid;
}

void freeGrid(Grid * grid) {
    for (int i = 0; i < grid->grid_width; i++) {
        for (int j = 0; j < grid->grid_height; j++) {
            ListNode * current_node = grid->grid[i][j];
            while (current_node != NULL) {
                ListNode * temp = current_node;
                current_node = current_node->next;
                free(temp);
            }
        }
        free(grid->grid[i]);
    }
    free(grid);
}

void checkEntityRays(Game * game, Entity * ent, Entity * target, float * current_rays, Grid * grid) {
    Vector2 current_direction = rotateVector2(ent->direction, -game->ray_angle / 2.);
    float angle_increment = game->ray_angle / (game->ray_number - 1);
    int cur_cell_x = (int)floor(ent->position.x / grid->cell_size);
    int cur_cell_y = (int)floor(ent->position.y / grid->cell_size);
    for (int i = 0; i < game->ray_number; i++) {
        float closest = game->ray_length;
        Vector2 ray_start = ent->position;
        Vector2 ray_end = addVector2(ent->position, flatMultiplyVector2(current_direction, game->ray_length));
        float collide_ray_length = lineCircleCollision(target->position, game->entity_size, ray_start, ray_end);
        closest = collide_ray_length < closest ? collide_ray_length : closest;
        current_rays[i] = closest / game->ray_length < current_rays[i] ? closest / game->ray_length : current_rays[i];
        current_direction = rotateVector2(current_direction, angle_increment);
    }
}

void checkWallRays(Game * game, Entity * ent, float * current_rays) {
    Vector2 current_direction = rotateVector2(ent->direction, -game->ray_angle / 2.);
    float angle_increment = game->ray_angle / (game->ray_number - 1);
    Vector2 x_wall_start = {ent->position.x < game->size.x / 2. ? 0 : game->size.x, 0};
    Vector2 x_wall_end = {ent->position.x < game->size.x / 2. ? 0 : game->size.x, game->size.y};
    Vector2 y_wall_start = {0, ent->position.y < game->size.y / 2. ? 0 : game->size.y};
    Vector2 y_wall_end = {game->size.x, ent->position.y < game->size.y / 2. ? 0 : game->size.y};
    for (int i = 0; i < game->ray_number; i++) {
        Vector2 ray_start = ent->position;
        Vector2 ray_end = addVector2(ent->position, flatMultiplyVector2(current_direction, game->ray_length));
        float x_wall_distance = lineLineCollision(ray_start, x_wall_start, ray_end, x_wall_end);
        float y_wall_distance = lineLineCollision(ray_start, y_wall_start, ray_end, y_wall_end);
        x_wall_distance = x_wall_distance < y_wall_distance ? x_wall_distance : y_wall_distance;
        current_rays[i] = x_wall_distance / game->ray_length < current_rays[i] ? x_wall_distance / game->ray_length : current_rays[i];
    }
}

void gameTick(Game * game) {
    float cell_size = game->entity_size + game->ray_length;
    int grid_width = ceil(game->size.x / cell_size);
    int grid_height = ceil(game->size.y / cell_size);
    Grid * grid = createGrid(cell_size, grid_width, grid_height);
    for (int i = 0; i < game->entity_num; i++) {
        int cell_x = (int)floor(game->entities[i]->position.x / cell_size);
        int cell_y = (int)floor(game->entities[i]->position.y / cell_size);
        ListNode * new_node = (ListNode *) malloc(sizeof(ListNode));
        new_node->entity = game->entities[i];
        new_node->next = NULL;
        if (grid->grid[cell_x][cell_y] == NULL) {
            grid->grid[cell_x][cell_y] = new_node;
        } else {
            ListNode * current_node = grid->grid[cell_x][cell_y];
            while (current_node->next != NULL) {
                current_node = current_node->next;
            }
            current_node->next = new_node;
        }
    }
    for (int r = 0; r < game->entity_num; r++) {
        int cur_cell_x = (int)floor(game->entities[r]->position.x / cell_size);
        int cur_cell_y = (int)floor(game->entities[r]->position.y / cell_size);

        float * ray_length_array = (float *) malloc(sizeof(float) * game->ray_number);
        for (int i = 0; i < game->ray_number; i++) {
            ray_length_array[i] = 1.;
        }

        for (int j = -1; j <= 1; j++) {
            if (cur_cell_x + j > grid_width - 1 || cur_cell_x + j < 0) {continue;}
            for (int k = -1; k <= 1; k++) {
                if (cur_cell_y + k > grid_height - 1 || cur_cell_y + k < 0) {continue;}
                ListNode * current_node = grid->grid[cur_cell_x + j][cur_cell_y + k];
                while (current_node != NULL) {
                    if (current_node->entity == game->entities[r]) {
                        current_node = current_node->next;
                        continue;
                    }
                    // check against node
                    checkEntityRays(game, game->entities[r], current_node->entity, ray_length_array, grid);

                    current_node = current_node->next;
                }
            }
        }
        checkWallRays(game, game->entities[r], ray_length_array);
        for (int cur = 0; cur < game->ray_number; cur++) {
            if (ray_length_array[cur] == 0) {
                game->entities[r]->fitness += 20;
            }
        }
        float * brain_output = calculateNetwork(game->entities[r]->brain, game->ray_number, ray_length_array);
        free(ray_length_array);
        game->entities[r]->direction = rotateVector2(game->entities[r]->direction, (brain_output[0] - 0.5) * game->turn_speed);
        game->entities[r]->current_speed = brain_output[1];
        free(brain_output);
    }
    freeGrid(grid);
    for (int i = 0; i < game->entity_num; i++) {
        game->entities[i]->position = addVector2(game->entities[i]->position, flatMultiplyVector2(game->entities[i]->direction, game->entity_speed * game->entities[i]->current_speed));
        if (game->entities[i]->position.x < 0 || game->entities[i]->position.x >= game->size.x || 
         game->entities[i]->position.y < 0 || game->entities[i]->position.y >= game->size.y) {
            spawnEntity(game->entities[i], game);
            game->entities[i]->fitness += 0;
            game->crashes += 1;
        }
    }
}

int compareFitness(const void* a, const void* b) {
    const Entity * structA = *(const Entity **)a;
    const Entity * structB = *(const Entity **)b;
    if (structA->fitness - structB->fitness > 0) {
        return 1;
    } else if (structA->fitness - structB->fitness < 0) {
        return -1;
    } else {
        return 0;
    }
    return (structA->fitness - structB->fitness);
}

void writePositions(Game * game) {
    FILE * fp = fopen("data.bin", "ab");
    for (int i = 0; i < game->entity_num; i++) {
        fwrite(&game->entities[i]->position.x, sizeof(float), 1, fp);
        fwrite(&game->entities[i]->position.y, sizeof(float), 1, fp);
    }
    fclose(fp);
}

void gameEnd(Game * game) {
    int crossover_safe_number = ceil(game->entity_num / 2.);
    int mutate_safe_number = ceil(game->entity_num / 20.);
    qsort(game->entities, game->entity_num, sizeof(Entity *), compareFitness);
    float average_fitness = 0;
    for (int i = 0; i < game->entity_num; i++) {
        average_fitness += game->entities[i]->fitness;
    }
    average_fitness /= game->entity_num;
    printf("crashes: %i - best_fitness: %f - average_fitness: %f\n", game->crashes, game->entities[game->entity_num-1]->fitness, average_fitness);
    for (int i = 0; i < game->entity_num; i++) {
        if (i < game->entity_num - crossover_safe_number && i % 2 == 0) {
            int parent_index1 = game->entity_num - 1 - rand() % crossover_safe_number;
            int parent_index2 = game->entity_num - 1 - rand() % crossover_safe_number;
            crossoverNetworks(game->entities[parent_index1]->brain, game->entities[parent_index2]->brain, game->entities[i]->brain, game->entities[i+1]->brain);
        }
        if (i < game->entity_num - mutate_safe_number) {
            mutateNetwork(game->entities[i]->brain);
        }
        game->entities[i]->fitness = 0;
        spawnEntity(game->entities[i], game);
    }
}

void writeGameHeader(Game * game) {
    FILE * fp = fopen("data.bin", "wb");
    fwrite(&game->size.x, sizeof(float), 1, fp);
    fwrite(&game->size.y, sizeof(float), 1, fp);
    fwrite(&game->entity_num, sizeof(int), 1, fp);
    fwrite(&game->entity_size, sizeof(float), 1, fp);
    fwrite(&game->max_ticks, sizeof(int), 1, fp);
    fclose(fp);
}