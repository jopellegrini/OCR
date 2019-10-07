#ifndef TESTS_NEURON_H
#define TESTS_NEURON_H

typedef struct Neuron Neuron;
struct Neuron {
    float* weights;
    float biais;
    int nbWeights;
};

Neuron* randomNeuron(int nbWeights);
Neuron* buildNeuron(float biais, int nbWeights, float* weights);

float simulate(Neuron* n, float* inputs);
float activation(float x);

void destroyNeuron(Neuron* neuron);

#endif