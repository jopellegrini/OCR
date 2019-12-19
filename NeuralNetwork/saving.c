#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "saving.h"
#include "file_utils.h"

//Serialized networks are of this form:
// 
// nbLayers nbInputs (nbNeuronsHiddenLayer)* nbOutputs
// (nbWeights biais (weight)+)+   //For each neuron

void saveNetwork(Network* network, char* path) {
    writefile(path, serializeNetwork(network));
}

Network* readNetwork(char* path) {
    return parseNetwork(readfile(path));
}

char* serializeNetwork(Network* network) {

    //Allocates the memory needed to store the string 
    //(it will then be copied to another string with the exact space)
    //A double as a string is at most 13 characters long 
    //(x.xxxxxxe-xxx). Integer values are considered 3 digits long.
    //Each neuron will be serialized with its number of 
    //weights, its biais and its weights, separated by spaces.
    //Neurons are separated by line breaks. The first line 
    //wich contains the number of layers followed by the number of
    //neurons for each layer. Each neuron is (weights+1)*14 + 4 characters long.
    int totalWeights = 0;
    int totalNeurons = getTotalNeurons(network);
    for(int i = 0; i < totalNeurons; i++)
        totalWeights += network->neurons[i]->nbWeights;
    char* res = malloc( sizeof(char) * 
            ((totalWeights+totalNeurons)*14+totalNeurons*4 
             + ((*network).nbLayers+1)*3) );

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
        current = writedouble(current, (*neuron).biais, ' ');
        for(int j = 0; j < (*neuron).nbWeights; j++)
            current = writedouble(current, (*neuron).weights[j], ' ');
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
    free(layerLengths); //The newNetwork function copies the 
    //layerLengths so this array is useless

    //Reads all the neurons
    int neuronIndex = 0;
    while(stream[0] != 0) {
        
        int nbWeights = readint(&stream);
        double* weights = malloc(sizeof(double)*nbWeights);
        double biais = readdouble(&stream);

        for(int i = 0; i < nbWeights; i++)
            weights[i] = readdouble(&stream);
        
        network->neurons[neuronIndex] = buildNeuron(biais, nbWeights, weights);
        neuronIndex++;
    }    

    return network;
}

