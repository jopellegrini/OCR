#ifndef LETTERS_CLASSIFIER_H
#define LETTERS_CLASSIFIER_H

double* scaleForNetwork(int* source, int width, int height) ;
char charFromNeuronIndex(int index) ;
int neuronIndexFromChar(char c) ;
void initClassifier(char* networkPath) ;
char classifyImage(int* image, int imageWidth, int imageHeight) ;

#endif //LETTERS_CLASSIFIER_H
