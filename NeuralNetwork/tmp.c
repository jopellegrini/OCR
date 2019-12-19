#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "network.h"
#include "saving.h"
#include "training.h"
#include "training_example.h"

double getS_uccessRate(Network* net, TrainingExample** examples, int nbExamples) {

    int nbSuccess = 0;

    for(int i = 0; i < nbExamples; i++) {
        int wanted = getMaxIndex(examples[i]->label, getOutputNumber(net));
        int answer = getMaxIndex(
                getNetworkAnswer(net, examples[i]->inputs), getOutputNumber(net)
        );
        if(answer == wanted)
            nbSuccess++;
    }

    return (double)nbSuccess/(double)nbExamples;
}

int ____main() {

    //Initialises the rand() function to generate random numbers
    srand((unsigned int)time(NULL));

    
    int layers[] = {784, 100, 62};
    Network* network = NULL;
    network = newNetwork(layers, 3);
    //Network* network = readNetwork("Tools/Data/OCR_network");

    int nbTraining = 0;
    TrainingExample** trainingExamples = readDatabase("Tools/Data/DinOCRDatabase", &nbTraining);

    printf("%i\n", nbTraining);

    printf("Success rate: ");
    printf("%f\n", getS_uccessRate(network, trainingExamples, nbTraining));

    printf("Learning...\n");
    
    learn(network, 5,
          trainingExamples, nbTraining,
          10);

    printf("Success rate: ");
    printf("%f\n", getS_uccessRate(network, trainingExamples, nbTraining));
    
    printf("Cost: %lf\n", network_cost(network, trainingExamples, nbTraining));

    saveNetwork(network, "Tools/Data/DinOCR_network");

    return 0;
}
