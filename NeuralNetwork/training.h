#ifndef TRAINING_H
#define TRAINING_H

#include "network.h"
#include "neuron.h"
#include "training_example.h"

void learnOneBatch(Network* network, TrainingExample** examples, int nbExamples) ;

void learnWithMomentum(Network* network, 
                       TrainingExample** examples, int nbExamples, 
                       double** previousDeltaW, double* previousDeltaB) ;

void learnSavingCosts(Network* network, long maxTimeSec,
                      TrainingExample** trnExamples, int nbTrn,
                      TrainingExample** tstExamples, int nbTst,
                      char* trnCostsPath, char* tstCostsPath,
                      int trainingBatchSize, int costBatchSize) ;

void learn(Network* network, long maxTimeSec,
           TrainingExample** trnExamples, int nbTrn,
           int batchSize) ;

double network_cost(Network* network, TrainingExample** examples, int nbExamples) ;

#endif //TRAINING_H
