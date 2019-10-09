#include <stdlib.h>
#include "neuron.h"

/** Generates a random float between min and max */
float randomFloat(float min, float max) {
    //Don't forget to initialise the rand() function somewhere in the program:
    //srand((unsigned int)time(NULL));
    return (max - min) * (float)rand()/(float)(RAND_MAX) + min;
}

/** Creates a new neuron with random weights and random biais */
Neuron* randomNeuron(int nbWeights) {

    //Allocates memory for the weights, the biais and the number of weights
    //(All the fields in the Neuron struct)
    Neuron* neuron = malloc(sizeof(float) + sizeof(float*) + sizeof(int));

    //Generates random weights and random biais
    float* weights = malloc(sizeof(float)*nbWeights);
    for(int i = 0; i < nbWeights; i++)
        weights[i] = randomFloat(-1, 2) / (float) nbWeights;

    (*neuron).biais = randomFloat(-1, 1);
    (*neuron).weights = weights;
    (*neuron).nbWeights = nbWeights;

    return neuron;
}

/** Creates a new neuron with given biais and given weights
 * <br> Warning, the weights are just copied, don't forget to free them */
Neuron* buildNeuron(float biais, int nbWeights, float* weights) {

    //Allocates memory for the weights, the biais and the number of weights
    //(All the fields in the Neuron struct)
    Neuron* neuron = malloc(sizeof(float) + sizeof(float)*nbWeights + sizeof(int));

    for(int i = 0; i < nbWeights; i++)
        (*neuron).weights[i] = weights[i];

    (*neuron).biais = biais;
    (*neuron).nbWeights = nbWeights;

    return neuron;
}

/** Calculates the answer of the neuron for a specific inputs array
 * <br> The length of the inputs array must be equal to the number of weights of the neuron */
float simulate(Neuron* n, float* inputs) {
    float sum = (*n).biais; //Adds the biais to the weighted sum
    //Sums all the inputs multiplied by the weights
    for(int i = 0; i < (*n).nbWeights; i++)
        sum += (*n).weights[i] * inputs[i];
    return activation(sum);
}

/** The activation function: x < 0 => 0; x > 1 => 1; else: x */
float activation(float x) {
    if(x > 1) return 1;
    if(x < 0) return 0;
    return x;
}

void destroyNeuron(Neuron* neuron) {
    free((*neuron).weights);
    free(neuron);
}