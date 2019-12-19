#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "training.h"

#define ETA 0.1
//#define USE_CROSS_ENTROPY

// Tool functions --------------------------------------------------------------

double sqr(double a) { return a*a; }

//Returns the cost of an example. y: the desired activation; a: the actual one
double quadratic_cost(double a, double y) {
    return 0.5 * sqr(a-y); 
}

double crossentropy_cost(double a, double y) {
    
    double res;
    
    if(y == 0.0) 
        res = -log(1-a);
    else if(y == 1.0) 
        res = -log(a);
    
    else res = -(y*log(a) + (1-y)*log(1-a));
    
    return isnan(res) ? 0.0 : res;
}

/** Returns the gradient of the cost (Backpropagation algorithm).
 * The outputs are in delta[WB] (Any value in these array will be erased) */
void getCostGradient(Network* network, TrainingExample* example, 
                     double** deltaW, double* deltaB) {

    //1st step: get the activation values before and 
    //after the activation for all the neurons

    //Does feed forward to update the lastA and lastZ stored in the neurons
    //lastA is the last output of the neuron and 
    //the last output before application of the activation function
    free(getNetworkAnswer(network, example->inputs));

    //2nd step: get the deltas (error) for the last layer

    int outputNumber = getOutputNumber(network);
    int firstOutputIndex = network->firstNeuronIndices[network->nbLayers-1];
    double* deltas = malloc(sizeof(double)* outputNumber);

    //Calculates the last layer delta. For each neuron: delta = C' * o'(zi).
    //o' is the derivative of activation function, zi
    //is the activation of the neuron before the activation function
    //C' is the derivative of the cost
    for(int i = 0; i < outputNumber; i++) {
        Neuron* neuron = network->neurons[firstOutputIndex+i];
        deltas[i] = (neuron->lastA - example->label[i])
#ifndef USE_CROSS_ENTROPY
            * activation_prime(neuron->lastZ)
#endif
        ;
    }

    //Does the 4th step now for this layer so we don't have to store the deltas
    //Calculation of the gradient for the output layer
    int outputLayerIndex = network->nbLayers-1;
    int previousLayerLength = network->layerLengths[outputLayerIndex-1];
    for(int neuronIndex = 0; neuronIndex < outputNumber; neuronIndex++) {
        deltaB[firstOutputIndex + neuronIndex] = deltas[neuronIndex];
        for (int weightIndex = 0; weightIndex < previousLayerLength; weightIndex++) {
            //If the network contains only one real layer 
            //the activations are in the example
            double act = network->nbLayers > 2
                ? network->neurons[
                        network->firstNeuronIndices[network->nbLayers-2]+weightIndex
                    ]->lastA
                : example->inputs[weightIndex];
               
            deltaW[firstOutputIndex + neuronIndex][weightIndex] = 
                act * deltas[neuronIndex];     
        }
    }

    //3rd step: get the deltas (error) for the all the hidden layers

    //Calculates the deltas for each hidden layer (starts at the one before 
    //the output layer and ends at the one before the input layer). 
    //The calculation is explained below
    for(int layerIndex = network->nbLayers-2; layerIndex >= 1; layerIndex--) {

        //Stores the deltas calculated one step before, so the 
        //deltas of the next layer
        double* nextLayerDeltas = deltas;

        //Some useful values
        int layerLength = network->layerLengths[layerIndex];
        int nextLayerLength = network->layerLengths[layerIndex+1];
        int nextLayerFirstNeuronIndex = network->firstNeuronIndices[layerIndex+1];
        //Allocates memory for the deltas of the current layer
        deltas = malloc(sizeof(double)*layerLength);

        //For each neuron of the current layer
        for(int neuronIndex = 0; neuronIndex < layerLength; neuronIndex++) {

            //The delta for each neuron is the sum of a value calculated 
            //for each neuron of the next layer
            //multiplied by the activation of the neuron before 
            //application of the activation function
            double sum = 0;
            //For each neuron of the next layer
            for(int nextLayerNeuronIndex = 0; nextLayerNeuronIndex < nextLayerLength; nextLayerNeuronIndex++) {
                //The value added to the sum is: w*d
                //Let "this neuron" refer to the neuron nextLayerNeuronIndex 
                //(so it's in the next layer)
                //w: The weight of the neuron neuronIndex 
                //(the value multiplied by the neuron neuronIndex's activation
                //during this neuron's simulation during feed forward).
                //d: The delta of this neuron
                sum += network
                    ->neurons[nextLayerFirstNeuronIndex + nextLayerNeuronIndex]
                    ->weights[neuronIndex]
                        * nextLayerDeltas[nextLayerNeuronIndex];
            }
            //Multiplication by o'(z)
            deltas[neuronIndex] = sum
#ifndef USE_CROSS_ENTROPY
                 * activation_prime(network->neurons[
                        neuronIndex + network->firstNeuronIndices[layerIndex]
                    ]->lastZ)        
#endif
                 ;
        }

        // 4th step: Calculate the gradient

        //Inside the 3rd step loop so we can free the delta arrays
        previousLayerLength = network->layerLengths[layerIndex-1];
        for(int neuronIndex = 0; neuronIndex < layerLength; neuronIndex++) {
            //delta refers to the delta of the corresponding neuron
            //The biais components are given by: dC/db = delta
            deltaB[network->firstNeuronIndices[layerIndex] + neuronIndex] = 
                deltas[neuronIndex];
            //The weights are given by dC/dw_k = a_(l-1)_k * delta
            for(int weightIndex = 0; weightIndex < previousLayerLength; weightIndex++) {

                //If the layer is the one after the inputs, 
                //the activations are in the example
                double act = (layerIndex > 1
                    ? network
                        ->neurons[network->firstNeuronIndices[layerIndex-1] + weightIndex]
                        ->lastA
                    : example->inputs[weightIndex]);
            
                
                deltaW
                    [network->firstNeuronIndices[layerIndex] + neuronIndex]
                    [weightIndex] 
                    = act * deltas[neuronIndex];
            }
        }

        free(nextLayerDeltas);
    }

    free(deltas);
}

