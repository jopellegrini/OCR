#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "network.h"

// Documentation: http://neuralnetworksanddeeplearning.com/

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

/** Calculates the answer of the network over a specific input array (feed forward)
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
        costs[i] /= (float)nbExamples*2; //The *2 here makes the cost derivative simpler

    return costs;
}

/** Updates the weights and biaises to improve answers to given examples
 * <br> eta is the learning rate (bigger = less precise, lower = slower). in [0,1]
 * <br> Uses the backpropagation algorithm to calculate the gradient of the cost of each example
 * and then modifies each weight and biais proportionaly to the average of these gradients */
void learn(Network* network, float eta, TrainingExample** examples, int nbExamples) {

    int totalNeurons = getTotalNeurons(network);

    //Cost gradient for each weight (output of the backpropagation).
    //deltaW[i][j] is the adjustment to make on the jth weight of the ith neuron
    float** deltaW = malloc(sizeof(float*)*totalNeurons);
    for(int i = 0; i < totalNeurons; i++)
        deltaW[i] = malloc(sizeof(float)*(network->neurons[i]->nbWeights));
    //Cost gradient for each biais (output of the backpropagation).
    //deltaB[i] is the adjustment to make on the biais of the ith neuron
    float* deltaB = malloc(sizeof(float)*totalNeurons);

    //Initialises all the deltas to 0
    for(int i = 0; i < totalNeurons; i++) {
        for(int j = 0; j < network->neurons[i]->nbWeights; j++)
            deltaW[i][j] = 0;
        deltaB[i] = 0;
    }

    //Calculates the cost gradient for each example and sums all the adjustments to make (to do it only once)
    for(int exampleIndex = 0; exampleIndex < nbExamples; exampleIndex++) {

        //Calculates the gradient of the cost for this example
        float** exampleDeltaW = malloc(sizeof(float*)*totalNeurons); //Same as deltaW but for this example only
        for(int i = 0; i < totalNeurons; i++)
            exampleDeltaW[i] = malloc(sizeof(float)*(network->neurons[i]->nbWeights));
        float* exampleDeltaB = malloc(sizeof(float)*totalNeurons);   //Same as deltaB but for this example only
        //The output of this function are exampleDeltaW and exampleDeltaB
        getCostGradient(network, examples[exampleIndex], exampleDeltaW, exampleDeltaB);

        //Adds the values of the gradient of the cost for this example to deltaW and deltaB
        for(int i = 0; i < totalNeurons; i++) {
            deltaB[i] += exampleDeltaB[i];
            int nbWeights = network->neurons[i]->nbWeights;
            for(int j = 0; j < nbWeights; j++)
                deltaW[i][j] += exampleDeltaW[i][j];
        }

        //Frees the memory for the current example
        for(int i = 0; i < totalNeurons; i++)
            free(exampleDeltaW[i]);
        free(exampleDeltaB);
        free(exampleDeltaW);
    }

    //All the values of the gradient will be scaled by this (negative to descend the gradient)
    //nbExample to make the average instead of the sum of all the costs)
    //eta is the learning rate (bigger = less precise, lower = slower)
    float scale = -eta/(float)nbExamples;

    //Each weight/biais gets added -eta/nbExamples * correspondingDelta
    for(int i = 0; i < totalNeurons; i++) {
        Neuron* neuron = network->neurons[i];
        for(int j = 0; j < neuron->nbWeights; j++)
            neuron->weights[j] += scale * deltaW[i][j];
        neuron->biais += scale * deltaB[i];
    }

    //Frees the memory used for the gradient
    for(int i = 0; i < totalNeurons; i++)
        free(deltaW[i]);
    free(deltaB);
    free(deltaW);
}

/** Returns the gradient of the cost (Backpropagation algorithm).
 * The outputs are in deltaW and deltaB (Any value in these array will be erased) */
