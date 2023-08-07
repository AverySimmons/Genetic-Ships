#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
    float value;
    int weight_num;
    float * weights;
    float bias;
} Node;

typedef struct {
    int input_num;
    int layer_num;
    int * layer_array;
    int total_nodes;
    int total_connections;
    Node *** network;
} NeuralNetwork;

NeuralNetwork * createNetwork(int input_num, int layer_num, int * layer_array) {
    NeuralNetwork * new_network = (NeuralNetwork *) malloc(sizeof(NeuralNetwork));
    new_network->input_num = input_num;
    new_network->layer_num = layer_num;
    new_network->layer_array = (int *) malloc(sizeof(int) * layer_num);
    new_network->total_nodes = 0;
    for (int i = 0; i < layer_num; i++) {
        new_network->layer_array[i] = layer_array[i];
        new_network->total_nodes += layer_array[i];
    }
    new_network->network = (Node ***) malloc(sizeof(Node **) * (layer_num));
    new_network->total_connections = 0;
    for (int i = 0; i < layer_num; i++) {
        new_network->network[i] = (Node **) malloc(sizeof(Node *) * layer_array[i]);
        for (int j = 0; j < layer_array[i]; j++) {
            new_network->network[i][j] = (Node *) malloc(sizeof(Node));
            new_network->network[i][j]->weight_num = (i == 0) ? input_num : layer_array[i - 1];
            new_network->total_connections += new_network->network[i][j]->weight_num;
            new_network->network[i][j]->weights = (float *) malloc(sizeof(float) * new_network->network[i][j]->weight_num);
            for (int k = 0; k < new_network->network[i][j]->weight_num; k++) {
                new_network->network[i][j]->weights[k] = ((float)rand() / RAND_MAX) * 4 - 2;
            }
            new_network->network[i][j]->bias = 0.0;
        }
    }
    return new_network;
}

void freeNetwork(NeuralNetwork * nn) {
    for (int i = 0; i < nn->layer_num; i++) {
        for (int j = 0; j < nn->layer_array[i]; j++) {
            free(nn->network[i][j]->weights);
            free(nn->network[i][j]);
        }
        free(nn->network[i]);
    }
    free(nn->network);
    free(nn->layer_array);
    free(nn);
}

float sigmoid(float input) {
    return 1 / (1 + exp(-input));
}

float * calculateNetwork(NeuralNetwork * nn, int input_num, float * input) {
    if (input_num != nn->input_num) {
        perror("Neural network input does not match input nodes!!");
    }
    for (int i = 0; i < nn->layer_num; i++) {
        for (int j = 0; j < nn->layer_array[i]; j++) {
            float new_value = 0;
            if (i == 0) {
                for (int k = 0; k < input_num; k++) {
                    new_value += input[k] * nn->network[i][j]->weights[k] + nn->network[i][j]->bias;
                }
            } else {
                for (int k = 0; k < nn->layer_array[i-1]; k++) {
                    new_value += nn->network[i-1][k]->value * nn->network[i][j]->weights[k] + nn->network[i][j]->bias;
                }
            }
            new_value = sigmoid(new_value);
            nn->network[i][j]->value = new_value;
        }
    }
    float * output = (float *) malloc(sizeof(float) * nn->layer_array[nn->layer_num-1]);
    for (int i = 0; i < nn->layer_array[nn->layer_num-1]; i++) {
        output[i] = nn->network[nn->layer_num-1][i]->value;
    }
    return output;
}

void printNetwork(NeuralNetwork * nn) {
    printf("Neural Network Structure:\n");
    printf("Input Layer: %d nodes\n", nn->input_num);

    for (int i = 0; i < nn->layer_num; i++) {
        printf("Layer %d: %d nodes\n", i + 1, nn->layer_array[i]);
        for (int j = 0; j < nn->layer_array[i]; j++) {
            printf("Node %d - Value: %.6f, Bias: %.6f, Weights: ", j + 1, nn->network[i][j]->value, nn->network[i][j]->bias);
            for (int k = 0; k < nn->network[i][j]->weight_num; k++) {
                printf("%.6f ", nn->network[i][j]->weights[k]);
            }
            printf("\n");
        }
    }
}

// only works for identical networks
void crossoverNetworks(NeuralNetwork * parent1, NeuralNetwork * parent2, NeuralNetwork * offspring1, NeuralNetwork * offspring2) {
    int weight_crossover_point = rand() % parent1->total_connections;
    int weight_count = 0;
    for (int i = 0; i < parent1->layer_num; i++) {
        for (int j = 0; j < parent1->layer_array[i]; j++) {
            for (int k = 0; k < parent1->network[i][j]->weight_num; k++) {
                if (weight_count < weight_crossover_point) {
                    offspring1->network[i][j]->weights[k] = parent1->network[i][j]->weights[k];
                    offspring2->network[i][j]->weights[k] = parent2->network[i][j]->weights[k];
                } else {
                    offspring1->network[i][j]->weights[k] = parent2->network[i][j]->weights[k];
                    offspring2->network[i][j]->weights[k] = parent1->network[i][j]->weights[k];
                }
                weight_count++;
            }
        }
    }
    int bias_crossover_point = rand() % parent1->total_nodes;
    int bias_count = 0;
    for (int i = 0; i < parent1->layer_num; i++) {
        for (int j = 0; j < parent1->layer_array[i]; j++) {
            if (bias_count < bias_crossover_point) {
                offspring1->network[i][j]->bias = parent1->network[i][j]->bias;
                offspring2->network[i][j]->bias = parent2->network[i][j]->bias;
            } else {
                offspring1->network[i][j]->bias = parent2->network[i][j]->bias;
                offspring2->network[i][j]->bias = parent1->network[i][j]->bias;
            }
            bias_count++;
        }
    }
}

void mutateNetwork(NeuralNetwork * nn) {
    for (int i = 0; i < nn->layer_num; i++) {
        for (int j = 0; j < nn->layer_array[i]; j++) {
            for (int k = 0; k < nn->network[i][j]->weight_num; k++) {
                if (rand() / (float)RAND_MAX < 0.01) {
                    nn->network[i][j]->weights[k] += (float)rand() / RAND_MAX * 0.2 - 0.1;
                }
            }
            if (rand() / (float)RAND_MAX < 0.01) {
                nn->network[i][j]->bias += (float)rand() / RAND_MAX * 0.2 - 0.1;
            }
        }
    }
}

void writeNetwork(NeuralNetwork * nn) {
    
}