/** Calculates the sum of the costs gradient for each example
 *  deltaW and deltaB have to be pointers to NULL array pointers,
 *  they will be initialised in the function */
void getCostGradientSum(Network* network, 
                        TrainingExample** examples, int nbExamples, 
                        double*** deltaW, double** deltaB) {
  
    int totalNeurons = getTotalNeurons(network);

    //Cost gradient for each weight (output of the backpropagation).
    //deltaW[i][j] is the adjustment to make on the jth weight of the ith neuron
    *deltaW = malloc(sizeof(double*)*totalNeurons);
    for(int i = 0; i < totalNeurons; i++)
        (*deltaW)[i] = calloc(network->neurons[i]->nbWeights, sizeof(double));
    //Cost gradient for each biais (output of the backpropagation).
    //deltaB[i] is the adjustment to make on the biais of the ith neuron
    *deltaB = calloc(totalNeurons, sizeof(double));

    //Calculates the cost gradient for each example 
    //and sums all the adjustments to make (to do it only once)
    for(int exampleIndex = 0; exampleIndex < nbExamples; exampleIndex++) {

        //Calculates the gradient of the cost for this example
        //exampleDelta[WB] are like delta[WB] but for this example only
        double** exampleDeltaW = malloc(sizeof(double*)*totalNeurons); 
        for(int i = 0; i < totalNeurons; i++)
            exampleDeltaW[i] = malloc(
                    sizeof(double)*(network->neurons[i]->nbWeights));
        double* exampleDeltaB = malloc(
                sizeof(double)*totalNeurons);
        //The output of this function are exampleDeltaW and exampleDeltaB
        getCostGradient(network, examples[exampleIndex], 
                exampleDeltaW, exampleDeltaB);

        //Adds the values of the gradient of the cost 
        //for this example to deltaW and deltaB
        for(int i = 0; i < totalNeurons; i++) {
            (*deltaB)[i] += exampleDeltaB[i];
            int nbWeights = network->neurons[i]->nbWeights;
            for(int j = 0; j < nbWeights; j++)
                (*deltaW)[i][j] += exampleDeltaW[i][j];
        }

        //Frees the memory for the current example
        for(int i = 0; i < totalNeurons; i++)
            free(exampleDeltaW[i]);
        free(exampleDeltaB);
        free(exampleDeltaW);
    }
}

// Public functions ------------------------------------------------------------

/** Updates the weights and biaises to improve answers to given examples
 * Uses the backpropagation algorithm to calculate the 
 * gradient of the cost of each example
 * and then modifies each weight and biais proportionaly 
 * to the average of these gradients */
