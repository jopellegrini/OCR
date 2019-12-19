#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "network.h"
#include "saving.h"
#include "training.h"
#include "training_example.h"

double getSuccessRate(Network* net, TrainingExample** examples, int nbExamples) {

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

int main() {

    //Initialises the rand() function to generate random numbers
    srand((unsigned int)time(NULL));

    /*
    int layers[] = {784, 100, 70};
    Network* network = NULL;
    network = newNetwork(layers, 3);
    */
    Network* network = readNetwork("Tools/Data/OCR_network_v2");

    int nbTraining = 0;
    TrainingExample** trainingExamples = readDatabase("Tools/Data/letters_database_v2", &nbTraining);

    printf("Success rate: ");
    printf("%f\n", getSuccessRate(network, trainingExamples, nbTraining));

    printf("Learning...\n");
    
    learnSavingCosts(network, 60,
          trainingExamples, nbTraining,
          trainingExamples, nbTraining,
          "Tools/Data/trnCosts", "Tools/Data/tstCosts",
          40, 1);

    printf("Success rate: ");
    printf("%f\n", getSuccessRate(network, trainingExamples, nbTraining));
    
    printf("Cost: %lf\n", network_cost(network, trainingExamples, nbTraining));

    saveNetwork(network, "Tools/Data/OCR_network_v2");

    return 0;
}
