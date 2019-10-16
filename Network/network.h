#ifndef TESTS_NETWORK_H
#define TESTS_NETWORK_H

#include "neuron.h"
#include "training_example.h"

//TODO: Enlever la declaration de struct + include neuron

typedef struct {
    //All the neurons of the network. The layers are stored one after the other
    //The input layer doesn't contain any neuron because it's just the input values
    Neuron** neurons;
    //The index of the first neuron of each layer.
    //firstNeuronIndices[1] is the index of the first neuron of the 2nd layer (the one just after the input layer)
    //firstNeuronIndices[1] will always give 0 because the input layer doesn't contain any neuron.
    //firstNeuronIndices[0] will give -(the number of inputs)
    int* firstNeuronIndices;
    //The length of each layer. layerLengths[0] gives the number of inputs.
    int* layerLengths;
    //The number of layers (inputs included. ex: inputs - hidden - hidden - outputs => 4)
    int nbLayers;
} Network;

Network* newNetwork(int* layerLengths, int nbLayers);

float* getNetworkAnswer(Network* network, float* inputs);

float* cost(Network* network, int nbExamples, float** inputs, float** labels);
void learn(Network* network, float eta, TrainingExample** examples, int nbExamples);
void getCostGradient(Network* network, TrainingExample* example, float** deltaW, float* deltaB);

char* serializeNetwork(Network* network);
Network* parseNetwork(char* data);

void destroyNetwork(Network* network);
int getLayerLength(Network* network, int layerIndex);
int getOutputNumber(Network* network);
int getInputNumber(Network* network);
int getTotalNeurons(Network* network);

#endif
