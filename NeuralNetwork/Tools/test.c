#include "../network.h"
#include "../training_example.h"
#include "../saving.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char** argv) {

    srand((unsigned int)time(NULL));

    if(argc < 2 || argc > 3)
        err(1, "Please pass a network and a database in parameter");

    while(1) {

    int nbEx = 0;
    TrainingExample** exArray = readDatabase(argv[2], &nbEx);
    TrainingExample* ex = getExamplesSubset(exArray, nbEx, 1)[0];

    Network* network = readNetwork(argv[1]);

    double* outputs = getNetworkAnswer(network, ex->inputs);

    //Displays the image in the terminal
    for(int y = 0; y < 28; y++) {
        for(int x = 0; x < 28; x++)
            printf("%c", ex->inputs[y*28+x] ? 'O' : '.');
        printf("\n");
    }
        
    //Displays the label and the networks answer
    int nbOut = getOutputNumber(network);
    //for(int i = 0; i < nbOut; i++)
        //printf("[%i]: %lf\n", i, outputs[i]);
    int answer = getMaxIndex(outputs, nbOut);
    int label = getMaxIndex(ex->label, nbOut);
    printf("Answer: %i\n", answer);
    printf("Label:  %i\n", label);
    printf(answer == label ? "\x1b[44mOK\x1b[0m\n" : "\x1b[41mKO\x1b[0m\n");

    if(answer != label)
        break;
    }

    return 0;
}
