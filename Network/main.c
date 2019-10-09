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

int main() {

    //Initialises the rand() function to generate random numbers
    srand((unsigned int)time(NULL));

    int layers[] = {2,2,2,1};
    Network* network = NULL;
    do {
        if(network != NULL) destroyNetwork(network);
        network = newNetwork(layers, 4);
    } while(xor_cost(network) > 0.1f);

    printf("%s\n", serialize(network));

    xor(network);
    printf("Cost: %f", xor_cost(network));

    return 0;
}