void learnOneBatch(Network* network, TrainingExample** examples, int nbExamples) {

    int totalNeurons = getTotalNeurons(network);

    // Gets the average costs gradient for all the training examples
    double** deltaW = NULL;
    double* deltaB = NULL;
    getCostGradientSum(network, examples, nbExamples, &deltaW, &deltaB);

    //All the values of the gradient will be scaled by 
    //this (negative to descend the gradient)
    //nbExample to make the average instead of the sum of all the costs)
    //eta is the learning rate (bigger = less precise, lower = slower)
    double scale = -ETA/(double)nbExamples;

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

/** Just like learnOneBatch, but the previousDelta[WB] are added to the gradient
 * After execution previousDelta[WB] contains the deltas used in the function */
void learnWithMomentum(Network* network, 
                       TrainingExample** examples, int nbExamples, 
                       double** previousDeltaW, double* previousDeltaB) {

    int totalNeurons = getTotalNeurons(network);

    // Gets the average costs gradient for all the training examples
    double** deltaW = NULL;
    double* deltaB = NULL;
    getCostGradientSum(network, examples, nbExamples, &deltaW, &deltaB);

    //All the values of the gradient will be scaled by 
    //this (negative to descend the gradient)
    //nbExample to make the average instead of the sum of all the costs)
    //eta is the learning rate (bigger = less precise, lower = slower)
    double scale = -ETA/(double)nbExamples;

    //Each weight/biais gets added -eta/nbExamples * (delta + previousDelta)
    for(int i = 0; i < totalNeurons; i++) {
        Neuron* neuron = network->neurons[i];
        for(int j = 0; j < neuron->nbWeights; j++) {
            previousDeltaW[i][j] += deltaW[i][j];
            neuron->weights[j] += scale * previousDeltaW[i][j];
        }
        previousDeltaB[i] += deltaB[i];
        neuron->biais += scale * previousDeltaB[i];
    }

    //Frees the memory used for the gradient
    for(int i = 0; i < totalNeurons; i++)
        free(deltaW[i]);
    free(deltaB);
    free(deltaW);
}

/** This function completely handles a learning process with costs saving
 * It will make the network learn with batches of trainingBatchSize examples 
 * taken from trnExamples, and note its performances during the process.
 * maxTimeMS is the max time the learning should take in seconds.
 * When this time is reached, the learning stops when the current batch finishes
 * The costs will be recorded every 500 batches with a batch of 
 * costBatchSize examples (with testExamples and with trainExamples). The
 * results will be saved in trnCostsPath and tstCostsPath. */
void learnSavingCosts(Network* network, long maxTimeSec,
                      TrainingExample** trnExamples, int nbTrn,
                      TrainingExample** tstExamples, int nbTst,
                      char* trnCostsPath, char* tstCostsPath,
                      int trainingBatchSize, int costBatchSize) {

    /*
    int totalNeurons = getTotalNeurons(network);
    double** deltaW = malloc(sizeof(double*)*totalNeurons);
    for(int i = 0; i < totalNeurons; i++)
        deltaW[i] = calloc(network->neurons[i]->nbWeights, sizeof(double));
    double* deltaB = calloc(totalNeurons, sizeof(double));
    */
    
    FILE* trnFile = fopen(trnCostsPath, "a+");
    FILE* tstFile = fopen(tstCostsPath, "a+");
     
    clock_t begin = clock();
    int i = 0;
    while((double)(clock() - begin)/CLOCKS_PER_SEC < maxTimeSec) {
    
        // Learns
        TrainingExample** subset = getExamplesSubset(trnExamples, nbTrn, trainingBatchSize);
        learnOneBatch(network, subset, trainingBatchSize);
        //                  deltaW, deltaB);
        free(subset);

        if(i % 100 == 0) {
            //Gets the cost on training
            subset = getExamplesSubset(trnExamples, nbTrn, costBatchSize);
            double cost = network_cost(network, subset, costBatchSize);
            fprintf(trnFile, "%lf\n", cost);
            free(subset);
            //Gets the cost on testing
            subset = getExamplesSubset(tstExamples, nbTst, costBatchSize);
            cost = network_cost(network, subset, costBatchSize);
            fprintf(tstFile, "%lf\n", cost);
            free(subset);
        }
        i++;
    }
    fclose(trnFile);
    fclose(tstFile);
}

void learn(Network* network, long maxTimeSec,
           TrainingExample** trnExamples, int nbTrn,
           int batchSize) {

    clock_t begin = clock();
    while((double)(clock() - begin)/CLOCKS_PER_SEC < maxTimeSec) {
        TrainingExample** subset = getExamplesSubset(trnExamples, nbTrn, batchSize);
        learnOneBatch(network, subset, batchSize);
        free(subset);
    }
}

/** Calculates the sum of the costs of all the output neurons */
double network_cost(Network* network, TrainingExample** examples, int nbExamples) {

    //Creates an array to store the cost for each output neuron
    int nbOutputs = getOutputNumber(network);
    double* costs = malloc(sizeof(double)*nbOutputs);
    for(int i = 0; i < nbOutputs; i++)
        costs[i] = 0;

    //For each example, for each neuron, add the square of the difference 
    //between the expected output and the given output to the cost
    for(int example = 0; example < nbExamples; example++) {
        double* output = getNetworkAnswer(network, examples[example]->inputs);
        for (int outputNeuron = 0; outputNeuron < nbOutputs; outputNeuron++)
            costs[outputNeuron] += 
                quadratic_cost(
                    output[outputNeuron], 
                    examples[example]->label[outputNeuron]);
        free(output);
    }

    for(int i = 0; i < nbOutputs; i++)
        costs[i] /= (double)nbExamples;

    double cost = 0;
    for(int i = 0; i < nbOutputs; i++)
        cost += costs[i];

    free(costs);

    return cost;
}
