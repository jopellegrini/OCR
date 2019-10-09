#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "network.h"

// Building ------------------------------------------------------------------------------------------------------------

/** Creates a new network with random weights and biaises
 * <br> Warning, the layer lengths are just copied, don't forget to free them
 * @param nbLayers - Number of layers
 * @param layerLengths - Number of neurons in each layer (including input and output layers) */
Network* newNetwork(int* layerLengths, int nbLayers) {

    Network* network = malloc(sizeof(Neuron**) + sizeof(int*)*2 + sizeof(int));

    //The index of the first neuron of each layer.
    //firstNeuronIndices[1] is the index of the first neuron of the 2nd layer (the one just after the input layer)
    //firstNeuronIndices[1] will always give 0 because the input layer doesn't contain any neuron.
    //firstNeuronIndices[0] will give -(the number of inputs)
    int* firstNeuronIndices = malloc(sizeof(int)*nbLayers);
    int lastIndex = -layerLengths[0]; //The index of the first neuron of the previous layer
    for (int i = 0; i < nbLayers; i++) {
        //Sets the index of the first neuron of the current layer and updates the lastIndex
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
        //Precalculates the number of neurons of the layer, the number of neurons
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

// Simulation ----------------------------------------------------------------------------------------------------------

/** Calculates the answer of the network over a specific input array
 * <br> The returned array is the array of the outputs of the neurons of the output layer
 * <br> The length of the input array must be equal to getInputNumber(network) */
float* getNetworkAnswer(Network* network, float* inputs) {

    float* nextLayerInputs = inputs; //Input array of the next layer to simulate
    float* outputs = NULL; //Array of outputs of the last layer simulated

    //For each layer
    for(int layer = 1; layer <= (*network).nbLayers-1; layer++) {

        //Gets the length of the layer and allocates the memory for the output of the layer
        int layerLength = getLayerLength(network, layer);
        outputs = malloc(sizeof(float)*layerLength);

        //For each neuron of the layer
        for(int neuronIndex = 0; neuronIndex < layerLength; neuronIndex++) {

            //Simulates the neuron and stores its answer
            Neuron* neuron = (*network).neurons[(*network).firstNeuronIndices[layer]+neuronIndex];
            outputs[neuronIndex] = simulate(neuron, nextLayerInputs);
        }

        //When the outputs are calculated, they become the inputs of the next layer
        if(layer != 1) free(nextLayerInputs); //The if avoids freeing the inputs array
        nextLayerInputs = outputs;
    }

    return outputs;
}

// Training ------------------------------------------------------------------------------------------------------------

float sqr(float x) { return x*x; }

float* cost(Network* network, int nbExamples, float** inputs, float** labels) {

    //Creates an array to store the cost for each output neuron
    int nbOutputs = getOutputNumber(network);
    float* costs = malloc(sizeof(float)*nbOutputs);
    for(int i = 0; i < nbOutputs; i++)
        costs[i] = 0;

    //For each example, for each neuron, add the square of the difference between the
    //expected output and the given output to the cost
    for(int example = 0; example < nbExamples; example++) {
        float* output = getNetworkAnswer(network, inputs[example]);
        for (int outputNeuron = 0; outputNeuron < nbOutputs; outputNeuron++)
            costs[outputNeuron] += sqr(output[outputNeuron] - labels[example][outputNeuron]);
        free(output);
    }

    for(int i = 0; i < nbOutputs; i++)
        costs[i] /= (float)nbExamples;

    return costs;
}

// Serialization/Parsing -----------------------------------------------------------------------------------------------

//Adds an int to the buffer and returns a pointer to the first char after the added int and the end character.
char* addint(char* buffer, int i, char end) {
    int size = sprintf(buffer, "%i", i);
    buffer[size] = end;
    return &buffer[size+1];
}

//Adds an float to the buffer and returns a pointer to the first char after the added float and the end character
char* addfloat(char* buffer, float f, char end) {
    int size = sprintf(buffer, "%f", f);
    buffer[size] = end;
    return &buffer[size+1];
}

char* serialize(Network* network) {

    //Allocates the memory needed to store the string (it will then be copied to another string with the exact space)
    //A float as a string is at most 13 characters long (x.xxxxxxe-xxx). Integer values are considered 3 digits long.
    //Each neuron will be serialized with its number of weights, his biais and his weights, separated by spaces.
    //Neurons are separated by line breaks. The first line wich contains the number of layers followed by the number of
    //neurons for each layer. Each neuron is (weights+1)*14 + 4 characters long.
    int totalWeights = 0;
    int totalNeurons = getTotalNeurons(network);
    for(int i = 0; i < totalNeurons; i++)
        totalWeights += network->neurons[i]->nbWeights;
    char* res = malloc( sizeof(char) * ((totalWeights+totalNeurons)*14+totalNeurons*4 + ((*network).nbLayers+1)*3) );

    //Stores the position of the cursor
    char* current = res;

    //Adds the number of layers
    current = addint(current, (*network).nbLayers, ' ');
    //Adds the number of neurons in each layer
    for(int i = 1; i < (*network).nbLayers; i++)
        current = addint(current, (*network).layerLengths[i], ' ');
    //Replaces the last space by a line break
    current[-1] = '\n';

    //Writes each neuron
    for(int i = 0; i < totalNeurons; i++) {
        Neuron* neuron = (*network).neurons[i];
        current = addint(current, (*neuron).nbWeights, ' ');
        current = addfloat(current, (*neuron).biais, ' ');
        for(int j = 0; j < (*neuron).nbWeights; j++)
            current = addfloat(current, (*neuron).weights[j], ' ');
        //Replaces the last space by a line break
        current[-1] = '\n';
    }

    //Marks the end of the serialization
    current[0] = '\0';

    //Copies the result in a string with exactly the right length
    unsigned long length = strlen(res);
    char* resized = malloc(sizeof(char)*length);
    for(unsigned long i = 0; i < length; i++)
        resized[i] = res[i];
    free(res);

    return resized;
}

// Tools ---------------------------------------------------------------------------------------------------------------

void destroyNetwork(Network* network) {
    free((*network).layerLengths);
    free((*network).firstNeuronIndices);
    int totalNeurons = getTotalNeurons(network);
    for(int i = 0; i < totalNeurons; i++)
        destroyNeuron((*network).neurons[i]);
    free(network);
}

/** Returns the number of neurons in the nth layer (0 is the input layer) */
int getLayerLength(Network* network, int layerIndex) { return (*network).layerLengths[layerIndex]; }
/** Returns the number of outputs of the network (length of the output array) */
int getOutputNumber(Network* network) { return (*network).layerLengths[(*network).nbLayers-1]; }
/** Returns the number of inputs of the network (length of the input array) */
int getInputNumber(Network* network) { return (*network).layerLengths[0]; }
/** Returns the total number of neurons in the network */
int getTotalNeurons(Network* network) {
    //The total number of neurons is the index of the first neuron of the output layer + the number of outputs
    return (*network).firstNeuronIndices[(*network).nbLayers-1]
            + getOutputNumber(network);
}
