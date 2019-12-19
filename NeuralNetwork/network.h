#ifndef NETWORK_H
#define NETWORK_H

#include "neuron.h"

typedef struct {
    //All the neurons of the network. The layers are stored one after the other
    //The input layer doesn't contain neurons because it's just the input values
    Neuron** neurons;
    //The index of the first neuron of each layer.
    //firstNeuronIndices[1] is the index of the first neuron 
    //of the 2nd layer (the one just after the input layer)
    //firstNeuronIndices[1] will always give 0 because the 
    //input layer doesn't contain any neuron.
    //firstNeuronIndices[0] will give -(the number of inputs)
    int* firstNeuronIndices;
    //The length of each layer. layerLengths[0] gives the number of inputs.
    int* layerLengths;
    //The number of layers (inputs included)
    int nbLayers;
} Network;

Network* newNetwork(int* layerLengths, int nbLayers);

double* getNetworkAnswer(Network* network, double* inputs);
int getMaxIndex(double* rawAnswer, int length);

void destroyNetwork(Network* network);
int getLayerLength(Network* network, int layerIndex);
int getOutputNumber(Network* network);
int getInputNumber(Network* network);
int getTotalNeurons(Network* network);

#endif //NETWORK_H
