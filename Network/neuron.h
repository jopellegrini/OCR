#ifndef TESTS_NEURON_H
#define TESTS_NEURON_H

typedef struct {
    float* weights;  //The weights of the neuron
    float biais;     //The biais
    int nbWeights;   //The number of weights (length of the weights array)
    float lastA;     //Used only for learning purposes: The last output of the neuron
    float lastZ;     //Used only for learning purposes: The last output before application of the activation function
} Neuron;

Neuron* randomNeuron(int nbWeights);
Neuron* buildNeuron(float biais, int nbWeights, float* weights);

float simulate(Neuron* n, float* inputs);
float activation(float x);
float activation_prime(float x);

void destroyNeuron(Neuron* neuron);

#endif