void getCostGradient(Network* network, TrainingExample* example, float** deltaW, float* deltaB) {

    //1st step: get the activation values before and after the activation for all the neurons

    //Does a first feed forward to update the lastA and lastZ values stored in the neurons
    //lastA is the last output of the neuron and the last output before application of the activation function
    free(getNetworkAnswer(network, example->inputs));

    //2nd step: get the deltas (error) for the last layer

    int outputNumber = getOutputNumber(network);
    int firstOutputIndex = network->firstNeuronIndices[network->nbLayers-1];
    float* deltas = malloc(sizeof(float)* outputNumber);

    //Calculates the delta for the last layer. For each neuron: delta = C' * o'(zi).
    //o' is the derivative of activation function, zi is the activation of the neuron before the activation function
    //C' is the derivative of the cost: ai - yi (ai is the activation of the neuron, yi is the label for this neuron)
    for(int i = 0; i < outputNumber; i++) {
        Neuron* neuron = network->neurons[firstOutputIndex+i];
        deltas[i] = (neuron->lastA - example->label[i]) * activation_prime(neuron->lastZ);
    }

    //Does the 4th step now for this layer so we don't have to store all the deltas
    //Calculation of the gradient for the output layer
    int outputLayerIndex = network->nbLayers-1;
    int previousLayerLength = network->layerLengths[outputLayerIndex-1];
    for(int neuronIndex = 0; neuronIndex < outputNumber; neuronIndex++) {
        deltaB[firstOutputIndex + neuronIndex] = deltas[neuronIndex];
        for (int weightIndex = 0; weightIndex < previousLayerLength; weightIndex++)
            deltaW[firstOutputIndex + neuronIndex][weightIndex] = deltas[neuronIndex]
                    * network->neurons[network->firstNeuronIndices[outputLayerIndex-1] + weightIndex]->lastA;
    }

    //3rd step: get the deltas (error) for the all the hidden layers

    //Calculates the deltas for each hidden layer (starts at the one before the output layer and ends at the one
    //before the input layer). The calculation is explained below
    for(int layerIndex = network->nbLayers-2; layerIndex >= 1; layerIndex--) {

        //Stores the deltas calculated one step before, so the deltas of the next layer
        float* nextLayerDeltas = deltas;

        //Some useful values
        int layerLength = network->layerLengths[layerIndex];
        int nextLayerLength = network->layerLengths[layerIndex+1];
        int nextLayerFirstNeuronIndex = network->firstNeuronIndices[layerIndex+1];
        int firstNeuronIndex = network->firstNeuronIndices[layerIndex];
        //Allocates memory for the deltas of the current layer
        deltas = malloc(sizeof(float)*layerLength);

        //For each neuron of the current layer
        for(int neuronIndex = 0; neuronIndex < layerLength; neuronIndex++) {

            //The delta for each neuron is the sum of a value calculated for each neuron of the next layer
            //multiplied by the activation of the neuron before application of the activation function
            float sum = 0;
            //For each neuron of the next layer
            for(int nextLayerNeuronIndex = 0; nextLayerNeuronIndex < nextLayerLength; nextLayerNeuronIndex++) {
                //The value added to the sum is: w*d
                //Let "this neuron" refer to the neuron nextLayerNeuronIndex (so it's in the next layer)
                //w: The weight of the neuron neuronIndex (the value multiplied by the neuron neuronIndex's activation
                //during this neuron's simulation during feed forward).
                //d: The delta of this neuron
                sum += network->neurons[nextLayerFirstNeuronIndex + nextLayerNeuronIndex]->weights[neuronIndex]
                        * nextLayerDeltas[nextLayerNeuronIndex];
            }
            //Multiplication by o'(z)
            deltas[neuronIndex] = sum * activation_prime(network->neurons[neuronIndex + firstNeuronIndex]->lastZ);
        }

        // 4th step: Calculate the gradient

        //Inside the 3rd step loop so we can free the delta arrays
        previousLayerLength = network->layerLengths[layerIndex-1];
        for(int neuronIndex = 0; neuronIndex < layerLength; neuronIndex++) {
            //delta refers to the delta of the corresponding neuron
            //The biais components are given by: dC/db = delta
            deltaB[network->firstNeuronIndices[layerIndex] + neuronIndex] = deltas[neuronIndex];
            //The weights are given by dC/dw_k = a_(l-1)_k * delta
            for(int weightIndex = 0; weightIndex < previousLayerLength; weightIndex++)
                deltaW[network->firstNeuronIndices[layerIndex] + neuronIndex][weightIndex] = deltas[neuronIndex]
                        * (layerIndex > 1 //If the layer is the one after the inputs, the activations are in the example
                            ? network->neurons[network->firstNeuronIndices[layerIndex-1] + weightIndex]->lastA
                            : example->inputs[weightIndex]);
        }

        free(nextLayerDeltas);
    }

    free(deltas);
}

// Serialization/Parsing -----------------------------------------------------------------------------------------------

//Serialized networks are of this form:
// 
// nbLayers nbInputs (nbNeuronsHiddenLayer)* nbOutputs
// (nbWeights biais (weight)+)+   //For each neuron

