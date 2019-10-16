#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void test(Network* network, int i1, int i2) {
    float input[] = {(float) i1,(float) i2};
    printf("Inputs: %i %i -> %f\n", i1, i2, getNetworkAnswer(network, input)[0]);
}

void xor(Network* network) {
    test(network, 0, 0);
    test(network, 0, 1);
    test(network, 1, 0);
    test(network, 1, 1);
}

float xor_cost(Network* network) {

    float* i1 = malloc(sizeof(float)*2); i1[0] = 0.0f; i1[1] = 0.0f;
    float* i2 = malloc(sizeof(float)*2); i2[0] = 0.0f; i2[1] = 1.0f;
    float* i3 = malloc(sizeof(float)*2); i3[0] = 1.0f; i3[1] = 0.0f;
    float* i4 = malloc(sizeof(float)*2); i4[0] = 1.0f; i4[1] = 1.0f;
    float** inputs = malloc(sizeof(float*)*4);
    inputs[0] = i1; inputs[1] = i2; inputs[2] = i3; inputs[3] = i4;
    float* l1 = malloc(sizeof(float)*1); l1[0] = 0.0f;
    float* l2 = malloc(sizeof(float)*1); l2[0] = 1.0f;
    float* l3 = malloc(sizeof(float)*1); l3[0] = 1.0f;
    float* l4 = malloc(sizeof(float)*1); l4[0] = 0.0f;
    float** labels = malloc(sizeof(float*)*4);
    labels[0] = l1; labels[1] = l2; labels[2] = l3; labels[3] = l4;

    float* resarray = cost(network, 4, inputs, labels);
    float res = resarray[0];
    free(resarray);

    free(i1); free(i2); free(i3); free(i4); free(l1); free(l2); free(l3); free(l4);
    free(inputs); free(labels);

    return res;
}

void learn_xor(Network* network) {

    float i1[] = {0,0};
    float i2[] = {0,1};
    float i3[] = {1,0};
    float i4[] = {1,1};
    float l1[] = {0};
    float l2[] = {1};
    float l3[] = {1};
    float l4[] = {0};
    TrainingExample** examples = malloc(sizeof(TrainingExample*)*4);
    examples[0] = newTrainingExample(i1, l1);
    examples[1] = newTrainingExample(i2, l2);
    examples[2] = newTrainingExample(i3, l3);
    examples[3] = newTrainingExample(i4, l4);

    learn(network, 1.0f, examples, 4);
}

int main() {

    //Initialises the rand() function to generate random numbers
    srand((unsigned int)time(NULL));

    int layers[] = {2,2,2,1};
    Network* network = NULL;
    network = newNetwork(layers, 4);
    xor(network);
    printf("Cost: %f\n\n", xor_cost(network));

    printf("Learning...\n\n");
    for(int i = 0; i < 100000; i++)
        learn_xor(network);

    xor(network);
    printf("Cost: %f\n", xor_cost(network));
    printf("\n");
    printf("%s\n", serializeNetwork(network));

    return 0;
}
