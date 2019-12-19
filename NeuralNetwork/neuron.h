#ifndef NEURON_H
#define NEURON_H

typedef struct {
    double* weights;  //The weights of the neuron
    double biais;     //The biais
    int nbWeights;   //The number of weights (length of the weights array)
    //The last 2 variables are only used during learning
    double lastA;     //The last output
    double lastZ;     //Same but before application of the activation function
} Neuron;

Neuron* randomNeuron(int nbWeights);
Neuron* buildNeuron(double biais, int nbWeights, double* weights);

double simulate(Neuron* n, double* inputs);
double activation(double x);
double activation_prime(double x);

void destroyNeuron(Neuron* neuron);

#endif //NEURON_H
