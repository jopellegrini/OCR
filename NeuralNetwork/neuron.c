#include <stdlib.h>
#include <math.h>

#include "neuron.h"

/** Generates a random double between min and max */
double randomDouble(double min, double max) {
    //Don't forget to initialise the rand() function somewhere in the program:
    //srand((unsigned int)time(NULL));
    return (max - min) * (double)rand()/(double)(RAND_MAX) + min;
}

/** Creates a new neuron with random weights and random biais */
Neuron* randomNeuron(int nbWeights) {

    //Allocates memory for the weights, the biais and the number of weights
    //(All the fields in the Neuron struct)
    Neuron* neuron = malloc(
            sizeof(double) + sizeof(double*) + sizeof(int) + sizeof(double)*2);
    
    //Generates random weights and random biais
    double* weights = malloc(sizeof(double)*nbWeights);
    for(int i = 0; i < nbWeights; i++)
        weights[i] = randomDouble(-1.5, 1.5);

    (*neuron).biais = randomDouble(-1.5, 1.5);
    (*neuron).weights = weights;
    (*neuron).nbWeights = nbWeights;

    return neuron;
}

/** Creates a new neuron with given biais and given weights array */
Neuron* buildNeuron(double biais, int nbWeights, double* weights) {

    //Allocates memory for the weights, the biais and the number of weights
    //(All the fields in the Neuron struct)
    Neuron* neuron = malloc(
            sizeof(double) + sizeof(double*) + sizeof(int) + sizeof(double)*2);
 
    (*neuron).weights = weights;
    (*neuron).biais = biais;
    (*neuron).nbWeights = nbWeights;

    return neuron;
}

/** Calculates the answer of the neuron for a specific inputs array
 * <br> The inputs array length must = the number of weights of the neuron */
double simulate(Neuron* n, double* inputs) {
    double sum = (*n).biais; //Adds the biais to the weighted sum
    //Sums all the inputs multiplied by the weights
    for(int i = 0; i < (*n).nbWeights; i++)
        sum += (*n).weights[i] * inputs[i];
    //Saves the values of the sum before and after the activation function 
    //(used by the backpropagation algorithm)
    (*n).lastZ = sum;
    (*n).lastA = activation(sum);
    return (*n).lastA;
}

/** The activation function. The weighted sum passes through
 * this function before becoming the output of the neuron */
double activation(double x) {
    return 1.0/(1.0+exp(-x));
}

/** The activation function derivative (used by the learning algorithm) */
double activation_prime(double x) {
    double eminusx = exp(-x);
    if(isinf(eminusx))
        return 0;
    return eminusx/((1+eminusx)*(1+eminusx));
}

void destroyNeuron(Neuron* neuron) {
    free((*neuron).weights);
    free(neuron);
}