//Adds an int to the buffer and returns a pointer to the first char after the added int and the end character.
char* writeint(char* buffer, int i, char end) {
    int size = sprintf(buffer, "%i", i);
    buffer[size] = end;
    return &buffer[size+1];
}

//Adds an float to the buffer and returns a pointer to the first char after the added float and the end character
char* writefloat(char* buffer, float f, char end) {
    int size = sprintf(buffer, "%f", f);
    buffer[size] = end;
    return &buffer[size+1];
}

char* serializeNetwork(Network* network) {

    //Allocates the memory needed to store the string (it will then be copied to another string with the exact space)
    //A float as a string is at most 13 characters long (x.xxxxxxe-xxx). Integer values are considered 3 digits long.
    //Each neuron will be serialized with its number of weights, its biais and its weights, separated by spaces.
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
    current = writeint(current, (*network).nbLayers, ' ');
    //Adds the number of neurons in each layer
    for(int i = 0; i < (*network).nbLayers; i++)
        current = writeint(current, (*network).layerLengths[i], ' ');
    //Replaces the last space by a line break
    current[-1] = '\n';

    //Writes each neuron
    for(int i = 0; i < totalNeurons; i++) {
        Neuron* neuron = (*network).neurons[i];
        current = writeint(current, (*neuron).nbWeights, ' ');
        current = writefloat(current, (*neuron).biais, ' ');
        for(int j = 0; j < (*neuron).nbWeights; j++)
            current = writefloat(current, (*neuron).weights[j], ' ');
        //Replaces the last space by a line break
        current[-1] = '\n';
    }

    //Marks the end of the serialization
    current[0] = '\0';

    //Copies the result in a string with exactly the right length
    unsigned long length = strlen(res)+1; //+1 to include the null terminator
    char* resized = malloc(sizeof(char)*length);
    for(unsigned long i = 0; i < length; i++)
        resized[i] = res[i];
    free(res);

    return resized;
}

//Reads an int from the str and returns it
//str gets automaticaly moved to the end of the str +1 char
//ex: "12 78" -> "78"
int readint(char** str) {

    char* s = *str;

    //Puts a null terminator at the end so the atoi function works
    int separatorPos = 0;
    while(s[separatorPos] != 0 && s[separatorPos] != ' ' && s[separatorPos] != '\n')
        separatorPos++;
    char separator = s[separatorPos];
    s[separatorPos] = '\0';
    
    int res = atoi(s);
    
    //Puts back the separator and moves the str pointer
    s[separatorPos] = separator;
    *str = s+separatorPos+1;

    return res;
}

//Reads an float from the str and returns it
//str gets automaticaly moved to the end of the str +1 char
//ex: "12.0 78.0" -> "78.0"
float readfloat(char** str) {
        
    char* s = *str;

    //Puts a null terminator at the end so the atoi function works
    int separatorPos = 0;
    while(s[separatorPos] != 0 && s[separatorPos] != ' ' && s[separatorPos] != '\n')
        separatorPos++;
    char separator = s[separatorPos];
    s[separatorPos] = '\0';
    
    float res = strtof(s, NULL);
    
    //Puts back the separator and moves the str pointer
    s[separatorPos] = separator;
    *str = s+separatorPos+1;

    return res;
}

Network* parseNetwork(char* data) {
    
    //Avoids the data variable being altered
    char* stream = data;

    //Reads the first line (number of layers and length of the layers)
    int nbLayers = readint(&stream);
    int* layerLengths = malloc(nbLayers);
    for(int i = 0; i < nbLayers; i++)
        layerLengths[i] = readint(&stream);
    
    //Creates the network now that the size is known
    Network* network = newNetwork(layerLengths, nbLayers);
    free(layerLengths); //The newNetwork function copies the layerLengths so this array is useless

    //Reads all the neurons
    int neuronIndex = 0;
    while(stream[0] != 0) {
        
        int nbWeights = readint(&stream);
        float* weights = malloc(sizeof(float)*nbWeights);
        float biais = readfloat(&stream);

        for(int i = 0; i < nbWeights; i++)
            weights[i] = readfloat(&stream);
        
        network->neurons[neuronIndex] = buildNeuron(biais, nbWeights, weights);
        neuronIndex++;
    }    

    return network;
}

// Tools ---------------------------------------------------------------------------------------------------------------

void destroyNetwork(Network* network) {
    int totalNeurons = getTotalNeurons(network);
    for(int i = 0; i < totalNeurons; i++)
        destroyNeuron((*network).neurons[i]);
    free((*network).layerLengths);
    free((*network).firstNeuronIndices);
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
