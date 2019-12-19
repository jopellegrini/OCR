#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "network.h"

// Documentation: http://neuralnetworksanddeeplearning.com/

// Building --------------------------------------------------------------------

/** Creates a new network with random weights and biaises
 * <br> Warning, the layer lengths are just copied, don't forget to free them
 * @param nbLayers - Number of layers
 * @param layerLengths - Number of neurons in each layer 
 * (including input and output layers) */
Network* newNetwork(int* layerLengths, int nbLayers) {

    Network* network = malloc(sizeof(Neuron**) + sizeof(int*)*2 + sizeof(int));

    //The index of the first neuron of each layer.
    //firstNeuronIndices[1] is the index of the first neuron of the 2nd layer 
    //(the one just after the input layer)
    //firstNeuronIndices[1] will always give 0 because the input layer doesn't 
    //contain any neuron.
    //firstNeuronIndices[0] will give -(the number of inputs)
    int* firstNeuronIndices = malloc(sizeof(int)*nbLayers);
    int lastIndex = -layerLengths[0]; //Index of 1st neuron of prev layer
    for (int i = 0; i < nbLayers; i++) {
        //Sets the index of the current layer 1st neuron + updates the lastIndex
        firstNeuronIndices[i] = lastIndex;
        lastIndex += layerLengths[i];
    }

    //The lastIndex is the index of the first neuron after the output layer now
    //So it's the number of neurons in the network, inputs excluded
    int nbNeurons = lastIndex;

    //Copies the layer lengths in a new array
    int* layerLengthsCopy = malloc(sizeof(int)*nbLayers);
    for(int i = 0; i < nbLayers; i++)
        layerLengthsCopy[i] = layerLengths[i];

    (*network).firstNeuronIndices = firstNeuronIndices;
    (*network).layerLengths = layerLengthsCopy;
    (*network).nbLayers = nbLayers;

    //Construction of the neurons
    Neuron** neurons = malloc(sizeof(Neuron*)*nbNeurons);
    //For each layer
    for (int layer = 1; layer < nbLayers; layer++) {
        //Precalculates the number of neurons of the layer, the number of neuron
        //of the previous layer and the index of the first neuron of the layer
        int layerLength = getLayerLength(network, layer);
        int previousLayerLength = getLayerLength(network, layer-1);
        int firstIndex = (*network).firstNeuronIndices[layer];
        //Creates all the neurons of the layer
        for (int neuron = 0; neuron < layerLength; neuron++)
            neurons[firstIndex + neuron] = randomNeuron(previousLayerLength);
    }

    (*network).neurons = neurons;

    return network;
}

// Simulation ------------------------------------------------------------------

/** Calculates the network answer over a specific input array (feed forward)
 * <br> The returned array is the outputs of the neurons of the output layer
 * <br> The length of the input array must = getInputNumber(network) */
double* getNetworkAnswer(Network* network, double* inputs) {

    double* nextLayerInputs = inputs; //Input array of the next layer to simulate
    double* outputs = NULL; //Array of outputs of the last layer simulated

    //For each layer
    for(int layer = 1; layer <= (*network).nbLayers-1; layer++) {

        //Gets the layer length and allocates memory for the output of the layer
        int layerLength = getLayerLength(network, layer);
        outputs = malloc(sizeof(double)*layerLength);

        //For each neuron of the layer
        for(int neuronIndex = 0; neuronIndex < layerLength; neuronIndex++) {

            //Simulates the neuron and stores its answer
            Neuron* neuron = (*network).neurons[
                (*network).firstNeuronIndices[layer]+neuronIndex
            ];
            outputs[neuronIndex] = simulate(neuron, nextLayerInputs);
        }

        //When outputs are calculated, they become the inputs of the next layer
        if(layer != 1) free(nextLayerInputs); //Avoids freeing the inputs array
        nextLayerInputs = outputs;
    }

    return outputs;
}

int getMaxIndex(double* rawAnswer, int length) {
    
    double max = -1e20;
    int maxIndex = 0;
    for(int i = 0; i < length; i++) {
        if(rawAnswer[i] > max) {
            max = rawAnswer[i];
            maxIndex = i;
        }
    }
    return maxIndex;
}

// Tools -----------------------------------------------------------------------

void destroyNetwork(Network* network) {
    int totalNeurons = getTotalNeurons(network);
    for(int i = 0; i < totalNeurons; i++)
        destroyNeuron((*network).neurons[i]);
    free((*network).layerLengths);
    free((*network).firstNeuronIndices);
    free(network);
}

/** Returns the number of neurons in the nth layer (0 is the input layer) */
int getLayerLength(Network* network, int layerIndex) { 
    return (*network).layerLengths[layerIndex]; 
}

/** Returns the number of outputs of the network (length of the output array) */
int getOutputNumber(Network* network) { 
    return (*network).layerLengths[(*network).nbLayers-1]; 
}

/** Returns the number of inputs of the network (length of the input array) */
int getInputNumber(Network* network) { 
    return (*network).layerLengths[0]; 
}

/** Returns the total number of neurons in the network */
int getTotalNeurons(Network* network) {
    //The total number of neurons is the index of the first neuron 
    //of the output layer + the number of outputs
    return (*network).firstNeuronIndices[(*network).nbLayers-1]
            + getOutputNumber(network);
}
