#include <stdlib.h>
#include <math.h>
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
    Neuron* neuron = malloc(sizeof(float) + sizeof(float*) + sizeof(int) + sizeof(float)*2);

    //Generates random weights and random biais
    float* weights = malloc(sizeof(float)*nbWeights);
    for(int i = 0; i < nbWeights; i++)
        weights[i] = randomFloat(-1, 2);

    (*neuron).biais = randomFloat(-3, 3);
    (*neuron).weights = weights;
    (*neuron).nbWeights = nbWeights;

    return neuron;
}

/** Creates a new neuron with given biais and given weights array */
Neuron* buildNeuron(float biais, int nbWeights, float* weights) {

    //Allocates memory for the weights, the biais and the number of weights
    //(All the fields in the Neuron struct)
    Neuron* neuron = malloc(sizeof(float) + sizeof(float*) + sizeof(int) + sizeof(float)*2);
 
    (*neuron).weights = weights;
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
    //Saves the values of the sum before and after the activation function (used by the backpropagation algorithm)
    (*n).lastZ = sum;
    (*n).lastA = activation(sum);
    return (*n).lastA;
}

/** The activation function. The weighted sum passes through
 * this function before becoming the output of the neuron */
float activation(float x) {
    return 1.0f/(1.0f+expf(-x));
}

/** The derivative of the activation function (used by the learning algorithm) */
float activation_prime(float x) {
    float eminusx = expf(-x);
    return eminusx/((1+eminusx)*(1+eminusx));
}

void destroyNeuron(Neuron* neuron) {
    free((*neuron).weights);
    free(neuron);
}
