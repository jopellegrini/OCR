#ifndef SAVING_H
#define SAVING_H

#include "network.h"
#include "neuron.h"

void saveNetwork(Network* network, char* path);
Network* readNetwork(char* path);

char* serializeNetwork(Network* network);
Network* parseNetwork(char* data);

#endif //SAVING_H
